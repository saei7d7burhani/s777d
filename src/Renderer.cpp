/**
 * =============================================================================
 * Renderer.cpp - OpenGL Rendering System Implementation
 * =============================================================================
 */

#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"

#include <glad/glad.h>
#include <algorithm>

// Embedded shader sources for the main rendering shader
static const char* VERTEX_SHADER_SOURCE = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
    // Transform position to world space for lighting calculations
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normal to world space
    // Use normal matrix to handle non-uniform scaling correctly
    Normal = normalMatrix * aNormal;
    
    // Pass texture coordinates through
    TexCoords = aTexCoords;
    
    // Final clip-space position
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

static const char* FRAGMENT_SHADER_SOURCE = R"(
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float opacity;
};

// Directional light (like the sun)
struct DirLight {
    bool enabled;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Point light (like a light bulb)
struct PointLight {
    bool enabled;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

// Spot light (like a flashlight)
struct SpotLight {
    bool enabled;
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int numPointLights;
uniform int numSpotLights;
uniform vec3 viewPos;

// Function declarations
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    // Normalize interpolated normal
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Start with no light contribution
    vec3 result = vec3(0.0);
    
    // Directional light
    if (dirLight.enabled) {
        result += CalcDirLight(dirLight, norm, viewDir);
    }
    
    // Point lights
    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; i++) {
        if (pointLights[i].enabled) {
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
        }
    }
    
    // Spot lights
    for (int i = 0; i < numSpotLights && i < MAX_SPOT_LIGHTS; i++) {
        if (spotLights[i].enabled) {
            result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
        }
    }
    
    FragColor = vec4(result, material.opacity);
}

// =============================================================================
// Directional Light Calculation
// =============================================================================
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    
    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffuse (Lambertian)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    
    return ambient + diffuse + specular;
}

// =============================================================================
// Point Light Calculation
// =============================================================================
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                               light.quadratic * distance * distance);
    
    return (ambient + diffuse + specular) * attenuation;
}

// =============================================================================
// Spot Light Calculation
// =============================================================================
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    
    // Spotlight intensity (soft edges)
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                               light.quadratic * distance * distance);
    
    return (ambient + (diffuse + specular) * intensity) * attenuation;
}
)";

// =============================================================================
// Constructor / Destructor
// =============================================================================

Renderer::Renderer(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_directionalLight(nullptr)
    , m_clearColor(0.1f, 0.1f, 0.15f)
    , m_wireframeMode(false)
    , m_cullingEnabled(true)
    , m_drawCallCount(0)
    , m_triangleCount(0)
{
    createShaders();
    setupRenderState();
}

Renderer::~Renderer() = default;

// =============================================================================
// Frame Management
// =============================================================================

void Renderer::beginFrame() {
    // Reset statistics
    m_drawCallCount = 0;
    m_triangleCount = 0;
    
    // Clear render queues
    m_opaqueCommands.clear();
    m_transparentCommands.clear();
    
    // Clear lights (they get re-added each frame)
    m_pointLights.clear();
    m_spotLights.clear();
    m_directionalLight = nullptr;
    
    // Clear the screen
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    // Activate shader
    m_shader->use();
    
    // Set camera matrices
    m_shader->setMat4("view", m_viewMatrix);
    m_shader->setMat4("projection", m_projectionMatrix);
    m_shader->setVec3("viewPos", m_cameraPosition);
    
    // Apply lighting
    applyLighting();
    
    // Render opaque objects first (any order, depth test handles visibility)
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    
    for (const auto& cmd : m_opaqueCommands) {
        executeCommand(cmd);
    }
    
    // Sort and render transparent objects (back to front)
    sortTransparentCommands();
    
    glDepthMask(GL_FALSE);  // Don't write to depth buffer
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (const auto& cmd : m_transparentCommands) {
        executeCommand(cmd);
    }
    
    // Restore state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Renderer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}

// =============================================================================
// Camera Setup
// =============================================================================

void Renderer::setCamera(const Camera& camera) {
    m_viewMatrix = camera.getViewMatrix();
    m_projectionMatrix = camera.getProjectionMatrix(
        static_cast<float>(m_width) / static_cast<float>(m_height));
    m_cameraPosition = camera.getPosition();
}

// =============================================================================
// Lighting Setup
// =============================================================================

void Renderer::setDirectionalLight(const DirectionalLight& light) {
    m_directionalLight = const_cast<DirectionalLight*>(&light);
}

void Renderer::addPointLight(const PointLight& light) {
    if (m_pointLights.size() < MAX_POINT_LIGHTS) {
        m_pointLights.push_back(light);
    }
}

void Renderer::addSpotLight(const SpotLight& light) {
    if (m_spotLights.size() < MAX_SPOT_LIGHTS) {
        m_spotLights.push_back(light);
    }
}

void Renderer::clearLights() {
    m_pointLights.clear();
    m_spotLights.clear();
    m_directionalLight = nullptr;
}

// =============================================================================
// Rendering
// =============================================================================

void Renderer::submit(const Model& model, const glm::mat4& transform) {
    RenderCommand cmd;
    cmd.model = &model;
    cmd.transform = transform;
    cmd.transparent = model.getMaterial().isTransparent();
    
    if (cmd.transparent) {
        // Calculate distance to camera for sorting
        glm::vec3 modelPos = glm::vec3(transform[3]);
        cmd.distanceToCamera = glm::length(m_cameraPosition - modelPos);
        m_transparentCommands.push_back(cmd);
    } else {
        m_opaqueCommands.push_back(cmd);
    }
}

void Renderer::drawImmediate(const Model& model, Shader& shader) {
    shader.use();
    model.draw(shader);
    m_drawCallCount++;
}

// =============================================================================
// Render Settings
// =============================================================================

void Renderer::setClearColor(const glm::vec3& color) {
    m_clearColor = color;
}

void Renderer::setWireframe(bool enabled) {
    m_wireframeMode = enabled;
    glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
}

void Renderer::setCulling(bool enabled) {
    m_cullingEnabled = enabled;
    if (enabled) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

// =============================================================================
// Private Methods
// =============================================================================

void Renderer::setupRenderState() {
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Enable back-face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Set default clear color
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, 1.0f);
}

void Renderer::applyLighting() {
    // Apply directional light
    if (m_directionalLight) {
        m_directionalLight->applyToShader(*m_shader, "dirLight");
    } else {
        m_shader->setBool("dirLight.enabled", false);
    }
    
    // Apply point lights
    m_shader->setInt("numPointLights", static_cast<int>(m_pointLights.size()));
    for (size_t i = 0; i < m_pointLights.size(); i++) {
        std::string name = "pointLights[" + std::to_string(i) + "]";
        m_pointLights[i].applyToShader(*m_shader, name);
    }
    
    // Disable unused point lights
    for (size_t i = m_pointLights.size(); i < MAX_POINT_LIGHTS; i++) {
        std::string name = "pointLights[" + std::to_string(i) + "].enabled";
        m_shader->setBool(name, false);
    }
    
    // Apply spot lights
    m_shader->setInt("numSpotLights", static_cast<int>(m_spotLights.size()));
    for (size_t i = 0; i < m_spotLights.size(); i++) {
        std::string name = "spotLights[" + std::to_string(i) + "]";
        m_spotLights[i].applyToShader(*m_shader, name);
    }
    
    // Disable unused spot lights
    for (size_t i = m_spotLights.size(); i < MAX_SPOT_LIGHTS; i++) {
        std::string name = "spotLights[" + std::to_string(i) + "].enabled";
        m_shader->setBool(name, false);
    }
}

void Renderer::sortTransparentCommands() {
    // Sort back to front (furthest first)
    std::sort(m_transparentCommands.begin(), m_transparentCommands.end(),
        [](const RenderCommand& a, const RenderCommand& b) {
            return a.distanceToCamera > b.distanceToCamera;
        });
}

void Renderer::executeCommand(const RenderCommand& cmd) {
    if (cmd.model && cmd.model->isVisible()) {
        cmd.model->draw(*m_shader, cmd.transform);
        m_drawCallCount++;
    }
}

void Renderer::createShaders() {
    m_shader = std::make_unique<Shader>(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE, false);
}
