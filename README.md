# OpenGL_Engine

A personal, reusable OpenGL engine scaffold built to support multiple coursework projects without copy-pasting boilerplate every time.

This repository is intended to be used as a **vendored engine** (via `add_subdirectory` or Git submodules), not as a system-installed library.

---

## Features

- Modern OpenGL setup
- Reusable engine core (no `main()` in this repo)
- Model loading via Assimp (OBJ / FBX / glTF / GLB)
- Windowing via GLFW
- Math via GLM
- UI via Dear ImGui (GLFW + OpenGL3 backend)
- Texture loading via stb_image
- Clean CMake target boundaries
- No package manager required

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

Each project/assignment includes the engine as a **Git submodule**.

Example layout:
```
Assignment1/
├── OpenGL_Engine/ ← submodule
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
project(Assignment1)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(OpenGL_Engine)

add_executable(app
    Main.cpp
)

target_link_libraries(app PRIVATE engine)

# Assignment-specific asset root
target_compile_definitions(app PRIVATE
    ENGINE_ASSET_ROOT="${CMAKE_SOURCE_DIR}"
)
```
### Important:
Do not modify engine targets from the consumer project.
All configuration that varies per assignment should apply to the app target, not ```engine```.

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

Each project pins its own engine version to avoid breaking older assignments.

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
- Engine code must not assume project layout
- Consumer projects configure behavior via compile definitions
- No hidden include paths
- No global state

---

## Non-Goals

- This engine is not intended to be installed system-wide
- This engine is not a framework
- This engine does not provide a default application entry point
- This engine is not optimized for distribution

---

## License

Personal/educational use.
