#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>

#include <tundra/core/log.hpp>
#include <tundra/gte/initialize.hpp>
#include <tundra/assets/model/model-asset.hpp>
#include <tundra/assets/model/model-deserializer.hpp>

#include "model.hpp"
#include "renderer.hpp"

constexpr int RESOLUTION[] = { 320, 240 };

constexpr size_t ORDERING_TABLE_SIZE = 2048; // entries

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
    renderer.get_camera().set_target({0, (int32_t)(ONE * 2.5), 0});

    while(true) {
            
        constexpr td::uint16 size = (td::uint16)(ONE * 0.75);
        Model model { *suzanne_model };
        model.position = { 0, 0 + (int32_t)((size * 0.75)), 0 };
        model.scale = { size, size, size };
        model.color = { 80, 255, 150, 0 };
        model.rotation = { 0, -ONE >> 2, 0 };

        renderer.draw_model(model);

        renderer.flush();
    }

    return 0; 
}