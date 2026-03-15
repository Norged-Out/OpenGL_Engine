#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader;

class ShadowMap
{
public:
    // Constructor builds the framebuffer + depth texture
    ShadowMap(unsigned int width, unsigned int height);
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

    // Bind depth texture to a texture unit for sampling
    void BindTexture(GLuint unit) const;

    // Update light-space matrices using a directional-light style setup
    void setDirectionalLight(const glm::vec3& lightDir, float orthoSize, float nearPlane, float farPlane);

    // Send light-space matrix to a shader
    void applyUniforms(Shader& shader, const char* uniformName = "lightSpaceMatrix") const;

    // Get light space matrix
    const glm::mat4& getLightMatrix() const { return lightSpaceMatrix; }

    // Debug access
    GLuint getDepthTexture() const { return depthTexture; }

    // Cleanup
    void Delete();

private:
    // GPU resources
    GLuint FBO = 0;
    GLuint depthTexture = 0;

    // Shadow map resolution
    unsigned int width;
    unsigned int height;

    // Light matrices
    glm::mat4 lightView = glm::mat4(1.0f);
    glm::mat4 lightProjection = glm::mat4(1.0f);
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
};