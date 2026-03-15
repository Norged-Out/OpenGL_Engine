#include "engine/ShadowMap.h"
#include "engine/Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


ShadowMap::ShadowMap(unsigned int width, unsigned int height)
    : width(width), height(height) {
    // Generate framebuffer
    glGenFramebuffers(1, &FBO);

    // Generate depth texture that will store the shadow map
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    // Allocate texture storage (depth only)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Filtering (important for shadow sampling later)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Prevent sampling outside the shadow map
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    // We don't need color output when rendering shadows
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Safety check
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ShadowMap framebuffer incomplete!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ShadowMap::Begin() {
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}


void ShadowMap::End() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ShadowMap::BindTexture(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
}


void ShadowMap::setDirectionalLight(const glm::vec3& lightDir,
    float orthoSize, float nearPlane, float farPlane) {
    // Position the light some distance away in its direction
    glm::vec3 lightPos = -lightDir * 10.0f;

    // View matrix from light perspective
    lightView = glm::lookAt(
        lightPos,
        glm::vec3(0.0f),     // look at origin
        glm::vec3(0,1,0)
    );

    // Orthographic projection (standard for directional lights)
    lightProjection = glm::ortho(
        -orthoSize, orthoSize,
        -orthoSize, orthoSize,
        nearPlane,
        farPlane
    );

    lightSpaceMatrix = lightProjection * lightView;
}


void ShadowMap::applyUniforms(Shader& shader, const char* uniformName) const {
    shader.setMat4(uniformName, lightSpaceMatrix);
}


void ShadowMap::Delete() {
    if (depthTexture)
        glDeleteTextures(1, &depthTexture);

    if (FBO)
        glDeleteFramebuffers(1, &FBO);
}