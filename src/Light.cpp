/**
 * =============================================================================
 * Light.cpp - Lighting System Implementation
 * =============================================================================
 */

#include "Light.h"
#include "Shader.h"

#include <cmath>

// =============================================================================
// Base Light
// =============================================================================

Light::Light()
    : ambient(0.1f)
    , diffuse(0.8f)
    , specular(1.0f)
    , enabled(true)
{
}

// =============================================================================
// Directional Light
// =============================================================================

DirectionalLight::DirectionalLight()
    : Light()
    , direction(-0.2f, -1.0f, -0.3f)
{
}

DirectionalLight::DirectionalLight(const glm::vec3& dir, 
                                   const glm::vec3& amb,
                                   const glm::vec3& diff,
                                   const glm::vec3& spec)
    : Light()
    , direction(glm::normalize(dir))
{
    ambient = amb;
    diffuse = diff;
    specular = spec;
}

void DirectionalLight::applyToShader(Shader& shader, const std::string& uniformName) const {
    shader.setBool(uniformName + ".enabled", enabled);
    shader.setVec3(uniformName + ".direction", direction);
    shader.setVec3(uniformName + ".ambient", ambient);
    shader.setVec3(uniformName + ".diffuse", diffuse);
    shader.setVec3(uniformName + ".specular", specular);
}

// =============================================================================
// Point Light
// =============================================================================

PointLight::PointLight()
    : Light()
    , position(0.0f)
    , constant(1.0f)
    , linear(0.09f)
    , quadratic(0.032f)
{
}

PointLight::PointLight(const glm::vec3& pos,
                       const glm::vec3& amb,
                       const glm::vec3& diff,
                       const glm::vec3& spec,
                       float c, float l, float q)
    : Light()
    , position(pos)
    , constant(c)
    , linear(l)
    , quadratic(q)
{
    ambient = amb;
    diffuse = diff;
    specular = spec;
}

void PointLight::applyToShader(Shader& shader, const std::string& uniformName) const {
    shader.setBool(uniformName + ".enabled", enabled);
    shader.setVec3(uniformName + ".position", position);
    shader.setVec3(uniformName + ".ambient", ambient);
    shader.setVec3(uniformName + ".diffuse", diffuse);
    shader.setVec3(uniformName + ".specular", specular);
    shader.setFloat(uniformName + ".constant", constant);
    shader.setFloat(uniformName + ".linear", linear);
    shader.setFloat(uniformName + ".quadratic", quadratic);
}

void PointLight::setRange(float range) {
    // Calculate attenuation coefficients for a given range
    // Based on empirical values from Ogre3D
    constant = 1.0f;
    linear = 4.5f / range;
    quadratic = 75.0f / (range * range);
}

// =============================================================================
// Spot Light
// =============================================================================

SpotLight::SpotLight()
    : Light()
    , position(0.0f, 3.0f, 0.0f)
    , direction(0.0f, -1.0f, 0.0f)
    , innerCutoff(12.5f)
    , outerCutoff(17.5f)
    , constant(1.0f)
    , linear(0.09f)
    , quadratic(0.032f)
{
}

SpotLight::SpotLight(const glm::vec3& pos,
                     const glm::vec3& dir,
                     const glm::vec3& amb,
                     const glm::vec3& diff,
                     const glm::vec3& spec,
                     float inner, float outer)
    : Light()
    , position(pos)
    , direction(glm::normalize(dir))
    , innerCutoff(inner)
    , outerCutoff(outer)
    , constant(1.0f)
    , linear(0.09f)
    , quadratic(0.032f)
{
    ambient = amb;
    diffuse = diff;
    specular = spec;
}

void SpotLight::applyToShader(Shader& shader, const std::string& uniformName) const {
    shader.setBool(uniformName + ".enabled", enabled);
    shader.setVec3(uniformName + ".position", position);
    shader.setVec3(uniformName + ".direction", direction);
    shader.setVec3(uniformName + ".ambient", ambient);
    shader.setVec3(uniformName + ".diffuse", diffuse);
    shader.setVec3(uniformName + ".specular", specular);
    
    // Store cutoff as cosine for efficient comparison in shader
    shader.setFloat(uniformName + ".cutOff", std::cos(glm::radians(innerCutoff)));
    shader.setFloat(uniformName + ".outerCutOff", std::cos(glm::radians(outerCutoff)));
    
    shader.setFloat(uniformName + ".constant", constant);
    shader.setFloat(uniformName + ".linear", linear);
    shader.setFloat(uniformName + ".quadratic", quadratic);
}

void SpotLight::setCutoff(float innerDegrees, float outerDegrees) {
    innerCutoff = innerDegrees;
    outerCutoff = outerDegrees;
}
