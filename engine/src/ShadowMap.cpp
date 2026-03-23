#include "engine/ShadowMap.h"
#include "engine/Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

#ifndef ENGINE_SHADER_DIR
#define ENGINE_SHADER_DIR "OpenGL_Engine/assets/shaders/"
#endif

ShadowMap::ShadowMap(unsigned int width, unsigned int height, ShadowMode mode)
    : width(width), height(height), mode(mode) {
    // Generate framebuffer
    glGenFramebuffers(1, &FBO);
    passShader = new Shader(
        std::string(ENGINE_SHADER_DIR) + "shadow.vert",
        std::string(ENGINE_SHADER_DIR) + "shadow.frag"
    );
    createResources();
}

void ShadowMap::createResources() {
    if (mode == ShadowMode::MSM) {
        createMSMResources();
    } else {
        createDepthResources();
    }
}

void ShadowMap::createDepthResources() {
    // Generate depth texture that will store the baseline shadow map
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

    // No color output when rendering shadows
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Safety check
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ShadowMap framebuffer incomplete!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::createMSMResources() {
    // Store the four depth moments in an RGBA floating-point texture.
    glGenTextures(1, &momentTexture);
    glBindTexture(GL_TEXTURE_2D, momentTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, momentTexture, 0);

    // Keep a depth attachment for rasterization correctness during the shadow pass.
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ShadowMap MSM framebuffer incomplete!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ShadowMap::Begin() {
    glViewport(0, 0, width, height);
    bindCurrentFramebuffer();
    if (mode == ShadowMode::MSM) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}


void ShadowMap::End() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ShadowMap::BindTexture(GLuint unit) const {
    if (mode == ShadowMode::MSM) {
        BindMomentTexture(unit);
    } else {
        BindDepthTexture(unit);
    }
}

void ShadowMap::BindDepthTexture(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
}

void ShadowMap::BindMomentTexture(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, momentTexture);
}


void ShadowMap::setDirectionalLight(const glm::vec3& lightDir,
    float orthoSize, float nearPlane, float farPlane) {
    // Position the light some distance away in its direction
    glm::vec3 lightPos = -lightDir * 10.0f;

    // View matrix from light perspective (look at origin)
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0,1,0));

    // Orthographic projection (for directional lights)
    lightProjection = glm::ortho(
        -orthoSize, orthoSize,
        -orthoSize, orthoSize,
        nearPlane, farPlane
    );

    lightSpaceMatrix = lightProjection * lightView;
}


void ShadowMap::applyUniforms(Shader& shader, const char* uniformName) const {
    shader.setMat4(uniformName, lightSpaceMatrix);
}

void ShadowMap::bindPassShader() const {
    if (!passShader) return;

    passShader->Activate();
    passShader->setInt("shadowMode", static_cast<int>(mode));
    applyUniforms(*passShader);
}

Shader& ShadowMap::getPassShader() const {
    return *passShader;
}

void ShadowMap::setMode(ShadowMode newMode) {
    if (mode == newMode) return;

    Delete();
    mode = newMode;
    glGenFramebuffers(1, &FBO);
    passShader = new Shader(
        std::string(ENGINE_SHADER_DIR) + "shadow.vert",
        std::string(ENGINE_SHADER_DIR) + "shadow.frag"
    );
    createResources();
}

void ShadowMap::bindCurrentFramebuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}


void ShadowMap::Delete() {
    if (depthTexture) glDeleteTextures(1, &depthTexture);

    if (momentTexture) glDeleteTextures(1, &momentTexture);

    if (depthRBO) glDeleteRenderbuffers(1, &depthRBO);

    if (FBO) glDeleteFramebuffers(1, &FBO);

    delete passShader;
    passShader = nullptr;

    depthTexture = 0;
    momentTexture = 0;
    depthRBO = 0;
    FBO = 0;
}
