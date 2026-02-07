cmake_minimum_required(VERSION 3.20)

project(OpenGL_Engine)

set(CMAKE_CXX_STANDARD 17)

# ---------- GLFW ----------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/glfw)

# ---------- GLAD ----------
add_library(glad
    third_party/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    third_party/glad/include
)

# ---------- STB ----------
add_library(stb INTERFACE)

target_include_directories(stb INTERFACE
    third_party/stb
)

# ---------- GLM ----------
add_library(glm INTERFACE)
add_library(glm::glm ALIAS glm)

target_include_directories(glm INTERFACE
    third_party/glm
)

# ---------- ASSIMP ----------
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

# static library
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# importers: start from nothing
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

# enable only what you want
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)

# kill common bloat
set(ASSIMP_ENABLE_DRACO OFF CACHE BOOL "" FORCE)
set(ASSIMP_ENABLE_ZLIB OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/assimp)

# ---------- IMGUI ----------
add_library(imgui
    third_party/imgui/src/imgui.cpp
    third_party/imgui/src/imgui_draw.cpp
    third_party/imgui/src/imgui_tables.cpp
    third_party/imgui/src/imgui_widgets.cpp
    third_party/imgui/src/imgui_impl_glfw.cpp
    third_party/imgui/src/imgui_impl_opengl3.cpp
)

add_library(imgui::imgui ALIAS imgui)

target_include_directories(imgui PUBLIC
    third_party/imgui/include
)

target_link_libraries(imgui PUBLIC
    glfw
)

# ---------- ENGINE ----------
add_library(engine
    engine/src/Camera.cpp
    engine/src/Cubemap.cpp
    engine/src/EBO.cpp
    engine/src/HDRConverter.cpp
    engine/src/HDRTexture.cpp
    engine/src/Mesh.cpp
    engine/src/Model.cpp
    engine/src/Shader.cpp
    engine/src/Skybox.cpp
    engine/src/Texture.cpp
    engine/src/VAO.cpp
    engine/src/VBO.cpp
    third_party/stb/stb_image.cpp
)

target_include_directories(engine PUBLIC
    engine/include
)

target_link_libraries(engine PUBLIC
    glad
    glfw
    stb
    glm::glm
    assimp::assimp
    imgui::imgui
)

# ---- for HDRI to cubemap conversion ----
target_compile_definitions(engine PUBLIC
    ENGINE_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/"
)