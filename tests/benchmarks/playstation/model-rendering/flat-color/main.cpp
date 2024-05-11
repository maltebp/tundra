#include <tundra/startup.hpp>

#include <cstdlib>

#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/core/duration.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/list.dec.hpp>
#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/rendering/render-system.hpp>
#include <tundra/rendering/camera.hpp>

#include "base.hpp"

#include "model-rendering/base.hpp"
#include "tundra/assets/model/model-asset.hpp"
#include "tundra/rendering/model.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

namespace assets {
    extern "C" const uint8_t mdl_plane_1x1[];
    extern "C" const uint8_t mdl_plane_2x2[];
    extern "C" const uint8_t mdl_plane_16x16[];
}

struct Test {
    td::String name = "NO TEST";
    const td::ModelAsset* model = nullptr;
    td::uint32 count = 0;
};

struct TestResult {
    Test test;
    td::Fixed32<12> submission_time;
    td::Fixed32<12> draw_time;
    td::Fixed32<12> num_triangles_rendered;
};

td::List<Test> tests;
Test current_test;
td::List<TestResult> results;
td::List<td::Entity*> models;

td::List<td::Fixed32<12>> submission_time;
td::List<td::Fixed32<12>> draw_time;
td::List<td::Fixed32<12>> num_triangles_rendered;

td::uint32 frame_count = 0;
bool is_first_frame_in_test = false;

void start_new_test(const Test& test) {
    std::printf("Running '%s'\n", test.name.get_c_string());

    submission_time.clear();
    draw_time.clear();
    num_triangles_rendered.clear();

    current_test = test;

    for( td::uint32 i = 0; i < models.get_size(); i++ ) {
        models[i]->destroy();
    }    
    models.clear();

    td::Fixed32<12> step_size = td::Fixed32<12>{1} / (int)test.count;
    for( td::uint32 i = 0; i < test.count; i++ ) {
        for( td::uint32 j = 0; j < test.count; j++ ) {
            td::Entity* entity = td::Entity::create();
            td::DynamicTransform* transform = entity->add_component<td::DynamicTransform>();
            transform->set_translation({step_size * (int)i, step_size * (int)j, 0});
            transform->set_rotation({td::to_fixed(-0.25), 0, 0});
            entity->add_component<td::Model>(*test.model, 0U, transform);
            models.add(entity);
        }        
    }

    frame_count = 0;
    is_first_frame_in_test = true;
        
}

extern void initialize(td::EngineSystems& engine_systems) { 
    engine_systems.render.set_clear_color({255U,255U,255U});
    td::Entity* camera_entity = td::Entity::create();
    td::DynamicTransform* camera_transform = camera_entity->add_component<td::DynamicTransform>();
    camera_transform->set_translation({td::to_fixed(0.5), td::to_fixed(0.5), td::to_fixed(-0.8)});

    td::List<td::CameraLayerSettings> layer_settings;
    layer_settings.add({0, 2048});

    camera_entity->add_component<td::Camera>(camera_transform, layer_settings);

    submission_time.reserve(NUM_FRAMES * 2);
    draw_time.reserve(NUM_FRAMES * 2);
    num_triangles_rendered.reserve(NUM_FRAMES * 2);

    // TODO: Add models
    tests.add({ "Small", engine_systems.asset_load.load_model(assets::mdl_plane_1x1), 16 });
    tests.add({ "Medium", engine_systems.asset_load.load_model(assets::mdl_plane_2x2), 8 });
    tests.add({ "Large", engine_systems.asset_load.load_model(assets::mdl_plane_16x16), 1 });

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
        num_triangles_rendered.add(td::Fixed32<12>((td::int32)engine_systems.render.get_num_triangles_rendered()));
    }
    if( frame_count > 0 && frame_count <= NUM_FRAMES ) {
        draw_time.add(engine_systems.render.get_last_draw_duration().to_milliseconds());
    }
    
    if( frame_count == NUM_FRAMES + 1 ) {
        TestResult test_result {
            current_test,
            compute_list_average(submission_time),
            compute_list_average(draw_time),
            compute_list_average(num_triangles_rendered)
        };

        results.add(test_result);

        if( tests.get_size() == 0 ) {        

            std::printf("Model, Submission Time, Draw Time, Num triangles\n");
            for( td::uint32 i = 0; i < results.get_size(); i++ ) {
                std::printf("%s, %s, %s, %s\n", 
                    results[i].test.name.get_c_string(),
                    td::to_string(results[i].submission_time, 4).get_c_string(),
                    td::to_string(results[i].draw_time, 4).get_c_string(),
                    td::to_string(results[i].num_triangles_rendered, 4).get_c_string()
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