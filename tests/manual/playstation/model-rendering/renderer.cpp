#include "renderer.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <psxgte.h>
#include <psxgpu.h>
#include <inline_c.h>

#include <tundra/assets/model/model-asset.hpp>

#include "globals.hpp"
#include "model.hpp"
#include "tundra/core/fixed.hpp"
#include "tundra/rendering/ordering-table-layer.hpp"

const int32_t CULL_DISTANCE = ONE * 20;

/* For easier handling of vertex indices */
typedef struct {
	short v0,v1,v2,v3;
} INDEX;

const int32_t HALF = ONE >> 1;

/* Cube vertices */
SVECTOR cube_verts[] = {
	{ -HALF, -HALF, -HALF, 0 },
	{ -HALF,  HALF, -HALF, 0 },
	{  HALF,  HALF, -HALF, 0 },
	{  HALF, -HALF, -HALF, 0 },
	{ -HALF, -HALF,  HALF, 0 },
	{ -HALF,  HALF,  HALF, 0 },
	{  HALF,  HALF,  HALF, 0 },
	{  HALF, -HALF,  HALF, 0 }
};

/* Cube face normals */
SVECTOR cube_norms[] = {
	{ 0, 0, -ONE, 0 }, // Front
	{ -ONE, 0, 0, 0 }, // Left
	{ 0, 0, ONE, 0 }, // Back
	{ ONE, 0, 0, 0 }, // Right
    { 0, -ONE, 0, 0 }, // Bottom
	{ 0, ONE, 0, 0 } // Top
};

/* Cube vertex indices */
INDEX cube_indices[] = {
	{ 0, 1, 3, 2 }, // Front
	{ 4, 5, 0, 1 }, // Left
	{ 7, 6, 4, 5 }, // Back
	{ 3, 2, 7, 6 }, // Right
	{ 4, 0, 7, 3 }, // Bottom
	{ 1, 5, 2, 6 }  // Top
};

/* Number of faces of cube */
#define CUBE_FACES 6

void Renderer::initialize(
    td::uint16 ordering_table_size,
    size_t primitives_buffer_size,
    int resolution_width, 
    int resolution_height,
    CVECTOR clear_color
) {
    this->resolution_width = resolution_width;
    this->resolution_height = resolution_height;

    render_buffer_1 = new RenderBuffer(
        ordering_table_size, primitives_buffer_size, 
        0, 0,
        resolution_width, 0,
        resolution_width, resolution_height,
        clear_color
    );

    render_buffer_2 = new RenderBuffer(
        ordering_table_size, primitives_buffer_size, 
        resolution_width, 0,
        0, 0,
        resolution_width, resolution_height,
         clear_color
    );

	render_buffer_1->clear();
    render_buffer_2->clear();

    active_render_buffer = render_buffer_1;
    render_buffer_1->activate();
	
	/* Set light ambient color and light color matrix */
	gte_SetBackColor( 140, 80, 60 );

    SetDispMask(1);

    FntLoad(0, 256); // TODO: Not sure how we determine where to store this    
}

void Renderer::draw_model(const Model& model) {
    
    int32_t model_x_distance_to_camera = model.position.vx - camera.get_position().vx;
    if( model_x_distance_to_camera < 0) model_x_distance_to_camera *= -1;
    if( model_x_distance_to_camera > CULL_DISTANCE) return;

    int32_t model_y_distance_to_camera = model.position.vy - camera.get_position().vy;
    if( model_y_distance_to_camera < 0) model_y_distance_to_camera *= -1;
    if( model_y_distance_to_camera > CULL_DISTANCE) return;

    if( light_colors_changed ) {
        gte_SetColorMatrix( &light_colors );
        light_colors_changed = false;
    }

    MATRIX size_matrix = {
        {
        {(int16_t)(model.scale.vx  >> MATH_SCALE), 0, 0},
        {0, (int16_t)(model.scale.vy >> MATH_SCALE), 0},
        {0, 0, (int16_t)(model.scale.vz >> MATH_SCALE)} },
        {0, 0, 0}
    };

    // Decided not to use this, because it requires some initialized matrix,
    // so might as well just set the matrix myself
    // ScaleMatrix(&size_matrix, box.size);

    MATRIX rotation_matrix;
    RotMatrix(const_cast<SVECTOR*>(&model.rotation), &rotation_matrix);

    // std::printf("Rotation: %d -> %d\n", model.rotation.vy, rotation_matrix.m[0][0]);

    MATRIX model_to_world;
    MulMatrix0(&rotation_matrix, &size_matrix, &model_to_world);
        
    VECTOR position {
        model.position.vx >> MATH_SCALE,
        model.position.vy >> MATH_SCALE,
        model.position.vz >> MATH_SCALE
    };

    TransMatrix( &model_to_world, &position );

    MATRIX world_to_view_matrix = camera.compute_matrix();
    
    MATRIX model_to_view_matrix = {{}, {}}; 
    CompMatrixLV(&world_to_view_matrix, &model_to_world, &model_to_view_matrix);

    MATRIX light_directions_in_model_space;

    // Multiplying the light directions as row vectors (which they are) with the
    // model matrix is equivalent to multiplying wih transpose (i.e. inverse) of
    // model matrix. This means light directions are now in model-space, so we
    // don't have to transform each normal of the face.
    // WARNING: This sets the rotation matrix registers, so it overrides SetRotMatrix
    MulMatrix0(&light_directions, &rotation_matrix, &light_directions_in_model_space);

    gte_SetRotMatrix( &model_to_view_matrix );
    gte_SetTransMatrix( &model_to_view_matrix );
    gte_SetLightMatrix( &light_directions_in_model_space );

    for( int part_index = 0; part_index < model.asset.num_parts; part_index++ ) {
        td::ModelPart* model_part = model.asset.model_parts[part_index];
        for( int i = 0; i < model_part->num_triangles; i++ ) {

            auto vec3_to_svector = [](const Vec3<td::int16> vec3) {
                return SVECTOR{vec3.x,vec3.y, vec3.z};
            };

            SVECTOR mv0 = vec3_to_svector(model.asset.vertices[model_part->vertex_indices[i].x - 1]);
            SVECTOR mv1 = vec3_to_svector(model.asset.vertices[model_part->vertex_indices[i].y - 1]);
            SVECTOR mv2 = vec3_to_svector(model.asset.vertices[model_part->vertex_indices[i].z - 1]);
        
            /* Load the first 3 vertices of a quad to the GTE */
            gte_ldv3(
                &mv0, &mv1, &mv2                 
                 
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

            td::uint32 ordering_table_index_32;
            gte_stopz(&ordering_table_index_32);
            ordering_table_index_32 >>= 12;

            // TODO: Set the layer factor

            td::OrderingTableLayer& layer = active_render_buffer->get_middle_layer();

            // In front, or on, near-plane
            if( ordering_table_index_32 == 0 ) continue;

            // Behind far-plane
            if( ordering_table_index_32 >= layer.get_resolution() ) continue;

            td::uint16 ordering_table_index = (td::uint16)(ordering_table_index_32);
        
            DVECTOR v0;
            DVECTOR v1;
            DVECTOR v2;

            /* Set the projected vertices to the primitive */
            gte_stsxy0( &v0 );
            gte_stsxy1( &v1 );
            gte_stsxy2( &v2 );
        
            if( !screen_triangle_is_in_screen(&v0, &v1, &v2)) {
                continue;
            }        

            // if( model.mesh.normalsType == TriangleMesh::NormalsType::FACE ) {
            //     POLY_F3* triangle_prim = active_render_buffer->create_and_add_prim<POLY_F3>(
            //         low_precision_average_z); 

            //     triangle_prim->r0 = model.color.r;
            //     triangle_prim->g0 = model.color.g;
            //     triangle_prim->b0 = model.color.b;

            //     *(DVECTOR*)(&triangle_prim->x0) = v0;
            //     *(DVECTOR*)(&triangle_prim->x1) = v1;
            //     *(DVECTOR*)(&triangle_prim->x2) = v2;

            //     gte_ldrgb( &triangle_prim->r0 );
                
            //     /* Load the face normal */
            //     gte_ldv0( &model.mesh.normals[i] );
                
            //     /* Normal Color Single */
            //     gte_nccs();

            //     gte_strgb( &triangle_prim->r0 );
            // }
            // else
            {
                POLY_G3* triangle_prim = active_render_buffer->create_and_add_prim<POLY_G3>(
                    ordering_table_index); 

                triangle_prim->r0 = model.color.r;
                triangle_prim->g0 = model.color.g;
                triangle_prim->b0 = model.color.b;

                triangle_prim->r1 = model.color.r;
                triangle_prim->g1 = model.color.g;
                triangle_prim->b1 = model.color.b;

                triangle_prim->r2 = model.color.r;
                triangle_prim->g2 = model.color.g;
                triangle_prim->b2 = model.color.b;

                *(DVECTOR*)(&triangle_prim->x0) = v0;
                *(DVECTOR*)(&triangle_prim->x1) = v1;
                *(DVECTOR*)(&triangle_prim->x2) = v2;

                auto compute_color = [&i, &model, &vec3_to_svector](uint8_t* color, const Vec3<td::int16>* normal) {
                    gte_ldrgb( color );

                    SVECTOR n = vec3_to_svector(*normal);

                    // Load vertex normal            
                    gte_ldv0( &n );
                    
                    /* Normal Color Single */
                    gte_nccs();

                    // Retrieve color
                    gte_strgb( color );    
                };

                compute_color(&triangle_prim->r0, &model.asset.normals[model_part->normal_indices[i].x - 1]);
                compute_color(&triangle_prim->r1, &model.asset.normals[model_part->normal_indices[i].y - 1]);
                compute_color(&triangle_prim->r2, &model.asset.normals[model_part->normal_indices[i].z - 1]);

                num_rendered_triangles++;
            }
        }
    }
}

void Renderer::flush() {

    DrawSync(0);
	VSync(0);

    // std::printf("Rendered triangles: %d\n", num_rendered_triangles);
    num_rendered_triangles = 0;

    RenderBuffer* inactive_render_buffer = active_render_buffer == render_buffer_1 ? render_buffer_2 : render_buffer_1;
    
    inactive_render_buffer->activate();
    inactive_render_buffer->clear();    
    active_render_buffer->draw();

    active_render_buffer = inactive_render_buffer;

    // The "active" render buffer is the one being displayed, and the one whose
    // ordering table and primitive buffer we are submitting to.
    // The inactive render buffer is drawing it's ordering-table to its framebuffer
}

void Renderer::set_light_direction(uint8_t light_index, SVECTOR normalized_direction) {
    TD_ASSERT(light_index >= 0, "light_index must not be negative (was %d)", light_index);
    TD_ASSERT(light_index <= 2, "light_index must not be larger (was %d)", light_index);

    // Each row is the color of a light
    light_directions.m[light_index][0] = normalized_direction.vx;
    light_directions.m[light_index][1] = normalized_direction.vy;
    light_directions.m[light_index][2] = normalized_direction.vz;
}

void Renderer::set_light_color(uint8_t light_index, VECTOR color) {
    TD_ASSERT(light_index >= 0, "light_index must not be negative (was %d)", light_index);
    TD_ASSERT(light_index <= 2, "light_index must not be larger (was %d)", light_index);

    // Each column is the color of a light
    light_colors.m[0][light_index] = color.vx;
    light_colors.m[1][light_index] = color.vy;
    light_colors.m[2][light_index] = color.vz;

    light_colors_changed = true;
}

bool Renderer::screen_triangle_is_in_screen(
    const DVECTOR* v0, const DVECTOR* v1, const DVECTOR* v2
) const {

    auto is_vertex_in_Screen = [this](const DVECTOR* v) -> bool {
        if( v->vx < 0 || v->vx > resolution_width ) return false;
        if( v->vy < 0 || v->vy > resolution_height ) return false;
        return true;
    };

    return is_vertex_in_Screen(v0) || is_vertex_in_Screen(v1) || is_vertex_in_Screen(v2);
}

bool Renderer::screen_quad_is_in_screen(
    const DVECTOR* v0, const DVECTOR* v1, const DVECTOR* v2, const DVECTOR* v3
) const {

    auto test_vertex = [this](const DVECTOR* v) -> bool {
        if( v->vx < 0 || v->vx > resolution_width ) return false;
        if( v->vy < 0 || v->vy > resolution_height ) return false;
        return true;
    };

    return test_vertex(v0) || test_vertex(v1) || test_vertex(v2) || test_vertex(v3);
}