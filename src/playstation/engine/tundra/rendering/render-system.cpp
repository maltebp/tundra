#include "tundra/assets/texture/texture-asset.hpp"
#include "tundra/core/math.hpp"
#include "tundra/core/vec/vec3.dec.hpp"
#include "tundra/rendering/ordering-table-layer.hpp"
#include "tundra/rendering/primitive-buffer.hpp"
#include "tundra/rendering/vram-allocator.hpp"
#include <tundra/rendering/render-system.hpp>

#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>

#include <tundra/core/assert.hpp>
#include <tundra/core/vec/vec2.hpp>
#include <tundra/core/mat/mat3x3.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/grid-allocator.hpp>
#include <tundra/core/log.hpp>
#include <tundra/core/mat/mat3x3.dec.hpp>
#include <tundra/core/types.hpp>

#include <tundra/assets/texture/texture-asset-load-info.hpp>

#include <tundra/engine/transform-matrix.hpp>

#include <tundra/rendering/camera.hpp>
#include <tundra/rendering/model.hpp>
#include <tundra/rendering/sprite.hpp>
#include <tundra/rendering/text.hpp>
#include <tundra/rendering/double-buffer-id.hpp>
#include <tundra/rendering/ordering-table-node.hpp>

#include <tundra/gte/cast.hpp>
#include <tundra/gte/compute-transform.hpp>
#include <tundra/gte/operations.hpp>


#define set_zsf3( zsf3 ) __asm__ volatile ( \
	"ctc2	%0, $29;"		\
	:						\
	: "r"( zsf3 ) )

#define set_zsf4( zsf4 ) __asm__ volatile ( \
	"ctc2	%0, $30;"		\
	:						\
	: "r"( zsf4 ) )

namespace td {

    namespace internal {

        // TODO: This should really be refactors
        volatile td::Duration last_draw_sync_time;
        ITime* g_time = nullptr;

        void draw_sync_callback() {
            if( g_time != nullptr ) {
                last_draw_sync_time = g_time->get_duration_since_start();
            }
        }

        const uint16 SCREEN_WIDTH = 320;
        const uint16 SCREEN_HEIGHT = 240;

        SVECTOR* vec3_int16_as_svector(const ::Vec3<td::int16>& vec3) {
            return const_cast<SVECTOR*>(reinterpret_cast<const SVECTOR*>(&vec3));
        };

        bool screen_triangle_is_in_screen(const DVECTOR* v0, const DVECTOR* v1, const DVECTOR* v2);

        template<typename TPrim>
        void set_prim_color(TPrim* prim, const Vec3<uint8>& color) {
            prim->r0 = color.x;
            prim->g0 = color.y;
            prim->b0 = color.z;
        }

        template<typename TPrim>
        void set_prim_color_3(TPrim* prim, const Vec3<uint8>& color) {
            prim->r0 = color.x;
            prim->g0 = color.y;
            prim->b0 = color.z;

            prim->r1 = color.x;
            prim->g1 = color.y;
            prim->b1 = color.z;

            prim->r2 = color.x;
            prim->g2 = color.y;
            prim->b2 = color.z;
        }

        template<typename TPrim>
        void set_prim_3_vertices(TPrim* prim, const DVECTOR& v0, const DVECTOR& v1, const DVECTOR v2) {
            *(DVECTOR*)(&prim->x0) = v0;
            *(DVECTOR*)(&prim->x1) = v1;
            *(DVECTOR*)(&prim->x2) = v2;
        }

        void compute_lit_color(uint8_t& outpout_color, const ::Vec3<td::int16>& normal) {
            gte_ldrgb( &outpout_color );

            SVECTOR* raw_normal = vec3_int16_as_svector(normal);
            gte_ldv0( raw_normal );
            
            // Compute color
            gte_nccs();

            // Retrieve color
            gte_strgb( &outpout_color );    
        };

        template<typename TPrim>
        inline void set_texture_3(TPrim* prim, const TextureAsset* texture, const td::Vec2<uint8>& uv0, const td::Vec2<uint8>& uv1, const td::Vec2<uint8>& uv2) {
            prim->tpage = texture->load_info->texture_page_id;
            prim->clut = texture->load_info->clut_id;

            prim->u0 = uv0.x;
            prim->v0 = uv0.y;

            prim->u1 = uv1.x;
            prim->v1 = uv1.y;

            prim->u2 = uv2.x;
            prim->v2 = uv2.y;
        }

        template<typename TPrim>
        inline void initialize_prim(TPrim& prim);

        template<> void initialize_prim<POLY_F3>(POLY_F3& prim) { setPolyF3(&prim); }
        template<> void initialize_prim<POLY_FT3>(POLY_FT3& prim) { setPolyFT3(&prim); }
        template<> void initialize_prim<POLY_G3>(POLY_G3& prim) { setPolyG3(&prim); }
        template<> void initialize_prim<POLY_GT3>(POLY_GT3& prim) { setPolyGT3(&prim); }

        template<typename TPrim>
        TPrim* allocate_prim(PrimitiveBuffer& primitive_buffer, OrderingTableLayer& ordering_table_layer, uint16 ordering_table_index) {
            TPrim* prim = (TPrim*)primitive_buffer.allocate(sizeof(TPrim));
            if( prim == nullptr ) {
                TD_DEBUG_LOG("Primitive is out of memory when allocating prim");
                return nullptr;
            }
            initialize_prim<TPrim>(*prim);
            ordering_table_layer.add_node((OrderingTableNode*)prim, ordering_table_index);
            return prim;
        }

    }

    RenderSystem::RenderSystem(
        ITime& time,
        VramAllocator& vram_allocator,
        uint32 primitive_buffer_size
    )
        :   frame_buffer_positions{
                vram_allocator.frame_buffer_1_position, 
                vram_allocator.frame_buffer_2_position
            },
            time(time),
            primitive_buffers{primitive_buffer_size, primitive_buffer_size},
            clear_color(128),
            ambient_color(128),
            light_directions(0), // All lights disabled by default
            light_colors(128) // Default all values to some gray
    { 
        GridAllocator::Result font_vram_allocation = vram_allocator
            .get_global_allocator()
            .allocate(64, 65, 7, 8);
        
        TD_ASSERT(font_vram_allocation.success, "Failed at allocate vram for font");
        
        // FntLoad(font_vram_allocation.position.x, font_vram_allocation.position.y); 
        FntLoad(font_vram_allocation.position.x >> 1, font_vram_allocation.position.y);

        DrawSyncCallback(internal::draw_sync_callback);
        internal::g_time = &time;
    }

    void RenderSystem::render() {
        
        td::Duration submit_start = time.get_duration_since_start();
        gte_SetBackColor( this->ambient_color.x, this->ambient_color.y, this->ambient_color.z );
        gte_SetColorMatrix(&gte::to_gte_matrix_ref(light_colors));

        primitive_buffers[(uint8)active_buffer].clear();

        num_triangles_rendered = 0;

        for(td::Camera* camera : Camera::get_all()) {
            camera->ordering_tables[(uint8)active_buffer].clear();
            this->render_camera(camera);
        }

        submit_duration = time.get_duration_since_start() - submit_start;

        // Flush
        DrawSync(0);
        td::Duration non_volatile_duration = internal::last_draw_sync_time;
        last_frame_draw_duration = non_volatile_duration - last_frame_draw_start;

	    VSync(0);
        
        // The "active" render buffer is the one being displayed, and the one whose
        // ordering table and primitive buffer we are submitting to.
        // The inactive render buffer is drawing it's ordering-table to its framebuffer

        last_frame_draw_start = time.get_duration_since_start();

        DoubleBufferId inactive_buffer = active_buffer == DoubleBufferId::First ? DoubleBufferId::Second : DoubleBufferId::First;

        Vec2<uint16> display_vram_position = frame_buffer_positions[(uint8)inactive_buffer];
        DISPENV display_env;
        SetDefDispEnv( &display_env, display_vram_position.x, display_vram_position.y, internal::SCREEN_WIDTH, internal::SCREEN_HEIGHT);
        PutDispEnv( &display_env );        

        Vec2<uint16> draw_vram_position = frame_buffer_positions[(uint8)active_buffer];
        DRAWENV draw_env;
        SetDefDrawEnv(&draw_env, draw_vram_position.x, display_vram_position.y, internal::SCREEN_WIDTH, internal::SCREEN_HEIGHT);
        draw_env.isbg = 1;
        draw_env.dtd = 0;
        draw_env.r0 = clear_color.x;
        draw_env.g0 = clear_color.y;
        draw_env.b0 = clear_color.z;
        PutDrawEnv( &draw_env );

        internal::last_draw_sync_time.microseconds = 0;

        for(td::Camera* camera : Camera::get_all()) {
            
            // TODO: Queue the drawing of the camera (now drawing a second will block while waiting)
            const td::OrderingTableNode* first_ordering_table_node_to_draw =
                camera->ordering_tables[(uint8)active_buffer].get_first_node_to_draw();
            DrawOTag((const td::uint32*)first_ordering_table_node_to_draw);
        }

        // We now submit to the ordering table of the buffer being displayed,
        // while the other one is being drawn to and not displayed
        active_buffer = inactive_buffer;
    }

    void RenderSystem::set_clear_color(Vec3<uint8> color) {
        this->clear_color = color;
    }

    void RenderSystem::set_ambient_light(Vec3<uint8> color) {
        this->ambient_color = color;
    }

    void RenderSystem::set_light_direction(uint8 light_index, Vec3<Fixed16<12>> direction) {
        TD_ASSERT(light_index < 3, "Light index must be between 0 and 2 (was %d)", light_index);
        light_directions.set_row(light_index, -direction);
    }

    void RenderSystem::set_light_color(uint8 light_index, Vec3<uint8> color) {
        TD_ASSERT(light_index < 3, "Light index must be between 0 and 2 (was %d)", light_index);
        
        // TODO: This could be a multiplication instead
        light_colors.set_column(
            light_index,
            Vec3<Fixed16<12>>( Vec3<Fixed32<12>>{color} >> 8 ));
    }

    void RenderSystem::render_camera(Camera* camera) {

        TD_ASSERT(camera->transform != nullptr, "Camera's transform is nullptr");

        const TransformMatrix& camera_matrix = gte::compute_camera_matrix(camera);

        for( Model* model : Model::get_all() ) {

            if( !camera->layers_to_render.contains(model->layer_index) ) continue;

            OrderingTableLayer& layer = camera->get_ordering_table_layer(active_buffer, model->layer_index);
            
            render_model(camera_matrix, model, layer);
        }

        for( Sprite* sprite : Sprite::get_all() ) {

            if( !camera->layers_to_render.contains(sprite->layer_index) ) continue;

            OrderingTableLayer& layer = camera->get_ordering_table_layer(active_buffer, sprite->layer_index);
            
            render_sprite(sprite, layer);
        }

        for( Text* text : Text::get_all() ) {

            if( !camera->layers_to_render.contains(text->layer_index) ) continue;

            OrderingTableLayer& layer = camera->get_ordering_table_layer(active_buffer, text->layer_index);
            
            render_text(text, layer);
        }
    }

    void RenderSystem::render_sprite(const Sprite* sprite, OrderingTableLayer& ordering_table_layer) {

        if( !sprite->enabled ) return;
        if( sprite->texture == nullptr ) return
        
        TD_ASSERT(sprite->size.x > 0 && sprite->size.y > 0, "Sprite size must be larger than 0");

        // OPTIMIZATION: Sprite rendering is not optimized at all (there is no specialized transform, nor transform computation for it)

        Vec2<Fixed32<12>> half_size {
            Fixed32<12>::from_raw_fixed_value(sprite->size.x.get_raw_value() >> 1),
            Fixed32<12>::from_raw_fixed_value(sprite->size.y.get_raw_value() >> 1)
        };

        Vec2<Fixed32<12>> v0;
        Vec2<Fixed32<12>> v1;
        Vec2<Fixed32<12>> v2;
        Vec2<Fixed32<12>> v3;

        if( sprite->rotation == 0 ) {
            v0 = { -half_size.x, half_size .y }; // Top left
            v1 = { half_size.x, half_size .y }; // Top right
            v2 = { -half_size.x, -half_size .y }; // Bottom left
            v3 = { half_size.x,  -half_size.y }; // Bottom right
        } 
        else {
            // OPTIMIZATION: I am sure we can make this calculation faster
            td::Fixed32<12> c = cos(sprite->rotation);
            td::Fixed32<12> s = sin(sprite->rotation);

            // We only have to calculate one point, and then we can rotate that by 90 degrees
            
            // Rotating top-left by 
            v0 = {
                c * -half_size.x - s * half_size.y,
                s * -half_size.x + c * half_size.y
            };

            v1 = { v0.y, -v0.x }; // Top Right
            v3 = { v1.y, -v1.x }; // Bottom right
            v2 = { v3.y, -v3.x }; // Bottom left
        }

        // Flip so we point y-axis downwards (i.e. screen space)
        v0.y = -v0.y;
        v1.y = -v1.y;
        v2.y = -v2.y;
        v3.y = -v3.y;
        
        // OPTIMIZATION: If rotation is 0 we should draw it as a PS Sprite instead (axis-aligned)
        
        v0 += { sprite->position.x, sprite->position.y };
        v1 += { sprite->position.x, sprite->position.y };
        v2 += { sprite->position.x, sprite->position.y };
        v3 += { sprite->position.x, sprite->position.y };

        POLY_FT4* primitive = (POLY_FT4*)primitive_buffers[(uint8)active_buffer].allocate(sizeof(POLY_FT4));
        if( primitive == nullptr ) {
            TD_DEBUG_LOG("Not enough space for Sprite primitive");
            return;
        }

        setPolyFT4(primitive);

        // TODO: Should there be z-depth to sprites?
        ordering_table_layer.add_to_front((OrderingTableNode*)primitive);
        
        // We use static cast to preserve the bit pattern (I believe the primitive members are
        // wrongly unsigned values - they should be signed, but this is only the case for x0, y0)
        primitive->x0 = static_cast<uint16>(v0.x.get_raw_integer());
        primitive->y0 = static_cast<uint16>(v0.y.get_raw_integer());

        primitive->x1 = (int16)v1.x.get_raw_integer();
        primitive->y1 = (int16)v1.y.get_raw_integer();

        primitive->x2 = (int16)v2.x.get_raw_integer();
        primitive->y2 = (int16)v2.y.get_raw_integer();

        primitive->x3 = (int16)v3.x.get_raw_integer();
        primitive->y3 = (int16)v3.y.get_raw_integer();
        
        primitive->tpage = sprite->texture->load_info->texture_page_id;
        primitive->clut = sprite->texture->load_info->clut_id;
       
        uint8 uv_u0 = sprite->texture->load_info->texture_page_offset.x;
        uint8 uv_v0 = sprite->texture->load_info->texture_page_offset.y;

        uint8 uv_u1 = sprite->texture->load_info->texture_page_offset.x + (uint8)(sprite->texture->pixels_width - 1U);
        uint8 uv_v1 = sprite->texture->load_info->texture_page_offset.y + (uint8)(sprite->texture->pixels_height - 1U);

        primitive->u0 = uv_u0;
        primitive->v0 = uv_v0;

        primitive->u1 = uv_u1;
        primitive->v1 = uv_v0;

        primitive->u2 = uv_u0;
        primitive->v2 = uv_v1;

        primitive->u3 = uv_u1;
        primitive->v3 = uv_v1;

        primitive->r0 = sprite->color.x >> 1;
        primitive->g0 = sprite->color.y >> 1;
        primitive->b0 = sprite->color.z >> 1;

        num_triangles_rendered += 2;
    }

    void RenderSystem::render_model(const TransformMatrix& camera_matrix, const Model* model, OrderingTableLayer& ordering_table_layer) {

        // TODO: Add radius cull distance

        const TransformMatrix& model_matrix = gte::compute_world_matrix(model->transform);
        TransformMatrix model_to_view_matrix = gte::multiply_transform_matrices(camera_matrix, model_matrix);

        // Multiplying the light directions as row vectors (which they are) with the
        // model matrix is equivalent to multiplying wih transpose (i.e. inverse) of
        // model matrix. This means light directions are now in model-space, so we
        // don't have to transform each normal of the face.
        // Note: This sets the rotation matrix registers, so it overrides SetRotMatrix
        Mat3x3<Fixed16<12>> model_rotation_matrix = gte::extract_rotation_matrix(model_matrix);
        Mat3x3<Fixed16<12>> light_directions_in_model_space =
            gte::multiply(light_directions, model_rotation_matrix);
        gte_SetLightMatrix( &gte::to_gte_matrix_ref(light_directions_in_model_space) );
        // TODO: Optimization: we could avoid doing all of this if there are no lights enabled, but
        // I think the far most likely scenario is that at least one light is enabled.
        
        gte_SetRotMatrix( &gte::to_gte_matrix_ref(model_to_view_matrix) );
        gte_SetTransMatrix( &gte::to_gte_matrix_ref(model_to_view_matrix) );

        volatile uint16 z_map_factor_3 = ordering_table_layer.get_z_map_factor_3();
        set_zsf3(z_map_factor_3);

        // TODO: I think this yields the wrong value for some reason
        volatile uint16 z_map_factor_4 = ordering_table_layer.get_z_map_factor_4();
        set_zsf4(z_map_factor_4);

        for( int part_index = 0; part_index < model->asset.num_parts; part_index++ ) {

            td::ModelPart* model_part = model->asset.model_parts[part_index];

            const bool part_has_texture = model_part->texture_index > 0;

            // We do not use the asset's diffuse color if it has a texture
            Vec3<uint8> part_real_color =
                part_has_texture ?
                td::Vec3<uint8>{255} : 
                td::Vec3<uint8>{model_part->color.x, model_part->color.y, model_part->color.z};
                
            if( model->color != Vec3<uint8>{255} ) {

                Vec3<UFixed32<12>> color_multiplier {
                    UFixed32<12>::from_raw_fixed_value((uint16)(UFixed32<12>{model->color.x}.get_raw_value() >> 8)),
                    UFixed32<12>::from_raw_fixed_value((uint16)(UFixed32<12>{model->color.y}.get_raw_value() >> 8)),
                    UFixed32<12>::from_raw_fixed_value((uint16)(UFixed32<12>{model->color.z}.get_raw_value() >> 8))
                };

                part_real_color = {
                    (uint8)((color_multiplier.x * part_real_color.x).get_raw_integer()),
                    (uint8)((color_multiplier.y * part_real_color.y).get_raw_integer()),
                    (uint8)((color_multiplier.z * part_real_color.z).get_raw_integer())
                };
            }

            for( int i = 0; i < model_part->num_triangles; i++ ) {

                auto vec3_int16_as_svector = [](const ::Vec3<td::int16>& vec3) {
                    return const_cast<SVECTOR*>(reinterpret_cast<const SVECTOR*>(&vec3));
                };

                // TODO: Change this when Model uses proper td::Vec3
                SVECTOR* mv0 = vec3_int16_as_svector(model->asset.vertices[model_part->vertex_indices[i].x - 1]);
                SVECTOR* mv1 = vec3_int16_as_svector(model->asset.vertices[model_part->vertex_indices[i].y - 1]);
                SVECTOR* mv2 = vec3_int16_as_svector(model->asset.vertices[model_part->vertex_indices[i].z - 1]);
            
                /* Load the first 3 vertices of a quad to the GTE */
                gte_ldv3(
                    mv0, mv1, mv2                 
                );
                    
                /* Rotation, Translation and Perspective Triple */
                gte_rtpt();
                
                // Compute normal clip for backface culling
                gte_nclip();
                int backface_sign;
                gte_stopz( &backface_sign );
                bool face_is_backfacing = backface_sign < 0;
                if( face_is_backfacing ) continue;
            
                /* Calculate average Z for depth sorting */
                gte_avsz3();

                // Typically, you'd use gte_stotz to get the average z, but this is
                // a 16-bit value, so many values above 2^16 would return a z-value
                // that is actually within the near- and far-plane becaus, so we
                // cannot detect that they are outside the far plane

                td::uint32 ordering_table_index_fixed_20_12;
                gte_stopz(&ordering_table_index_fixed_20_12);

                td::uint32 ordering_table_index = ordering_table_index_fixed_20_12 >> 12;

                // In front, or on, near-plane
                if( ordering_table_index == 0 ) {
                    // TD_DEBUG_LOG("Behind camera");
                    continue;
                }

                // Behind far-plane
                if( ordering_table_index >= ordering_table_layer.get_resolution() ) {
                    // TD_DEBUG_LOG("Too far away");
                    continue;
                } 

                uint16 ordering_table_index_16 = (uint16)ordering_table_index;
            
                DVECTOR v0;
                DVECTOR v1;
                DVECTOR v2;

                /* Set the projected vertices to the primitive */
                gte_stsxy0( &v0 );
                gte_stsxy1( &v1 );
                gte_stsxy2( &v2 );
            
                if( !internal::screen_triangle_is_in_screen(&v0, &v1, &v2)) {
                    //TD_DEBUG_LOG("Not in screen");
                    continue;
                }        
                
                if( !part_has_texture ) {
                    if( !model_part->is_smooth_shaded ) {

                        POLY_F3* prim = internal::allocate_prim<POLY_F3>(
                            primitive_buffers[(uint8)active_buffer],
                            ordering_table_layer,
                            ordering_table_index_16);

                        if( prim == nullptr) return;

                        internal::set_prim_3_vertices(prim, v0, v1, v2);
                        internal::set_prim_color(prim, part_real_color);

                        if( model->is_lit ) {
                            internal::compute_lit_color(prim->r0, model->asset.normals[model_part->normal_indices[i].x - 1]);
                        } 
                        
                        num_triangles_rendered++;
                    }
                    else
                    {
                        POLY_G3* prim = internal::allocate_prim<POLY_G3>(
                            primitive_buffers[(uint8)active_buffer],
                            ordering_table_layer,
                            ordering_table_index_16);

                        if( prim == nullptr) return;

                        internal::set_prim_3_vertices(prim, v0, v1, v2);
                        internal::set_prim_color_3(prim, part_real_color);

                        if( model->is_lit ) {
                            internal::compute_lit_color(prim->r0, model->asset.normals[model_part->normal_indices[i].x - 1]);
                            internal::compute_lit_color(prim->r1, model->asset.normals[model_part->normal_indices[i].y - 1]);
                            internal::compute_lit_color(prim->r2, model->asset.normals[model_part->normal_indices[i].z - 1]);    
                        }
                        
                        num_triangles_rendered++;
                    }
                }
                else {
                    if( !model_part->is_smooth_shaded ) {

                        POLY_FT3* prim = internal::allocate_prim<POLY_FT3>(
                            primitive_buffers[(uint8)active_buffer],
                            ordering_table_layer,
                            ordering_table_index_16);

                        if( prim == nullptr) return;

                        internal::set_texture_3(
                            prim, model->asset.texture, 
                            model->asset.mapped_uvs[model_part->uv_indices[i].x - 1],
                            model->asset.mapped_uvs[model_part->uv_indices[i].y - 1],
                            model->asset.mapped_uvs[model_part->uv_indices[i].z - 1]
                        );

                        Vec3<uint8> adjusted_color = part_real_color >> 1;

                        internal::set_prim_3_vertices(prim, v0, v1, v2);
                        internal::set_prim_color(prim, adjusted_color);

                        if( model->is_lit) {
                            internal::compute_lit_color(prim->r0, model->asset.normals[model_part->normal_indices[i].x - 1]);
                        }
                        
                        num_triangles_rendered++;
                    }
                    else
                    {
                        POLY_GT3* prim = internal::allocate_prim<POLY_GT3>(
                            primitive_buffers[(uint8)active_buffer],
                            ordering_table_layer,
                            ordering_table_index_16);

                        if( prim == nullptr) return;

                        internal::set_texture_3(
                            prim, model->asset.texture, 
                            model->asset.mapped_uvs[model_part->uv_indices[i].x - 1],
                            model->asset.mapped_uvs[model_part->uv_indices[i].y - 1],
                            model->asset.mapped_uvs[model_part->uv_indices[i].z - 1]
                        );

                        Vec3<uint8> adjusted_color = part_real_color >> 1;

                        internal::set_prim_3_vertices(prim, v0, v1, v2);
                        internal::set_prim_color_3(prim, adjusted_color);

                        if( model->is_lit ) {
                            internal::compute_lit_color(prim->r0, model->asset.normals[model_part->normal_indices[i].x - 1]);
                            internal::compute_lit_color(prim->r1, model->asset.normals[model_part->normal_indices[i].y - 1]);
                            internal::compute_lit_color(prim->r2, model->asset.normals[model_part->normal_indices[i].z - 1]);
                        }
                        
                        num_triangles_rendered++;
                    }
                }   
                
            }
        }
    }

    void RenderSystem::render_text(const Text* text, OrderingTableLayer& ordering_table_layer) {
        if( !text->is_enabled ) return;
        if( text->text.is_empty() ) return;
        
        ordering_table_layer.add_font_to_front(
            primitive_buffers[(uint8)active_buffer],
            text->position.x.get_raw_integer(),
            text->position.y.get_raw_integer(),
            text->text
        );

        num_triangles_rendered += text->text.get_size() * 2;
    }

    bool internal::screen_triangle_is_in_screen(
        const DVECTOR* v0, const DVECTOR* v1, const DVECTOR* v2
    ) {
        auto is_vertex_in_Screen = [&](const DVECTOR* v) -> bool {
            if( v->vx < 0 || v->vx > (int16)SCREEN_WIDTH ) return false;
            if( v->vy < 0 || v->vy > (int16)SCREEN_HEIGHT ) return false;
            return true;
        };

        return is_vertex_in_Screen(v0) || is_vertex_in_Screen(v1) || is_vertex_in_Screen(v2);
    }

}