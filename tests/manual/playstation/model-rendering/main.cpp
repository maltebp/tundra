
#include <tundra/startup.hpp>

#include <tundra/core/fixed.hpp>
#include <tundra/core/log.hpp>
#include <tundra/core/list.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/math.hpp>

#include <tundra/assets/texture/texture-asset.hpp>
#include <tundra/assets/model/model-asset.hpp>

#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/engine/static-transform.hpp>

#include <tundra/rendering/camera.hpp>
#include <tundra/rendering/model.hpp>
#include <tundra/rendering/render-system.hpp>
#include <tundra/rendering/sprite.hpp>
#include <tundra/rendering/text.hpp>

#include <tundra/gte/compute-transform.hpp>


const td::EngineSettings ENGINE_SETTINGS {
    30000
};

constexpr td::uint16 ORDERING_TABLE_SIZE = 2048; // entries

constexpr td::Vec3<td::uint8> CLEAR_COLOR = { 255, 100, 50 };

constexpr td::Vec3<td::uint8> AMBIENT_COLOR =
    { 150, 100, 40 };

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
    extern "C" const uint8_t mdl_car[];
    extern "C" const uint8_t tex_mdl_car_1[];
    extern "C" const uint8_t tex_ball[];
    extern "C" const uint8_t tex_dog[];
    extern "C" const uint8_t tex_strawberries[];
}


td::Fixed32<12> camera_y_rotation = 1;
td::Camera* camera;

extern void initialize(td::EngineSystems& engine_systems) {
    TD_DEBUG_LOG("Initializing %d", 1);

    engine_systems.render.set_clear_color(CLEAR_COLOR);
    engine_systems.render.set_ambient_light(AMBIENT_COLOR);
    engine_systems.render.set_light_direction(0, DIRECTIONAL_LIGHT_DIRECTIONS[0]);
    engine_systems.render.set_light_direction(1, DIRECTIONAL_LIGHT_DIRECTIONS[1]);
    engine_systems.render.set_light_direction(2, DIRECTIONAL_LIGHT_DIRECTIONS[2]);
    engine_systems.render.set_light_color(0, DIRECTIONAL_LIGHT_COLORS[0]);
    engine_systems.render.set_light_color(1, DIRECTIONAL_LIGHT_COLORS[1]);
    engine_systems.render.set_light_color(2, DIRECTIONAL_LIGHT_COLORS[2]);

    
    td::Entity* camera_entity = td::Entity::create();
    td::DynamicTransform* camera_transform = camera_entity->add_component<td::DynamicTransform>();
    camera_transform->set_translation({0, 3, td::to_fixed(-5.25)});
    camera_transform->set_translation({0, 0, -1});

    td::List<td::CameraLayerSettings> layer_settings;
    layer_settings.add({LAYER_FOREGROUND, 1});
    layer_settings.add({LAYER_MIDDLE, ORDERING_TABLE_SIZE});
    layer_settings.add({LAYER_BACKGROUND, 1});

    camera = camera_entity->add_component<td::Camera>(camera_transform, layer_settings);

     TD_DEBUG_LOG("Loading models..");
    
    const td::ModelAsset* fish_model = engine_systems.asset_load.load_model((td::byte*)assets::mdl_fish);
    TD_DEBUG_LOG("  Fish triangles: %d", fish_model->get_total_num_triangles());

    const td::ModelAsset* sphere_model = engine_systems.asset_load.load_model((td::byte*)assets::mdl_sphere);
    TD_DEBUG_LOG("  Sphere triangles: %d", sphere_model->get_total_num_triangles());

    const td::ModelAsset* sphere_box_model = engine_systems.asset_load.load_model((td::byte*)assets::mdl_sphere_box);
    TD_DEBUG_LOG("  Sphere-Box triangles: %d", sphere_model->get_total_num_triangles());
    
    const td::TextureAsset* car_texture_1 = engine_systems.asset_load.load_texture((td::byte*)assets::tex_mdl_car_1);

    const td::ModelAsset* car_model = engine_systems.asset_load.load_model((td::byte*)assets::mdl_car, car_texture_1);
    TD_DEBUG_LOG("  Car triangles: %d", car_model->get_total_num_triangles());

    const td::TextureAsset* ball_texture = engine_systems.asset_load.load_texture((td::byte*)assets::tex_ball);
    const td::TextureAsset* image = engine_systems.asset_load.load_texture((td::byte*)assets::tex_strawberries);    

    TD_DEBUG_LOG("Initializing rendering data");

    // Create the models
    td::Fixed32<12> model_distance = td::to_fixed(0.1);
    td::Fixed16<12> model_rotation = 0;

    for( td::int32 model_x = -1; model_x < 2; model_x++ ) {
        for( td::int32 model_z = -1; model_z < 2; model_z++ ) {
            
            if( model_x == 0 && model_z == 0 ) {
                td::Entity* car = td::Entity::create();
                td::StaticTransform* transform = car->add_component<td::StaticTransform>(
                    td::gte::compute_world_matrix(
                        td::Vec3<td::Fixed32<12>>{ td::Fixed32<12>{td::to_fixed(0.2)} },
                        td::Vec3<td::Fixed16<12>>{ 0, td::to_fixed(0.125), 0},
                        td::Vec3<td::Fixed32<12>>{ model_distance * model_x, 0, model_distance * model_z }
                    )
                );

                car->add_component<td::Model>(*car_model, LAYER_MIDDLE, transform);
                model_rotation += td::to_fixed(0.15);
            }
            else if( model_x == 1 && model_z == 1 ) {
                td::Entity* sphere_box = td::Entity::create();
                td::StaticTransform* transform = sphere_box->add_component<td::StaticTransform>(
                    td::gte::compute_world_matrix(
                        td::Vec3<td::Fixed32<12>>{ td::Fixed32<12>{td::to_fixed(0.2)} },
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

    // Create sprites
    auto create_ball = [&](td::Vec2<td::Fixed32<12>> position, td::Vec2<td::Fixed32<12>> size, td::Vec3<td::uint8> color) {
        td::Entity* e = td::Entity::create();
        td::Sprite* sprite = e->add_component<td::Sprite>(LAYER_FOREGROUND);
        sprite->texture = ball_texture;
        sprite->color = color;
        sprite->position = position;
        sprite->size = size;
    };

    create_ball({24, 24}, {16, 16}, {255U, 255U, 255U});
    create_ball({320 - 26, 26}, {20, 20}, { 0, 255U, 255U});
    create_ball({320 - 32, 240 - 32}, {32, 32}, { 255U, 255U, 0});
    create_ball({36, 240 - 36}, {40, 40}, { 120U, 255U, 120U});

    auto create_dog = [&](td::Vec2<td::Fixed32<12>> position, td::Vec2<td::Fixed32<12>> size) {
        td::Entity* e = td::Entity::create();
        td::Sprite* sprite = e->add_component<td::Sprite>(LAYER_FOREGROUND);
        sprite->texture = image;
        sprite->position = position;
        sprite->size = size;
    };

    create_dog({ 160, 24 }, {32, 32});

    // Create text

    auto create_text = [](const td::String& text, td::Vec2<td::Fixed32<12>> position) {
        td::Text* text_component = td::Entity::create()->add_component<td::Text>(LAYER_FOREGROUND);
        text_component->position = position;
        text_component->text = text;
    };

    create_text("This is some text", { 90, 60 });
    create_text("This is some more text", { 70, 210});
    create_text("... and more text!", {100, 220});
}

extern void update(td::EngineSystems& engine_systems, const td::FrameTime&) {

    std::printf("draw: %s\n", td::to_string(engine_systems.render.get_last_draw_duration().to_milliseconds()).get_c_string());
    
    for( td::Sprite* sprite : td::Sprite::get_all() ) {
        sprite->rotation += td::to_fixed(0.005);
        if( sprite->rotation > 1 ) {
            sprite->rotation -= 1;
        }
    }

    camera_y_rotation -= td::to_fixed(0.005);
    if( camera_y_rotation < 0 ) {
        camera_y_rotation += 1;            
    }

    td::Fixed32<12> camera_x = td::cos(camera_y_rotation) * CAMERA_XZ_DISTANCE;
    td::Fixed32<12> camera_z = td::sin(camera_y_rotation) * CAMERA_XZ_DISTANCE;

    camera->transform->set_translation({camera_x, CAMERA_HEIGHT, camera_z});
    camera->look_at(CAMERA_TARGET);
    
}