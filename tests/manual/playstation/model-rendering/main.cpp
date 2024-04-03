#include <tundra/core/fixed.hpp>

#include <cstdio>

#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>

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

constexpr td::Vec3<td::uint8> CLEAR_COLOR = { 200, 60, 30 };

constexpr td::uint32 LAYER_FOREGROUND = 0;
constexpr td::uint32 LAYER_MIDDLE = 1;
constexpr td::uint32 LAYER_BACKGROUND = 2;

namespace assets {
    extern "C" const uint8_t mdl_fish[];
}

int main() {

    std::printf("hello\n");

    TD_DEBUG_LOG("Initializing %d", 1);
    ResetGraph(0);
    td::gte::initialize();

    // Set the origin of screen space
	gte_SetGeomOffset(320 / 2, 240/ 2);
	
	// Set screen depth (basically FOV control, W/2 works best)
	gte_SetGeomScreen(320 / 2);

    TD_DEBUG_LOG("Loading Suzanne");
    td::ModelAsset* fish_model = td::ModelDeserializer().deserialize((td::byte*)assets::mdl_fish);
    TD_DEBUG_LOG("  Model triangles: %d", fish_model->get_total_num_triangles());

    // TD_DEBUG_LOG("Setting up renderer");
    TD_DEBUG_LOG("Initializing RenderSystem");
    td::RenderSystem render_system{PRIMIIVES_BUFFER_SIZE, CLEAR_COLOR};

    td::Entity* camera_entity = td::Entity::create();
    td::DynamicTransform* camera_transform = camera_entity->add_component<td::DynamicTransform>();
    // camera_transform->set_translation({0, 3, td::to_fixed(-5.25)});
    camera_transform->set_translation({0, 0, -1});

    td::List<td::CameraLayerSettings> layer_settings;
    layer_settings.add({LAYER_FOREGROUND, 1});
    layer_settings.add({LAYER_MIDDLE, ORDERING_TABLE_SIZE});
    layer_settings.add({LAYER_BACKGROUND, 1});

    td::Camera* camera = camera_entity->add_component<td::Camera>(camera_transform, layer_settings);

    TD_DEBUG_LOG("Initializing rendering data");

    SetDispMask(1);
    FntLoad(0, 256);


    // Create the fish
    constexpr td::Fixed16<12> MODEL_SIZE = 2;
    td::Fixed32<12> model_distance = 2;
    td::Fixed16<12> model_rotation = 0;

    for( td::int32 model_x = -1; model_x < 2; model_x++ ) {
        for( td::int32 model_z = -1; model_z < 2; model_z++ ) {

            td::Entity* fish = td::Entity::create();
            td::StaticTransform* transform = fish->add_component<td::StaticTransform>(
                td::gte::compute_world_matrix(
                    td::Vec3<td::Fixed32<12>>{ MODEL_SIZE },
                    td::Vec3<td::Fixed16<12>>{ 0, model_rotation,  0},
                    td::Vec3<td::Fixed32<12>>{ model_distance * model_x, 0, model_distance * model_z }
                )
            );
            
            model_rotation += td::to_fixed(0.15);

            td::Model* model = fish->add_component<td::Model>(*fish_model, LAYER_MIDDLE, transform);
            model->color = {80, 255, 150};
        }   
    }
        
    const td::Fixed32<12> CAMERA_HEIGHT = 2;
    const td::Fixed32<12> CAMERA_XZ_DISTANCE = 5;
    const td::Vec3<td::Fixed32<12>> CAMERA_TARGET { 0 };
    td::Fixed16<12> camera_y_rotation = 0;

    TD_DEBUG_LOG("Running main loop");
    while(true) {

        // camera_y_rotation -= td::to_fixed(0.005);
        // if( camera_y_rotation < 0 ) {
        //     camera_y_rotation += 1;            
        // }

        // td::Fixed32<12> camera_x = td::Fixed32<12>::from_raw_fixed_value(isin(camera_y_rotation.get_raw_value())) * CAMERA_XZ_DISTANCE;
        // td::Fixed32<12> camera_z = td::Fixed32<12>::from_raw_fixed_value(icos(camera_y_rotation.get_raw_value())) * CAMERA_XZ_DISTANCE;

        // camera->transform->set_translation({
        //     camera_x.get_raw_value(),
        //     CAMERA_HEIGHT.get_raw_value(),
        //     camera_z.get_raw_value() 
        // });

        camera->look_at({0, 0, 0});
        
        render_system.render();
    }

    return 0; 
}