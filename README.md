# OpenGL_Engine

A personal, reusable OpenGL engine scaffold built to support multiple coursework projects without copy-pasting boilerplate every time.

This repository builds a reusable rendering **library**, not an executable.  
It is intended to be used as a **vendored engine** by being included in projects via `add_subdirectory` or Git submodules.

---

## Features

- Modern OpenGL rendering pipeline
- Reusable engine core (no `main()` in this repo)
- Procedural geometry (cube, sphere, pyramid, ring, plane)
- Model loading via Assimp (OBJ / FBX / glTF / GLB)
- PBR-style material system
- Texture system with runtime filtering + wrapping control
- Skybox rendering using custom HDRI to Cubemap converter and stb_image
- Internal engine shader assets for reusable rendering systems
- Directional shadow map system with baseline depth + MSM support
- Camera system (free + cinematic modes)
- UI integration via Dear ImGui (GLFW + OpenGL3 backend)
- Clean CMake target boundaries
- Fully vendored dependencies (no package manager required)

---

## Core Architecture

The engine is structured around a strict separation of responsibilities.  
Each system owns a clearly defined part of the rendering pipeline.

---

### Rendering Layer

#### Shader
Encapsulates an OpenGL shader program.

Responsible for:
- Compilation and linking
- Program activation
- Uniform uploads

Contains no scene logic and owns only the GPU program object.

---

#### VBO (Vertex Buffer Object)
Wraps a GPU vertex buffer.

Responsible for:
- Uploading vertex data
- Binding/unbinding

Does not know about vertex semantics.

---

#### EBO (Element Buffer Object)
Wraps GPU index buffer storage.

Responsible for:
- Uploading index data
- Binding/unbinding

---

#### VAO (Vertex Array Object)
Stores vertex attribute layout configuration.

Responsible for:
- Linking VBO attribute layouts
- Preserving vertex format state

Separates geometry data from layout description.

---

### Scene Layer

#### Mesh
Represents drawable geometry.

Owns:
- Vertex data
- Index data
- VAO / VBO / EBO
- Optional Material reference

Does **not** own:
- World transform
- Texture semantics
- Scene logic

Mesh is purely a renderable unit.

---

#### Material
Defines surface appearance.

Maps semantic identifiers (e.g., `"diffuse"`, `"normal"`, `"roughness"`) to textures and handles texture binding during draw calls.

Contains no geometry and no transform data.

---

#### Texture
Encapsulates an OpenGL texture resource.

Responsible for:
- GPU allocation
- Image upload
- Mipmap generation
- Filtering state
- Wrapping state

Textures are semantic-agnostic — they do not know how they will be used.

---

#### Model
High-level scene object.

Owns:
- Transform state (position, rotation, scale)
- Collection of meshes
- Texture cache for loaded assets

Combines its transform with mesh data during rendering.

---

### Utility Layer

#### MathUtils
Provides:
- Deterministic TRS matrix construction
- Quaternion helpers
- Rotation order control

Used for explicit transform construction rather than hidden matrix logic.

#### Geometry
Provides:
- Procedural mesh generation (Cube, Pyramid, Sphere, Ring, Plane)
- Tangent computation for normal mapping

Encapsulates reusable primitive construction separate from rendering and scene logic.

---

#### Camera
Encapsulates:
- View matrix construction
- Projection matrix configuration
- Input handling (free/cinematic modes)
- Matrix export to shaders

Defines how the scene is viewed, but does not modify scene objects.

---

### Engine-Owned Rendering Systems

#### Skybox
Owns:
- Cubemap rendering state
- Internal skybox shader loaded from `assets/shaders`

Consumer projects only need to create the cubemap and call `Draw()`.

---

#### ShadowMap
Owns:
- Shadow framebuffer resources
- Light-space matrix setup
- Internal shadow pass shader loaded from `assets/shaders`
- Depth shadow map mode
- MSM moment shadow map mode
- Built-in two-pass Gaussian blur for MSM moment filtering

Consumer projects still decide which objects cast shadows, but the engine owns the reusable shadow pass infrastructure.

---

## Architectural Principles

- Clear ownership boundaries
- No hidden global state
- No implicit texture slot conventions
- Mesh does not store transform logic
- Texture does not store semantic meaning
- Model aggregates transforms
- Rendering state changes are explicit

This structure enables:

- Clean instancing
- Safer refactoring
- Runtime texture sampling control (mipmapping demonstrations)
- Stable engine updates across coursework projects
- Predictable rendering behavior
- Easier comparison between baseline shadows and research-inspired shadow techniques

---

## Dependency Model (Important)

All dependencies are **vendored and built from source**:

| Dependency | How it’s handled |
|-----------|------------------|
| GLFW | Vendored (source, static) |
| GLM | Vendored (header-only) |
| ImGui | Vendored (selected files only) |
| Assimp | Vendored (source, trimmed importers) |
| stb | Vendored (flat layout) |
| glad | Vendored |

This engine **does not rely on vcpkg, Conan, or system installs**.

---

## Repository Structure
```code
OpenGL_Engine/
├── engine/
│ ├── include/
│ └── src/
├── third_party/
│ ├── assimp/
│ ├── glfw/
│ ├── glm/
│ ├── imgui/
│ ├── stb/
│ └── glad/
├── assets/
│ └── shaders/
└── CMakeLists.txt
```

This repository builds **only a library**, not an executable.

---

## Intended Usage (Recommended)

### Submodule per Project (Best for Coursework)

Each project includes the engine as a **Git submodule**.

Example layout:
```
Project/
├── OpenGL_Engine/  (submodule)
├── CMakeLists.txt
└── src/
```

### Add the engine as a submodule
```code
git submodule add https://github.com/Norged-Out/OpenGL_Engine.git OpenGL_Engine
```

---

## Consumer Project CMake Example

```cmake
cmake_minimum_required(VERSION 3.20)
project(Project)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(OpenGL_Engine)

add_executable(app
    Main.cpp
)

target_link_libraries(app PRIVATE engine)

# project-specific asset root
target_compile_definitions(app PRIVATE
    ENGINE_ASSET_ROOT="${CMAKE_SOURCE_DIR}"
)
```
### Important:
Do not modify engine targets from the consumer project.
All configuration that varies per project should apply to the app target, not ```engine```.

---

## Asset Handling Pattern
Consumer projects manage their own assets.

Example post-build copy:
```cmake
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Shaders
        $<TARGET_FILE_DIR:app>/Shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Models
        $<TARGET_FILE_DIR:app>/Models
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Textures
        $<TARGET_FILE_DIR:app>/Textures
)
```

---

## Updating the Engine in a Project

Engine updates are explicit and opt-in.

```code
cd OpenGL_Engine
git pull
cd ..
git add OpenGL_Engine
git commit -m "Update OpenGL_Engine"
```

Each project pins its own engine version to avoid breaking older projects.

---

## Build Instructions
```code
cmake -S . -B build
cmake --build build
```

No package manager or toolchain file is required.

---

## Design Philosophy

- Prefer explicit vendoring over global dependencies
- Favor reproducibility over convenience
- Clear ownership boundaries between systems
- No implicit texture slot conventions
- No hidden include paths
- No global rendering state
- Reusable engine systems may own their own internal shaders
- Reusable engine systems may also own their own internal post-process steps when the feature needs them
- Engine code must not assume consumer project layout
- Consumer projects configure behavior via compile definitions

---

## Non-Goals

- Not intended to be installed system-wide
- Not a general-purpose game engine
- Does not provide a default application entry point
- Does not include a scene editor
- Does not include a runtime asset pipeline
- Does not force consumer projects to use engine-provided scene/material shaders
- Not optimized for distribution or commercial deployment

---

## License

Personal / educational use.
