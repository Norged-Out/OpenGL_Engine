#include "engine/ShadowMap.h"
#include "engine/Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iostream>
#include <string>

#ifndef ENGINE_SHADER_DIR
#define ENGINE_SHADER_DIR "OpenGL_Engine/assets/shaders/"
#endif

ShadowMap::ShadowMap(unsigned int width, unsigned int height, ShadowMode mode)
    : width(width), height(height), mode(mode) {
    // Generate framebuffer
    glGenFramebuffers(1, &FBO);

    // One shader renders the shadow data, the other blurs MSM moments afterward
    passShader = new Shader(
        std::string(ENGINE_SHADER_DIR) + "shadow.vert",
        std::string(ENGINE_SHADER_DIR) + "shadow.frag"
    );
    blurShader = new Shader(
        std::string(ENGINE_SHADER_DIR) + "moment_blur.vert",
        std::string(ENGINE_SHADER_DIR) + "moment_blur.frag"
    );
    createResources();
}

void ShadowMap::createResources() {
    // Depth mode keeps the classic shadow map path, MSM mode allocates moment textures
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
    // Store the four depth moments in an RGBA floating-point texture
    glGenTextures(1, &momentTexture);
    glBindTexture(GL_TEXTURE_2D, momentTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, momentTexture, 0);

    // Keep a depth attachment for rasterization correctness during the shadow pass
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

    ensureBlurResources();
}


void ShadowMap::Begin() {
    glViewport(0, 0, width, height);
    bindCurrentFramebuffer();

    // MSM writes color + depth, while the baseline only needs a depth clear
    if (mode == ShadowMode::MSM) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}


void ShadowMap::End() {
    lastBlurMs = 0.0;
    if (mode == ShadowMode::MSM && blurEnabled) {
        applyMomentBlur();
    }
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
    // Bind the baseline depth texture for hard shadows or PCF
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
}

void ShadowMap::BindMomentTexture(GLuint unit) const {
    // Bind the filtered moment texture for MSM reconstruction
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

    // Push the shared shadow-pass state before the app draws each caster
    passShader->Activate();
    passShader->setInt("shadowMode", static_cast<int>(mode));
    passShader->setBool("useSignedMSMDepth", useSignedDepth);
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
    blurShader = new Shader(
        std::string(ENGINE_SHADER_DIR) + "moment_blur.vert",
        std::string(ENGINE_SHADER_DIR) + "moment_blur.frag"
    );
    createResources();
}

void ShadowMap::resize(unsigned int newWidth, unsigned int newHeight) {
    if (width == newWidth && height == newHeight) return;

    width = newWidth;
    height = newHeight;

    Delete();
    glGenFramebuffers(1, &FBO);
    passShader = new Shader(
        std::string(ENGINE_SHADER_DIR) + "shadow.vert",
        std::string(ENGINE_SHADER_DIR) + "shadow.frag"
    );
    blurShader = new Shader(
        std::string(ENGINE_SHADER_DIR) + "moment_blur.vert",
        std::string(ENGINE_SHADER_DIR) + "moment_blur.frag"
    );
    createResources();
}

void ShadowMap::bindCurrentFramebuffer() const {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void ShadowMap::ensureBlurResources() {
    // Create a tiny post-process setup once and reuse it every frame
    if (blurFBO == 0) {
        glGenFramebuffers(1, &blurFBO);
    }

    if (blurTexture == 0) {
        // Temporary texture that holds the horizontal blur result
        glGenTextures(1, &blurTexture);
        glBindTexture(GL_TEXTURE_2D, blurTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    if (blurVAO == 0) {
        // Fullscreen triangle draw needs only a VAO in core OpenGL
        glGenVertexArrays(1, &blurVAO);
    }
}

void ShadowMap::applyMomentBlur() {
    if (!blurShader || !momentTexture || !blurTexture || !blurFBO || !blurVAO) {
        return;
    }

    auto blurStart = std::chrono::high_resolution_clock::now();

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(blurVAO);

    blurShader->Activate();
    blurShader->setInt("sourceTexture", 0);
    blurShader->setVec2("texelSize", glm::vec2(1.0f / width, 1.0f / height));
    blurShader->setFloat("blurScale", blurScale);

    // Blur horizontally from the raw moment map into a temporary texture
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture, 0);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, momentTexture);
    blurShader->setVec2("blurDirection", glm::vec2(1.0f, 0.0f));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Blur vertically back into the main moment texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, momentTexture, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, blurTexture);
    blurShader->setVec2("blurDirection", glm::vec2(0.0f, 1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);

    // The blur is just a post-process step, so restore depth testing for normal rendering
    glEnable(GL_DEPTH_TEST);

    auto blurEnd = std::chrono::high_resolution_clock::now();
    lastBlurMs = std::chrono::duration<double, std::milli>(blurEnd - blurStart).count();
}

size_t ShadowMap::getApproxMemoryBytes() const {
    // Estimate only the shadow resources this subsystem owns right now
    size_t depthBytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 4ull;
    size_t momentBytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 16ull;
    size_t depthRboBytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 4ull;
    size_t blurBytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 16ull;

    if (mode == ShadowMode::MSM) {
        return momentBytes + depthRboBytes + blurBytes;
    }

    return depthBytes;
}


void ShadowMap::Delete() {
    if (depthTexture) glDeleteTextures(1, &depthTexture);

    if (momentTexture) glDeleteTextures(1, &momentTexture);

    if (blurTexture) glDeleteTextures(1, &blurTexture);

    if (depthRBO) glDeleteRenderbuffers(1, &depthRBO);

    if (FBO) glDeleteFramebuffers(1, &FBO);

    if (blurFBO) glDeleteFramebuffers(1, &blurFBO);

    if (blurVAO) glDeleteVertexArrays(1, &blurVAO);

    delete passShader;
    passShader = nullptr;

    delete blurShader;
    blurShader = nullptr;

    depthTexture = 0;
    momentTexture = 0;
    blurTexture = 0;
    depthRBO = 0;
    FBO = 0;
    blurFBO = 0;
    blurVAO = 0;
}
