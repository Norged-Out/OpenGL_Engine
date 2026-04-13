// ==================================================
// Author: Priyansh Nayak
// Description: Skybox draw path and internal shader setup
// ==================================================

#include "engine/Skybox.h"
#include "engine/Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

#ifndef ENGINE_SHADER_DIR
#error ENGINE_SHADER_DIR not defined
#endif

Skybox::Skybox(Cubemap& cubemap)
    : environment(cubemap)
{
    shader = new Shader(
        (std::string(ENGINE_SHADER_DIR) + "skybox.vert").c_str(),
        (std::string(ENGINE_SHADER_DIR) + "skybox.frag").c_str()
    );
    initCube();
}

Skybox::~Skybox()
{
    if (shader) delete shader;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Skybox::initCube()
{
    // 36 vertices, position only
    float vertices[] = {
        // back face
        -1,-1,-1,  1, 1,-1,  1,-1,-1,
         1, 1,-1, -1,-1,-1, -1, 1,-1,
		// front face
        -1,-1, 1,  1,-1, 1,  1, 1, 1,
         1, 1, 1, -1, 1, 1, -1,-1, 1,
		 // left face
        -1, 1, 1, -1, 1,-1, -1,-1,-1,
        -1,-1,-1, -1,-1, 1, -1, 1, 1,
		// right face
         1, 1, 1,  1,-1,-1,  1, 1,-1,
         1,-1,-1,  1, 1, 1,  1,-1, 1,
		 // bottom face
        -1,-1,-1,  1,-1,-1,  1,-1, 1,
         1,-1, 1, -1,-1, 1, -1,-1,-1,
		 // top face
        -1, 1,-1,  1, 1, 1,  1, 1,-1,
         1, 1, 1, -1, 1,-1, -1, 1, 1
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Only position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void Skybox::Draw(const Camera& camera) {
    // Save state
    GLboolean wasCulling = glIsEnabled(GL_CULL_FACE);
    GLint prevCullFace;
    glGetIntegerv(GL_CULL_FACE_MODE, &prevCullFace);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    // draw inner faces
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // Use the internal skybox shader owned by the engine feature.
    shader->Activate();

    // remove translation from view matrix
    glm::mat4 view = glm::mat4(glm::mat3(camera.view));
    shader->setMat4("view", view);
    shader->setMat4("projection", camera.projection);

    environment.Bind(0);
    shader->setInt("environmentMap", 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Restore state
    glCullFace(prevCullFace);
    if (!wasCulling) glDisable(GL_CULL_FACE);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

