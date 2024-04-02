#include <tundra/core/fixed.hpp>

#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>

#include <tundra/core/log.hpp>
#include <tundra/core/list.hpp>
#include <tundra/gte/initialize.hpp>
#include <tundra/assets/model/model-asset.hpp>
#include <tundra/assets/model/model-deserializer.hpp>

#include "model.hpp"
#include "renderer.hpp"
#include "tundra/core/fixed.hpp"

constexpr int RESOLUTION[] = { 320, 240 };

constexpr td::uint16 ORDERING_TABLE_SIZE = 2048; // entries

constexpr size_t PRIMIIVES_BUFFER_SIZE = 30000; // bytes

constexpr CVECTOR CLEAR_COLOR = { 200, 60, 30 };

namespace assets {
    extern "C" const uint8_t mdl_fish[];
}

#include <cstdio>

int main() {

    std::printf("hello\n");

    TD_DEBUG_LOG("Initializing %d", 1);
    ResetGraph(0);
    td::gte::initialize();

    // Set the origin of screen space
	gte_SetGeomOffset(RESOLUTION[0] / 2, RESOLUTION[1] / 2);
	
	// Set screen depth (basically FOV control, W/2 works best)
	gte_SetGeomScreen(RESOLUTION[0] / 2);

    TD_DEBUG_LOG("Loading Suzanne");
    td::ModelAsset* fish_model = td::ModelDeserializer().deserialize((td::byte*)assets::mdl_fish);
    TD_DEBUG_LOG("  Model triangles: %d", fish_model->get_total_num_triangles());

    // TD_DEBUG_LOG("Setting up renderer");
    TD_DEBUG_LOG("Initializing renderer and data");
    Renderer renderer;
    renderer.initialize(
        ORDERING_TABLE_SIZE,
        PRIMIIVES_BUFFER_SIZE,
        RESOLUTION[0],
        RESOLUTION[1],
        CLEAR_COLOR
    );

    renderer.set_light_direction( 0, {ONE >> 1, ONE >> 1, -ONE >> 1 });
	renderer.set_light_color(0, {ONE >> 1, ONE >> 1, ONE >> 1 });

    renderer.get_camera().set_position({0, ONE * 3, (int32_t)(-ONE * 5.25)});
    renderer.get_camera().set_target({0, 0, 0});

    constexpr td::uint16 MODEL_SIZE = (td::uint16)(ONE * 2);

    td::Fixed32<12> model_distance = 2;
    td::List<Model> models;
    td::Fixed16<12> model_rotation = 0;
    for( td::int32 model_x = -1; model_x < 2; model_x++ ) {
        for( td::int32 model_z = -1; model_z < 2; model_z++ ) {
            
            Model model { *fish_model };
            model.scale = { MODEL_SIZE, MODEL_SIZE, MODEL_SIZE };
            model.color = { 80, 255, 150, 0 };
            model.position = VECTOR{ (model_distance * model_x).get_raw_value(), 0, (model_distance * model_z).get_raw_value() };
            model.rotation = {0, model_rotation.get_raw_value(), 0};
            model_rotation += td::to_fixed(0.15);
            models.add(model);
        
        }   
    }
        
    td::Fixed16<12> camera_y_rotation = 0;

    td::Fixed32<12> camera_height = 2;
    td::Fixed32<12> camera_xz_distance = 5;

    TD_DEBUG_LOG("Running main loop");
    while(true) {

        camera_y_rotation -= td::to_fixed(0.005);
        if( camera_y_rotation < 0 ) {
            camera_y_rotation += 1;            
        }

        td::Fixed32<12> camera_x = td::Fixed32<12>::from_raw_fixed_value(isin(camera_y_rotation.get_raw_value())) * camera_xz_distance;
        td::Fixed32<12> camera_z = td::Fixed32<12>::from_raw_fixed_value(icos(camera_y_rotation.get_raw_value())) * camera_xz_distance;

        renderer.get_camera().set_position({
            camera_x.get_raw_value(),
            camera_height.get_raw_value(),
            camera_z.get_raw_value() 
        });

        for( td::uint32 i = 0; i < models.get_size(); i++ ) {
            renderer.draw_model(models[i]);
        }

        renderer.flush();
    }

    return 0; 
}