#include <tundra/core/fixed.hpp>

#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>

#include <tundra/core/log.hpp>
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
    extern "C" const uint8_t mdl_suzanne[];
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
    td::ModelAsset* suzanne_model = td::ModelDeserializer().deserialize((td::byte*)assets::mdl_suzanne);

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

    constexpr td::uint16 MODEL_SIZE = (td::uint16)(ONE * 0.75);
    
    Model model { *suzanne_model };
    model.scale = { MODEL_SIZE, MODEL_SIZE, MODEL_SIZE };
    model.color = { 80, 255, 150, 0 };

    td::Fixed16<12> model_y_rotation = 0;
    td::Fixed16<12> camera_y_rotation = 0;

    td::Fixed32<12> camera_height = 2;
    td::Fixed32<12> camera_xz_distance = 5;

    TD_DEBUG_LOG("Running main loop");
    while(true) {
        model_y_rotation += td::to_fixed(0.01);
        if( model_y_rotation > 1 ) {
            model_y_rotation -= 1;
        }

        model.rotation = { 0, model_y_rotation.get_raw_value(), 0 };

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

        renderer.draw_model(model);

        renderer.flush();
    }

    return 0; 
}