#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cstddef>

class Shader;

enum class ShadowMode {
    Depth = 0,
    MSM
};

class ShadowMap
{
public:
    // Constructor builds the framebuffer and shadow resources for the selected mode
    ShadowMap(unsigned int width, unsigned int height, ShadowMode mode = ShadowMode::Depth);
    // Destructor
    ~ShadowMap() {
        Delete();
    }

    // Prevent copying
    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

    // Bind shadow framebuffer for depth rendering
    void Begin();

    // Return to default framebuffer
    void End();

    // Bind the active shadow texture used for sampling in the lighting pass
    void BindTexture(GLuint unit) const;

    // Explicit bind helpers for comparison/debug workflows
    void BindDepthTexture(GLuint unit) const;
    void BindMomentTexture(GLuint unit) const;

    // Update light-space matrices using a directional-light style setup
    void setDirectionalLight(const glm::vec3& lightDir, float orthoSize, float nearPlane, float farPlane);

    // Send light-space matrix to a shader
    void applyUniforms(Shader& shader, const char* uniformName = "lightSpaceMatrix") const;

    // Activate the internal shadow-pass shader and apply shared uniforms
    void bindPassShader() const;

    // Access the internal shadow-pass shader for per-draw uniforms like model matrices
    Shader& getPassShader() const;

    // Get light space matrix
    const glm::mat4& getLightMatrix() const { return lightSpaceMatrix; }

    ShadowMode getMode() const { return mode; }
    void setMode(ShadowMode newMode);
    void resize(unsigned int newWidth, unsigned int newHeight);

    void setUseSignedDepth(bool enabled) { useSignedDepth = enabled; }
    bool getUseSignedDepth() const { return useSignedDepth; }

    void setBlurEnabled(bool enabled) { blurEnabled = enabled; }
    bool isBlurEnabled() const { return blurEnabled; }

    void setBlurScale(float scale) { blurScale = scale; }
    float getBlurScale() const { return blurScale; }

    // Debug access
    GLuint getDepthTexture() const { return depthTexture; }
    GLuint getMomentTexture() const { return momentTexture; }
    GLuint getDebugTexture() const { return mode == ShadowMode::MSM ? momentTexture : depthTexture; }
    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    size_t getApproxMemoryBytes() const;
    double getLastBlurMs() const { return lastBlurMs; }

    // Cleanup
    void Delete();

private:
    void createResources();
    void createDepthResources();
    void createMSMResources();
    void bindCurrentFramebuffer() const;
    void applyMomentBlur();
    void ensureBlurResources();

    // GPU resources
    GLuint FBO = 0;
    GLuint depthTexture = 0;
    GLuint momentTexture = 0;
    GLuint depthRBO = 0;
    GLuint blurFBO = 0;
    GLuint blurTexture = 0;
    GLuint blurVAO = 0;
    Shader* passShader = nullptr;
    Shader* blurShader = nullptr;

    // Shadow map resolution
    unsigned int width;
    unsigned int height;
    ShadowMode mode = ShadowMode::Depth;
    bool useSignedDepth = true;
    bool blurEnabled = true;
    float blurScale = 1.0f;
    double lastBlurMs = 0.0;

    // Light matrices
    glm::mat4 lightView = glm::mat4(1.0f);
    glm::mat4 lightProjection = glm::mat4(1.0f);
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
};
