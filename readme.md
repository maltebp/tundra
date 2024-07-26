# Tundra
Tundra is the **runtime** part of a general-purpose 3D engine for the **original PlayStation** console (PlayStation 1 / PSX), along with a few CLI tools to ease development and use of the engine. It uses C++20 and [psn00bsdk](https://github.com/Lameguy64/PSn00bSDK) as its underlying SDK, but currently it only supports Windows.

The engine was created as part of my Master's thesis, and has been used to create the game [Color Cannon Coop ](https://jesperpapiorgmailcom.itch.io/colorcannoncoop)at Nordic Game Jam 2024. As such, the engine is limited in features, has many things I would do differently and contains some hacky solutions.

This repository is not under further development, as I'm planning on rebooting the project under a new one. This will be linked to here, once that is started.



### Content

- [Thesis](#Thesis)
- [Thesis](#Thesis)
- [Thesis](#Thesis)
- [How to use](#How-to-use)
  - ...
- [Setup and build](#Setup-and-build)
- [Benchmarks](#Benchmarks)





## **Thesis**

This engine was created as the core part of my Master's thesis in MSc in Games (technology track) at IT University of Copenhagen in 2024. 

In practice, I primarily desired to spend my time on developing an engine. Formally, however, the motivation was that there seem to be no general-purpose game engines for the original PlayStation out there (to my knowledge), which seem to be because of the technical limitations of the console. But I'm not entirely convinced that is reason enough for one to not to exist, and so I formally sought to answer the following hypothesis:

> What are the technical capabilities of a general-purpose 3D game-engine for the original PlaySta-
> tion?

The final report can be found in the root of the repository ([](thesis.pdf).



## Features

- Game object system using *entities* and *components* 

- Renderer supporting textured/untextured and smooth/flat shaded 3D models, 2D sprites, 2D debug text (psn00bsdk's debug text), cameras, layers and directional lighting.

- 3D transform system accelerated by GTE

- Asset compiler from raw assets (.obj, .png, .wav etc.) to native engine formats, including simple loading of those assets in in engine

- Input system for original controller (not analog)

- Sound system to play 23 looping and one-off sounds simultaneously plus 1 music trac

- Hardware accelerated matrices and vector classes

- Fixed-point type including fixed point math

- Time/clock system

- Various other utility (strings, list, debug asserts, to-string functionality)

- Automated test system (mostly for engine development)

  

### Missing features and limitations

- The entire game is packed into a single binary that is fully loaded at startup, which means
  - The entire game (including assets) is limited to 2 megabytes
  - All assets have to be loaded into RAM, even those that are just loaded into video and sound RAM.
  - There is no audio streaming, significantly limiting the potential of music

- No advanced audio support (no stereo, no spatial sounds, no procedural sound effects)
- No concept of a "scene system"
- The renderer is not particularly optimized, and has only trivial culling mechanisms (e.g. big levels with a lot of geometry out of view still incur a big performance cost).
- No support for other controllers than original
- No C++ standard template library (STL)



## How to use

### Setup game

To use the engine for a game, setup the game with the [Tundra game template](https://github.com/maltebp/tundra-template). See the repository, for how to use the template.



### Setup and game loop

To use the engine, you must not provide your own `main` function, but instead it is started by including the `<tundra/startup.hpp>` in some translation unit, and defining the following two functions and global variable:

```c++
// Read by engine before init
const td::EngineSettings ENGINE_SETTINGS { };

// Run at start-up
void initialize(td::EngineSystems& engine_systems) { }

// Run every frame (game loop)
void update(td::EngineSystems& engine_systems, const td::FrameTime& frame_time) { }
```

The `engine_systems` arguments holds references to all the engine systems (asset loader, time, input, sound player).

### Assets

The engine supports automatic compilation and inclusion of raw assets (`.png`, `.obj`, `.wav`) to formats that the engine can load and use. 

#### Include asset

Assets are added in CMake using the `td_target_assets` like so:

```cmake
td_target_assets(<target> <path to asset directory> 
	<asset>
	...
	<asset>
)
```

**Note:** in the game template, this is done slightly different.

Each `<asset>` is of the format:

```cmake
<type> <code name> <path> <param> ... <param>
```

where

- `type`: type of the asset, is either `MODEL`, `TEXTURE` or `SOUND`
- `code`: the name of the global variable in code pointing to the asset data
- `path`: path to the asset, relative to the asset directory
- `param`: optional parameters on the asset, depending on the asset type (see assets types below)



#### Using asset

To use the asset in code, you must first create an external global variable using the *code name* you gave the asset:

```c++
extern "C" const td::uint8 <code name>[];
```

The asset must then be loaded using the `AssetLoader`:

```c++
extern void initialize(td::EngineSystems& engine_systems) {
    td::AssetLoader& asset_loader = engine_systems.asset_loader;
    
    td::ModelAsset* model = asset_loader.load_model(<model asset>);
    td::TextureAsset* texture = asset_loader.load_texture(<texture asset>);
    td::SoundAsset* sound = asset_loader.load_sound(<model asset>);
}
```

#### Textures

Textures are compiled using [img2tim](https://github.com/Lameguy64/img2tim) and thus supports any file type that it supports (tested with `.png` and `.jpg`), but they are limited to 256x256 pixels.

By default they are compiled as *full-color* textures (every pixel has a distinct color), but you can compile it using a palette instead (CLUT) to reduce the size by specifying either of the following parameters:

- `PALETTE4`: image has at most 16 colors
- `PALETTE8`: image has at most 256 colors

#### Models

Models must be in OBJ format and are compiled using a custom compiler.

It does not support all features of OBJ, but it supports the following:

- Normals
- Indexed mode for vertices, UVS and normals
- Textures (see below)
- Diffuse material color
- Smooth and flat shading

**With texture**

If a model has a texture, UVs will be automatically compiled, but the texture must be included separately. In code, when loading the model, the loaded texture must be passed as a parameter to `load_model`.

#### Sound

Sounds must be in 16-bit PCM `.wav` format with a single channel (mono), and are compiled using [es-ps2-vag-tool](https://github.com/eurotools/es-ps2-vag-tool).

A sound can be marked as looping by specifying the `LOOP` parameter. There is no distinction between music and sound effects, but sounds used for music must be looping.



### Entity system

The game object system consists of two concepts: *components* and *entities*.

- *Component*: customizable data type (through inheritance) to which arbitrary systems can apply behavior through iteration.
- *Entity*: a group of component.

The relevant headers are:

- `<tundra/engine/entity-system/component.hpp>`
- `<tundra/engine/entity-system/entity.hpp>`
- `<tundra/engine/entity-system/component-ref.hpp>`

#### Defining custom component

```c++
class MyComponent : public td::Component<MyComponent> {
public 
	int value;  
};
```

#### Creating component/entity

```c++
td::Entity* entity = td::Entity::create();
MyComponent* my_component = entity->add_component<MyComponent>();
my_component->value = 42;
```

#### Deleting component/entity

```c++
my_component->destroy(); // Only destroys this component
entity->destroy(); // Also destroys all components on entity
```

#### **Applying behavior**

Components have no "update" function (like you see in Unity). Instead, behavior is applied to components by iterating all components of a given type:

```c++
void bar() {
	for( MyComponent* my_component : MyComponent::get_all() ) {
        my_component->value++;
    }
}
```

This provides flexibility, as it allows any system to apply any logic to any number of components any number of times.

#### Component references

Entity and component memory addresses are stable, so you can safely store their pointers. But the system also has a *reference count* mechanism, that allows you to avoid dangling pointers by using the `ComponentRef` class. The `ComponentRef` will automatically be `nullptr` if the component that it references is destroyed.

```c++
#include <tundra/engine/entity-system/component-ref.hpp>

td::Entity* entity = td::Entity::create();
MyComponent* my_component = entity->add_component<MyComponent>();
td::ComponentRef<MyComponent> ref = my_component;

if( ref != nullptr ) {
    TD_DEBUG_LOG("Ref is valid");
}

my_component->destroy();

if( ref == nullptr ) {
    TD_DEBUG_LOG("Ref is invalid");
}
```

*Things to be aware of*

- There can be at most 255 references to the same component
- A `ComponentRef` will clear its reference count to a component when the reference is accessed and the component is destroyed, and while any `ComponentRef` has a reference to a component, its memory will not be freed (it will be destroyed though).
- Because entities are technically also components, you can also have a `td::ComponentRef<td::Entity>`



### Transforms

In Tundra, the entity system has no inherent knowledge of the concept of transforms. Instead, transforms are just regular components. Meaning that an entity can have an arbitrary number of transforms.

#### Transform types

There are 2 types of transforms:

- **`DynamicTransform`**: Can have a parent and children, and can be changed after constructing (92 bytes). Useful for moving objects (e.g. player)
- **`StaticTransform`**: Cannot have a parent nor children, and it must given its matrix at construction (44 bytes). Useful for environment (e.g. a tree or rock).

Both transforms inherit the abstract `Transform` class. Other components that needs a transform (such as a `Model`) require an `Transform` in its constructor.

*Example:*

```c++
td::Entity* entity = td::Entity::create();
td::DynamicTransform* transform_1 = entity->add_component<td::DynamicTransform>();
td::DynamicTransform* transform_2 = entity->add_component<td::DynamicTransform>();
td::StaticTransform* transform_3 = entity->add_component<td::StaticTransform>(..);

transform_1->add_child(transform_2);
```



#### Transform matrices

To retrieve the *local-to-world* matrix, or the `TransformMatrix`, of a transform and use it to transform a vector you may use the hardware accelerated computation functions:

```c++
#include <tundra/gte/compute-transform.hpp>

td::Transform* transform = /* ... */;

td::TransformMatrix world_matrix = td::gte::compute_world_matrix(transform);

td::Vec3<td::Fixed16<12>> v = /* ... */;
Vec3<Fixed32<12>> v_world = td::gte::apply_transform_matrix(world_matrix, v);
```

`DynamicTransform` caches its transform matrix, meaning it will only be calculated when needed and only recomputed if it has changed (various optimizations are done here to limit what is recomputed).

The `StaticTransform` stores its matrix directly, and thus involves no computation. This can also be fetched directly.

The header also contains other transform matrix functions, including multiplication of matrices, extraction of rotation matrices and computation of the inverse transform matrix.

### Rendering

The engine automatically runs the rendering logic after the `update(..)` function has been called. You utilize the rendering features by

- Adjusting lighting settings in the `RenderSystem` accessed via the `EngineSystems` (ambient lighting and 3 directional lights are available)
- Setup a `Camera` component along with *layer settings*
- Create *renderable components* that are either  `Model`, `Sprite` and `Text`

#### Camera

To render anything you must create at least one `Camera` component, which is assigned a `Transform` at construction. For every camera, every renderable component is rendered in relation to the camera's transform position and view direction. The camera's view direction is the camera's local negative z-axis (engine uses *right-handed* coordinate system).

```c++
auto layer_settings = // ... See below
    
td::Entity* camera_entity = td::Entity::create();

td::DynamicTransform* camera_transform = 
    camera_entity->add_component<td::DynamicTransform>();

td::Camera* camera = 
    camera_entity->add_component<td::Camera>(camera_transform, layer_settings);
```

The renderer automatically picks up every camera, so there is no need to register it with the renderer.

You can manually manipulate the camera's transform to change its view direction, but you can also use the `look_at` function:

```c++
camera->transform->set_translation({1, 0, 1})
camera->look_at({0, 0, 0 });
```

#### Layers

All renderable components are assigned to a *layer*, which is just an integer ID, and a camera has a list of layers that it renders.

**Note:** Because you currently cannot set which part of the framebuffer that a camera renders to, the multiple camera functionality is not all that useful (e.g. you cannot do split screen or a rearview camera).

Every camera can setup its own depth settings for every: how far it renders and the resolution of the depth (number of distinct depth values in the depth range). Increasing the resolution, increases the memory overhead of the camera (2 bytes per value). 

*Example:*

```c++
const td::uint32 LAYER_UI = 0;
const td::uint32 LAYER_WORLD = 1;
const td::uint32 LAYER_GROUND = 2;

td::List<td::CameraLayerSettings> layer_settings;
layer_settings.add({LAYER_UI, 1});
layer_settings.add({LAYER_WORLD, 2048});
layer_settings.add({LAYER_GROUND, 128});

camera_entity->add_component<td::Camera>(
    camera_transform, 
    layer_settings
);
```

#### Models

Models are instances of 3D `ModelAsset`s, including various instance settings such as a transform and tint.

```c++
td::Entity* entity = td::Entity::create();

td::Transform* transform = /* .. */;
td::uint32* layer = /* .. */;
td::ModelAsset* model_asset = /* .. */;

td::Model* model = entity->add_component<td::Model>(
    model_asset,
    layer, 
    transform
);
```

#### Sprites

Sprites are instances `TextureAsset`'s' in screen space including various instance settings such as position, rotation, size and tint.

```c++
td::uint32* layer = /* .. */;
td::TextureAsset* texture = /* .. */;

td::Entity* e = td::Entity::create();
td::Sprite* sprite = e->add_component<td::Sprite>(layer);
sprite->texture = texture;
sprite->size = { 32, 32 };
sprite->position = { 64, 64 };
```

Screen space is 320x240 with (0, 0) being top left and (320,240) being bottom right.

**Note:** I never got around to implement 2D transforms for Tundra.

#### Text

Tundra does not have proper UI text support, but it exposes *psn00bsdk*'s  *debug font* API via the component system.

```c++
td::uint32* layer = /* .. */;

td::Entity* entity = td::Entity::create();
td::Text* text_component = entity->add_component<td::Text>(layer);
text_component->text = "Hello word!";
text_component->position = { 64, 64};
```

The text is limited in that you cannot change the text's font, size, color or rotation.



### Sound

To play sound you must use the `SoundPlayer` provided with the `EngineSystems`, and you can play either music or sound effects.

Example:

```c++
td::SoundAsset* my_sound_effect;
td::SoundAsset* my_music;

void update(td::EngineSystems& engine_systems, const td::FrameTime& frame_time) { 
	
    td::uint32 play_id = engine_systems.sound_player.play_sound(my_sound_effect);
    engine_systems.sound_player.stop_sound(play_id);
    
    engine_systems.play_music(my_music);
    engine_systems.stop_music();
}
```

- Whether the sound is looping is given by the sound asset (see [Assets](#Assets))

### Input

Input is handled by polling the state of either of the two controllers (`IController`) in the `Input` class provided by the `EngineSystems`.

Example:

```c++
void update(td::EngineSystems& engine_systems, const td::FrameTime& frame_time) { 
	
    IController& controller_1 = engine_systems.input.controller_1;
    IController& controller_2 = engine_systems.input.controller_2;
    
    if( controller_1.is_pressed(td::Button::R1) ) {
        if( controller_1.is_pressed_this_frame(td::Button::Circle) ) {
            // Circle is pressed this frame (and not last frame) while R1 is 			 	// being pressed
        }
    }
	
    if( controller_2.is_active() ) {
        if( controller_2.is_pressed(td::Button::Cross) ) {
            // Cross is pressed on controller 2
        }
    }
}
```



### Utility

The engine contains various utility, some of which covers the lack of an STL implementation. All of these utility classes exists in `<tundra/core/...>` headers



#### Fixed-point class

Header: `<tundra/core/fixed.hpp>`

The PlayStation does not have hardware support for floating point numbers and instead uses [fixed point numbers](https://en.wikipedia.org/wiki/Fixed-point_arithmetic). Tundra has a some types two template types to represent such numbers: a 16-bit type `Fixed16<TNumFractionBits>` and a 32-bit type `Fixed32<TNumFractionBits>`.

The most common value for `TNumFractionBits` is 12, as that is what is used by the PlayStation hardware.

They support most of the operators you will need such as addition, subtraction, multiplication and casts in a type-safe manner.

##### Constructing

Integers (both constant expressions and non-constant expressions) can implicitly be cast to a fixed-pointer number:

```
int i = /* value from somewhere */;
td::Fixed16<12> f = 15;
td::Fixed32<12> f = i;
```

This cast incurs a single bit-shift.

You can construct a fixed-point number from a `double` value that is a constant expression, such as a `double` literal using the `td::to_fixed`:

```c++
td::Fixed32<12> f = td::to_fixed(12.5);
```

This is a `consteval` API that ensures the constant expression double is converted to a fixed-point type at compile time.

##### Math

The `<tundra/core/math.hpp>` includes various math functions that operates on the fixed-point types, including `sin`, `cos`, `abs` and so on. These implementations are from [fpm](https://github.com/MikeLankamp/fpm/blob/master/include/fpm/math.hpp) repository.

#### List

Header: `<tundra/core/list.hpp>`

Just a simple *dynamic array* implementation with geometric growth. This is the only container provided by engine.

```c++
td::List<int> l;
l.reserve(16);
l.add(10);
bool was_removed = l.remove(12);
int index = l.index_of(10);
bool contains = l.contains(42);
l.clear();
```

#### String

*Header: `<tundra/core/string.hpp>`*

A dynamic character array implementation very similar to `std::string`.

``` c++
td::String s = "Hello, ";
td::String s2 = "World!";
td::String s3 = s1 + S2;
```

##### From format

Using the header `<tundra/core/string-util.hpp>`, you can create a `td::String` from a string format (like `printf`):

```c++
td::String s = td::string_util::create_from_format("(%d, %d)", 10, 15);
```

##### To string

You can convert various primitives and types to a `td::String` using the specializations and overloads `td::to_string<T>`:

```c++
td::String s1 = td::to_string(10);

td::Fixed16<12> f = td::to_fixed(4.5);
td::String s2 = td::to_string(f);
```

You can implement a specialization for this function for you own types as well:

```c++
struct MyStruct {
	int i, j;
};

td::String td::to_string(const MyStruct& my_struct) {
	return td::string_util::create_from_format("(%d, %d)", i, j);
}
```

##### Debug log

To utilize `String` for printing, you can use the `TD_DEBUG_LOG` macro from `<tundra/core/log.hpp>` header. This uses `printf`-like formatting, but it can also print `Strings` and types that has a `td::to_string` implementation using the  `%s` format:

```c++
TD_DEBUG_LOG("%d", 42);

td::String s = "Hello world";
TD_DEBUGLOG("%s", s);

td::Fixed32<12> f = /* .. */;
TD_DEBUG_LOG("%s", f);
```

Debug logs are automatically compiled out when doing a *Release* build.

#### Debug asserts

*Header: `<tundra/core/assert.hpp>`*

Asserts whether a given expression is true, and if not, it will print the given message and halt the program:

```c++
int i = / * ... */;
int j = / * ... */;
TD_ASSERT(i == j, "%d does not equal %d", i, j);
```

Unfortunately, I did not get around to support the same functionality as with `TD_DEBUG_LOG`, so you must manually convert non-primitive objects and `td::String` to a C-strings:

```c++
td::String s = "Hello world";
TD_ASSERT(false, "%s", s.get_c_string());

td::Fixed32<12> f = /* .. */;
TD_ASSERT(false, "%s", td::to_string(f).get_c_string());
```

#### Matrices and vectors

*Headers: `<tundra/core/vec/vec3.hpp>`, `<tundra/core/vec/vec2.hpp>`, `<tundra/core/mat/mat3x3.hpp>`*

The engine contains some very simple template types for 2D and 3D vectors and 3x3 matrices that supports some simple addition, subtraction and comparison operations.

```c++
td::Vec3<int>`v1 { 1, 2, 3 };
td::Vec3<int>`v2 { 4 };
td::Vec3<int> v3 = v1 + v2;

td::Vec2<td::Fixed32<12>> v4;

td::Mat3x3<td::Fixed16<12>> m {
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
};
```

##### Hardware acceleration

*Header: `<tundra/gte/operations.hpp>`*

A few *hardware accelerated* operations exists for specific combinations of `Vec3` and `Mat3x3` using `Fixed32` and `Fixed16` as template types, including multiplication, normalization and creation of a rotation matrix.

Example:

```c++
Mat3x3<Fixed16<12>> m1;
Mat3x3<Fixed16<12>> m2;

Mat3x3<Fixed16<12>> m3 = td::gte::multiply(m1, m2);
```



## Setup and build



## Benchmarks

