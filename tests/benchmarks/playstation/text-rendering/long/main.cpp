#include <tundra/startup.hpp>

#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/core/duration.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/list.dec.hpp>
#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/rendering/render-system.hpp>
#include <tundra/rendering/camera.hpp>
#include <tundra/assets/model/model-asset.hpp>
#include <tundra/assets/texture/texture-asset.hpp>
#include <tundra/rendering/model.hpp>

#include "base.hpp"

#include "model-rendering/base.hpp"
#include "tundra/rendering/sprite.hpp"
#include "tundra/rendering/text.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

struct Test {
    td::String name = "NO TEST";
};

struct TestResult {
    Test test;
    td::Fixed32<12> submission_time;
    td::Fixed32<12> draw_time;
    td::Fixed32<12> num_triangles_rendered;
    td::Fixed32<12> allocated_bytes;
};

td::List<Test> tests;
Test current_test;
td::List<TestResult> results;
td::List<td::Entity*> test_entities;

td::List<td::Fixed32<12>> submission_time;
td::List<td::Fixed32<12>> draw_time;
td::List<td::uint32> num_triangles_rendered;
td::List<td::uint32> allocated_bytes;

td::uint32 frame_count = 0;
bool is_first_frame_in_test = false;

void start_new_test(const Test& test) {

    submission_time.clear();
    draw_time.clear();
    num_triangles_rendered.clear();

    current_test = test;

    for( td::uint32 i = 0; i < test_entities.get_size(); i++ ) {
        test_entities[i]->destroy();
    }    
    test_entities.clear();

    td::Entity* entity = td::Entity::create();
    td::Text* text = entity->add_component<td::Text>(0U);
    text->text = "";
    for( td::uint32 i = 0; i < 30; i++ ) {
        text->text += "abcdefghijklmopqrstuvwxyz0123456789+-/*.\n";
    }

    test_entities.add(entity);

    frame_count = 0;
    is_first_frame_in_test = true;

    std::printf("Running '%s'\n", test.name.get_c_string());
}

extern void initialize(td::EngineSystems& engine_systems) { 
    engine_systems.render.set_clear_color({128U,128U,128U});

    td::Entity* camera_entity = td::Entity::create();
    td::DynamicTransform* camera_transform = camera_entity->add_component<td::DynamicTransform>();
    camera_transform->set_translation({td::to_fixed(0.5), td::to_fixed(0.5), td::to_fixed(-0.8)});

    td::List<td::CameraLayerSettings> layer_settings;
    layer_settings.add({0, 2048});

    camera_entity->add_component<td::Camera>(camera_transform, layer_settings);

    submission_time.reserve(NUM_FRAMES * 2);
    draw_time.reserve(NUM_FRAMES * 2);
    num_triangles_rendered.reserve(NUM_FRAMES * 2);

    tests.add({ "Long" });

    Test next_test = tests[0];
    tests.remove_at(0);
    start_new_test(next_test);
}
 
extern void update(td::EngineSystems& engine_systems, const td::FrameTime&) {

    if( is_first_frame_in_test ) {
        is_first_frame_in_test = false;
        return;
    }

    if( frame_count < NUM_FRAMES) {
        submission_time.add(engine_systems.render.get_submit_duration().to_milliseconds());
        num_triangles_rendered.add(engine_systems.render.get_num_triangles_rendered());
        allocated_bytes.add(engine_systems.render.get_num_allocated_bytes_in_buffer());
    }
    if( frame_count > 0 && frame_count <= NUM_FRAMES ) {
        draw_time.add(engine_systems.render.get_last_draw_duration().to_milliseconds());
    }
    
    if( frame_count == NUM_FRAMES + 1 ) {
        TestResult test_result {
            current_test,
            compute_list_average(submission_time),
            compute_list_average(draw_time),
            compute_list_average(num_triangles_rendered),
            compute_list_average(allocated_bytes)
        };

        results.add(test_result);

        if( tests.get_size() == 0 ) {        

            std::printf("Model, Submission Time, Draw Time, Num triangles, Allocated bytes\n");
            for( td::uint32 i = 0; i < results.get_size(); i++ ) {
                std::printf("%s, %s, %s, %s, %s\n", 
                    results[i].test.name.get_c_string(),
                    td::to_string(results[i].submission_time, 4).get_c_string(),
                    td::to_string(results[i].draw_time, 4).get_c_string(),
                    td::to_string(results[i].num_triangles_rendered, 4).get_c_string(),
                    td::to_string(results[i].allocated_bytes, 1).get_c_string()
                );
            }; 
            
            engine_systems.exit_requested = true;
            pcsx_exit(0);
        }
        else {
            Test next_test = tests[0];
            tests.remove_at(0);
            start_new_test(next_test);
        }
        
        return;
    }
    
    frame_count++;
}