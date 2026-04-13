// ==================================================
// Author: Priyansh Nayak
// Description: Shadow-map subsystem supporting both depth and MSM resources
// ==================================================

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
    // Params: unsigned int width, unsigned int height, ShadowMode mode = ShadowMode::Depth
    ShadowMap(unsigned int width, unsigned int height, ShadowMode mode = ShadowMode::Depth);
    // Destructor
    ~ShadowMap() {
        // Description: Delete
        // Params: none
        Delete();
    }

    // Prevent copying
    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

    // Bind shadow framebuffer for depth rendering
    // Params: none
    void Begin();

    // Return to default framebuffer
    // Params: none
    void End();

    // Bind the active shadow texture used for sampling in the lighting pass
    // Params: GLuint unit
    void BindTexture(GLuint unit) const;

    // Explicit bind helpers for comparison/debug workflows
    // Params: GLuint unit
    void BindDepthTexture(GLuint unit) const;
    void BindMomentTexture(GLuint unit) const;

    // Update light-space matrices using a directional-light style setup
    // Params: const glm::vec3& lightDir, float orthoSize, float nearPlane, float farPlane
    void setDirectionalLight(const glm::vec3& lightDir, float orthoSize, float nearPlane, float farPlane);

    // Send light-space matrix to a shader
    // Params: Shader& shader, const char* uniformName = "lightSpaceMatrix"
    void applyUniforms(Shader& shader, const char* uniformName = "lightSpaceMatrix") const;

    // Activate the internal shadow-pass shader and apply shared uniforms
    // Params: none
    void bindPassShader() const;

    // Access the internal shadow-pass shader for per-draw uniforms like model matrices
    // Params: none
    Shader& getPassShader() const;

    // Get light space matrix
    const glm::mat4& getLightMatrix() const { return lightSpaceMatrix; }

    ShadowMode getMode() const { return mode; }
    // Description: setMode
    // Params: ShadowMode newMode
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
    // Description: getApproxMemoryBytes
    // Params: none
    size_t getApproxMemoryBytes() const;
    double getLastBlurMs() const { return lastBlurMs; }

    // Cleanup
    // Params: none
    void Delete();

private:
    // Description: createResources
    // Params: none
    void createResources();
    void createDepthResources();
    // Description: createMSMResources
    // Params: none
    void createMSMResources();
    void bindCurrentFramebuffer() const;
    // Description: applyMomentBlur
    // Params: none
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

