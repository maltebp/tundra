# Tundra
The *runtime* part of a general-purpose 3D engine for the original PlayStation console (PlayStation 1 / PSX), along with a few CLI tools to ease development and use of the engine. 

The engine was created as part of my Master's thesis, and has been used to create the game [Color Cannon Coop ](https://jesperpapiorgmailcom.itch.io/colorcannoncoop)at Nordic Game Jam 2024.

This repository is not under further development, as I'm planning on rebooting the project under a new one. This will be linked here, once that is started



TODO: Mention psn00bsdk

TODO: Mention that it is in C++



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



### Missing features

- No CD support...



## How to use

### Setup game

To use the engine for a game, setup the game with the [Tundra game template](https://github.com/maltebp/tundra-template). See the repository, for how to use the template.



### Setup and game loop



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

- `type`: type of the asset, either `MODEL`, `TEXTURE`, `SOUND`
- `code`: the name of the global variable pointing to the asset data
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

#### 

### Entity system

### Renderer

### Models

### Sprites

### Text

### Audio

### Input



## Setup and build



## Benchmarks

...



☐ Features

☐ Known bugs

☐ About the extra benchmarks 

☐ How to use the engine

☐ Documentation on features (just examples)

☐ Mention the project is discontinued