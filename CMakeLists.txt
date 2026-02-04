cmake_minimum_required(VERSION 3.20)

project(OpenGL_Engine)

set(CMAKE_CXX_STANDARD 17)

# ---------- FIND PACKAGES ----------
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# ---------- GLAD ----------
add_library(glad
    third_party/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    third_party/glad/include
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
)

target_include_directories(engine PUBLIC
    engine/include
    ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/include
)

target_link_libraries(engine PUBLIC
    glad
    glfw
    glm::glm
    assimp::assimp
    imgui::imgui
)

# ---- for HDRI to cubemap conversion ----
target_compile_definitions(engine PUBLIC
    ENGINE_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/"
)