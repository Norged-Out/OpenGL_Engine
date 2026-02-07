cmake_minimum_required(VERSION 3.20)

project(OpenGL_Engine)

set(CMAKE_CXX_STANDARD 17)

# ---------- GLFW ----------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/glfw)

# ---------- FIND PACKAGES ----------
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# ---------- GLAD ----------
add_library(glad
    third_party/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    third_party/glad/include
)

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