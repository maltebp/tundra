# Tundra
Tundra is the **runtime** part of a general-purpose 3D engine for the **original PlayStation** console (PlayStation 1 / PSX), along with a few CLI tools to ease development and use of the engine. It uses C++20 and [psn00bsdk](https://github.com/Lameguy64/PSn00bSDK) as its underlying SDK, but currently it only supports Windows.

The engine was created as part of my Master's thesis, and has been used to create the game [Color Cannon Coop ](https://jesperpapiorgmailcom.itch.io/colorcannoncoop)at Nordic Game Jam 2024. As such, the engine is somewhat limited and cuts some corners that provided little value for that project.

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

In practice, I primarily desired to spend my time on developing an engine. Formally, however, the motivation was that there seem to be no general-purpose game engines for the PlayStation out there (to my knowledge), which seem to be because of the technical limitations of the console. But I'm not entirely convinced that is reason enough for one to not to exist, and so I formally sought to answer the following hypothesis:

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
my_component->destroy(); // Destroys on this component
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



### Renderer

### Models

### Sprites

### Text

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

- Fixed point
- List
- String
- Input/output
- Asserts
- Matrices vectors



## Setup and build



## Benchmarks

