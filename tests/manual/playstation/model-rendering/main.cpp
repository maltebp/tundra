#include "tundra/core/math.hpp"
#include "tundra/core/vector.hpp"

#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>

#include <tundra/core/fixed.hpp>
#include <tundra/core/log.hpp>
#include <tundra/core/list.hpp>
#include <tundra/core/fixed.hpp>

#include <tundra/assets/model/model-asset.hpp>
#include <tundra/assets/model/model-deserializer.hpp>

#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/engine/static-transform.hpp>

#include <tundra/rendering/camera.hpp>
#include <tundra/rendering/model.hpp>
#include <tundra/rendering/render-system.hpp>

#include <tundra/gte/initialize.hpp>
#include <tundra/gte/compute-transform.hpp>


constexpr td::uint16 ORDERING_TABLE_SIZE = 2048; // entries
constexpr td::uint32 PRIMIIVES_BUFFER_SIZE = 30000; // bytes

constexpr td::Vec3<td::uint8> CLEAR_COLOR = { 255, 100, 50 };

constexpr td::Vec3<td::uint8> AMBIENT_COLOR =
    { 150, 100, 40 };
    // { 200, 60, 30 };
    // { 200, 60, 30 };

constexpr td::Vec3<td::Fixed16<12>> DIRECTIONAL_LIGHT_DIRECTIONS[3] {
    td::Vec3<td::Fixed16<12>>{  td::to_fixed(-0.71), td::to_fixed(-0.71), 0 },
    td::Vec3<td::Fixed16<12>>{ 0 },
    td::Vec3<td::Fixed16<12>>{ 0 }
};

constexpr td::Vec3<td::uint8> DIRECTIONAL_LIGHT_COLORS[3] {
    td::Vec3<td::uint8>{  255, 225, 200 },
    td::Vec3<td::uint8>{ 0 },
    td::Vec3<td::uint8>{ 0 }
};


constexpr td::Fixed32<12> CAMERA_HEIGHT { td::to_fixed(0.1) };
constexpr td::Fixed32<12> CAMERA_XZ_DISTANCE {td::to_fixed(0.2)};
constexpr td::Vec3<td::Fixed32<12>> CAMERA_TARGET { 0 };

constexpr td::uint32 LAYER_FOREGROUND = 0;
constexpr td::uint32 LAYER_MIDDLE = 1;
constexpr td::uint32 LAYER_BACKGROUND = 2;

namespace assets {
    extern "C" const uint8_t mdl_fish[];
    extern "C" const uint8_t mdl_sphere[];
    extern "C" const uint8_t mdl_sphere_box[];
}

int main() {

    TD_DEBUG_LOG("Initializing %d", 1);
    ResetGraph(0);
    td::gte::initialize();

    // Set the origin of screen space
	gte_SetGeomOffset(320 / 2, 240/ 2);
	
	// Set screen depth (basically FOV control, W/2 works best)
	gte_SetGeomScreen(320 / 2);

    TD_DEBUG_LOG("Loading models..");
    
    td::ModelAsset* fish_model = td::ModelDeserializer().deserialize((td::byte*)assets::mdl_fish);
    TD_DEBUG_LOG("  Fish triangles: %d", fish_model->get_total_num_triangles());

    td::ModelAsset* sphere_model = td::ModelDeserializer().deserialize((td::byte*)assets::mdl_sphere);
    TD_DEBUG_LOG("  Sphere triangles: %d", sphere_model->get_total_num_triangles());

    td::ModelAsset* sphere_box_model = td::ModelDeserializer().deserialize((td::byte*)assets::mdl_sphere_box);
    TD_DEBUG_LOG("  Sphere-Box triangles: %d", sphere_model->get_total_num_triangles());

    TD_DEBUG_LOG("Initializing RenderSystem");
    td::RenderSystem render_system{PRIMIIVES_BUFFER_SIZE, CLEAR_COLOR};
    render_system.set_ambient_light(AMBIENT_COLOR);
    render_system.set_light_direction(0, DIRECTIONAL_LIGHT_DIRECTIONS[0]);
    render_system.set_light_direction(1, DIRECTIONAL_LIGHT_DIRECTIONS[1]);
    render_system.set_light_direction(2, DIRECTIONAL_LIGHT_DIRECTIONS[2]);
    render_system.set_light_color(0, DIRECTIONAL_LIGHT_COLORS[0]);
    render_system.set_light_color(1, DIRECTIONAL_LIGHT_COLORS[1]);
    render_system.set_light_color(2, DIRECTIONAL_LIGHT_COLORS[2]);

    td::Entity* camera_entity = td::Entity::create();
    td::DynamicTransform* camera_transform = camera_entity->add_component<td::DynamicTransform>();
    camera_transform->set_translation({0, 3, td::to_fixed(-5.25)});
    camera_transform->set_translation({0, 0, -1});

    td::List<td::CameraLayerSettings> layer_settings;
    layer_settings.add({LAYER_FOREGROUND, 1});
    layer_settings.add({LAYER_MIDDLE, ORDERING_TABLE_SIZE});
    layer_settings.add({LAYER_BACKGROUND, 1});

    td::Camera* camera = camera_entity->add_component<td::Camera>(camera_transform, layer_settings);

    TD_DEBUG_LOG("Initializing rendering data");

    // Create the models
    td::Fixed32<12> model_distance = td::to_fixed(0.1);
    td::Fixed16<12> model_rotation = 0;

    for( td::int32 model_x = -1; model_x < 2; model_x++ ) {
        for( td::int32 model_z = -1; model_z < 2; model_z++ ) {
            
            if( model_x == 0 && model_z == 0 ) {
                td::Entity* sphere_box = td::Entity::create();
                td::StaticTransform* transform = sphere_box->add_component<td::StaticTransform>(
                    td::gte::compute_world_matrix(
                        td::Vec3<td::Fixed32<12>>{ td::Fixed32<12>{td::to_fixed(0.5)} },
                        td::Vec3<td::Fixed16<12>>{ 0, 0,  0},
                        td::Vec3<td::Fixed32<12>>{ model_distance * model_x, 0, model_distance * model_z }
                    )
                );

                sphere_box->add_component<td::Model>(*sphere_box_model, LAYER_MIDDLE, transform);
                model_rotation += td::to_fixed(0.15);
            }
            else if( (model_x == -1 || model_x == 1) && (model_z == -1 || model_z == 1) ) {
                td::Entity* sphere = td::Entity::create();
                td::StaticTransform* transform = sphere->add_component<td::StaticTransform>(
                    td::gte::compute_world_matrix(
                        td::Vec3<td::Fixed32<12>>{ td::Fixed32<12>{td::to_fixed(0.5)} },
                        td::Vec3<td::Fixed16<12>>{ 0, model_rotation,  0},
                        td::Vec3<td::Fixed32<12>>{ model_distance * model_x, 0, model_distance * model_z }
                    )
                );

                sphere->add_component<td::Model>(*sphere_model, LAYER_MIDDLE, transform);
                model_rotation += td::to_fixed(0.15);
            }
            else {
                td::Entity* fish = td::Entity::create();
                td::StaticTransform* transform = fish->add_component<td::StaticTransform>(
                    td::gte::compute_world_matrix(
                        td::Vec3<td::Fixed32<12>>{ td::to_fixed(0.1) },
                        td::Vec3<td::Fixed16<12>>{ 0, model_rotation,  0},
                        td::Vec3<td::Fixed32<12>>{ model_distance * model_x, 0, model_distance * model_z }
                    )
                );

                td::Model* model = fish->add_component<td::Model>(*fish_model, LAYER_MIDDLE, transform);
                model->color = { 100, 100, 255 };
                model_rotation += td::to_fixed(0.15);
            }
        }   
    }
    
    td::Fixed32<12> camera_y_rotation = 1;
    
    SetDispMask(1);
    FntLoad(0, 256);

    TD_DEBUG_LOG("Running main loop");
    while(true) {

        camera_y_rotation -= td::to_fixed(0.005);
        if( camera_y_rotation < 0 ) {
            camera_y_rotation += 1;            
        }

        td::Fixed32<12> camera_x = td::cos(camera_y_rotation) * CAMERA_XZ_DISTANCE;
        td::Fixed32<12> camera_z = td::sin(camera_y_rotation) * CAMERA_XZ_DISTANCE;

        camera->transform->set_translation({camera_x, CAMERA_HEIGHT, camera_z});
        camera->look_at(CAMERA_TARGET);

        // TD_DEBUG_LOG("y_rot = %s, T = %s, R = %s", camera_y_rotation, camera->transform->get_translation(), camera->transform->get_rotation());

        render_system.render();
    }

    return 0; 
}