/**
 * =============================================================================
 * Material.cpp - Surface Material Properties Implementation
 * =============================================================================
 */

#include "Material.h"
#include "Shader.h"

// =============================================================================
// Constructors
// =============================================================================

Material::Material()
    : ambient(0.2f)
    , diffuse(0.8f)
    , specular(0.5f)
    , shininess(32.0f)
    , opacity(1.0f)
    , diffuseMap(0)
    , specularMap(0)
    , normalMap(0)
{
}

Material::Material(const glm::vec3& amb,
                   const glm::vec3& diff,
                   const glm::vec3& spec,
                   float shine)
    : ambient(amb)
    , diffuse(diff)
    , specular(spec)
    , shininess(shine)
    , opacity(1.0f)
    , diffuseMap(0)
    , specularMap(0)
    , normalMap(0)
{
}

// =============================================================================
// Shader Application
// =============================================================================

void Material::applyToShader(Shader& shader, const std::string& uniformName) const {
    shader.setVec3(uniformName + ".ambient", ambient);
    shader.setVec3(uniformName + ".diffuse", diffuse);
    shader.setVec3(uniformName + ".specular", specular);
    shader.setFloat(uniformName + ".shininess", shininess);
    shader.setFloat(uniformName + ".opacity", opacity);
}

// =============================================================================
// Preset Materials - Standard
// =============================================================================

Material Material::Default() {
    return Material(
        glm::vec3(0.2f),
        glm::vec3(0.8f),
        glm::vec3(0.5f),
        32.0f
    );
}

Material Material::Chrome() {
    Material mat(
        glm::vec3(0.25f, 0.25f, 0.25f),
        glm::vec3(0.4f, 0.4f, 0.4f),
        glm::vec3(0.774597f, 0.774597f, 0.774597f),
        76.8f
    );
    return mat;
}

Material Material::Gold() {
    return Material(
        glm::vec3(0.24725f, 0.1995f, 0.0745f),
        glm::vec3(0.75164f, 0.60648f, 0.22648f),
        glm::vec3(0.628281f, 0.555802f, 0.366065f),
        51.2f
    );
}

Material Material::Silver() {
    return Material(
        glm::vec3(0.19225f, 0.19225f, 0.19225f),
        glm::vec3(0.50754f, 0.50754f, 0.50754f),
        glm::vec3(0.508273f, 0.508273f, 0.508273f),
        51.2f
    );
}

Material Material::Bronze() {
    return Material(
        glm::vec3(0.2125f, 0.1275f, 0.054f),
        glm::vec3(0.714f, 0.4284f, 0.18144f),
        glm::vec3(0.393548f, 0.271906f, 0.166721f),
        25.6f
    );
}

Material Material::Copper() {
    return Material(
        glm::vec3(0.19125f, 0.0735f, 0.0225f),
        glm::vec3(0.7038f, 0.27048f, 0.0828f),
        glm::vec3(0.256777f, 0.137622f, 0.086014f),
        12.8f
    );
}

Material Material::Brass() {
    return Material(
        glm::vec3(0.329412f, 0.223529f, 0.027451f),
        glm::vec3(0.780392f, 0.568627f, 0.113725f),
        glm::vec3(0.992157f, 0.941176f, 0.807843f),
        27.8974f
    );
}

Material Material::Emerald() {
    Material mat(
        glm::vec3(0.0215f, 0.1745f, 0.0215f),
        glm::vec3(0.07568f, 0.61424f, 0.07568f),
        glm::vec3(0.633f, 0.727811f, 0.633f),
        76.8f
    );
    mat.opacity = 0.55f;
    return mat;
}

Material Material::Ruby() {
    Material mat(
        glm::vec3(0.1745f, 0.01175f, 0.01175f),
        glm::vec3(0.61424f, 0.04136f, 0.04136f),
        glm::vec3(0.727811f, 0.626959f, 0.626959f),
        76.8f
    );
    mat.opacity = 0.55f;
    return mat;
}

Material Material::Pearl() {
    return Material(
        glm::vec3(0.25f, 0.20725f, 0.20725f),
        glm::vec3(1.0f, 0.829f, 0.829f),
        glm::vec3(0.296648f, 0.296648f, 0.296648f),
        11.264f
    );
}

Material Material::Obsidian() {
    return Material(
        glm::vec3(0.05375f, 0.05f, 0.06625f),
        glm::vec3(0.18275f, 0.17f, 0.22525f),
        glm::vec3(0.332741f, 0.328634f, 0.346435f),
        38.4f
    );
}

Material Material::Turquoise() {
    Material mat(
        glm::vec3(0.1f, 0.18725f, 0.1745f),
        glm::vec3(0.396f, 0.74151f, 0.69102f),
        glm::vec3(0.297254f, 0.30829f, 0.306678f),
        12.8f
    );
    mat.opacity = 0.8f;
    return mat;
}

Material Material::Jade() {
    return Material(
        glm::vec3(0.135f, 0.2225f, 0.1575f),
        glm::vec3(0.54f, 0.89f, 0.63f),
        glm::vec3(0.316228f, 0.316228f, 0.316228f),
        12.8f
    );
}

// =============================================================================
// Preset Materials - Car Specific
// =============================================================================

Material Material::CarPaintRed() {
    return Material(
        glm::vec3(0.15f, 0.02f, 0.02f),
        glm::vec3(0.8f, 0.1f, 0.1f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        64.0f
    );
}

Material Material::CarPaintBlue() {
    return Material(
        glm::vec3(0.02f, 0.02f, 0.15f),
        glm::vec3(0.1f, 0.2f, 0.8f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        64.0f
    );
}

Material Material::CarPaintBlack() {
    return Material(
        glm::vec3(0.02f, 0.02f, 0.02f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        128.0f
    );
}

Material Material::CarPaintWhite() {
    return Material(
        glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3(0.95f, 0.95f, 0.95f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        64.0f
    );
}

Material Material::CarPaintSilver() {
    return Material(
        glm::vec3(0.15f, 0.15f, 0.15f),
        glm::vec3(0.6f, 0.6f, 0.65f),
        glm::vec3(0.95f, 0.95f, 0.95f),
        96.0f
    );
}

Material Material::Glass() {
    Material mat(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.1f, 0.1f, 0.12f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        128.0f
    );
    mat.opacity = 0.3f;
    return mat;
}

Material Material::GlassTinted() {
    Material mat(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.05f, 0.05f, 0.08f),
        glm::vec3(0.8f, 0.8f, 0.8f),
        128.0f
    );
    mat.opacity = 0.4f;
    return mat;
}

Material Material::Rubber() {
    return Material(
        glm::vec3(0.02f, 0.02f, 0.02f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        4.0f
    );
}

Material Material::Leather() {
    return Material(
        glm::vec3(0.1f, 0.05f, 0.02f),
        glm::vec3(0.3f, 0.15f, 0.05f),
        glm::vec3(0.2f, 0.2f, 0.2f),
        16.0f
    );
}

Material Material::DashboardPlastic() {
    return Material(
        glm::vec3(0.02f, 0.02f, 0.02f),
        glm::vec3(0.15f, 0.15f, 0.18f),
        glm::vec3(0.3f, 0.3f, 0.3f),
        16.0f
    );
}

Material Material::HeadlightGlass() {
    Material mat(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.05f, 0.05f, 0.05f),
        glm::vec3(0.95f, 0.95f, 0.95f),
        256.0f
    );
    mat.opacity = 0.2f;
    return mat;
}

// =============================================================================
// Preset Materials - Environment
// =============================================================================

Material Material::Concrete() {
    return Material(
        glm::vec3(0.1f, 0.1f, 0.1f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        4.0f
    );
}

Material Material::Tile() {
    return Material(
        glm::vec3(0.15f, 0.15f, 0.15f),
        glm::vec3(0.7f, 0.7f, 0.7f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        32.0f
    );
}

Material Material::Metal() {
    return Material(
        glm::vec3(0.1f, 0.1f, 0.1f),
        glm::vec3(0.4f, 0.4f, 0.45f),
        glm::vec3(0.8f, 0.8f, 0.8f),
        64.0f
    );
}

Material Material::Wood() {
    return Material(
        glm::vec3(0.1f, 0.05f, 0.02f),
        glm::vec3(0.4f, 0.25f, 0.1f),
        glm::vec3(0.2f, 0.2f, 0.2f),
        16.0f
    );
}
