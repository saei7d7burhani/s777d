/**
 * =============================================================================
 * Model.cpp - 3D Model Container Implementation
 * =============================================================================
 */

#include "Model.h"
#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp>

// =============================================================================
// Constructors / Destructor
// =============================================================================

Model::Model()
    : m_name("Unnamed")
    , m_position(0.0f)
    , m_rotation(0.0f)
    , m_scale(1.0f)
    , m_visible(true)
    , m_modelMatrix(1.0f)
    , m_modelMatrixDirty(true)
{
}

Model::Model(const std::string& name)
    : m_name(name)
    , m_position(0.0f)
    , m_rotation(0.0f)
    , m_scale(1.0f)
    , m_visible(true)
    , m_modelMatrix(1.0f)
    , m_modelMatrixDirty(true)
{
}

Model::~Model() = default;

// Move constructor
Model::Model(Model&& other) noexcept
    : m_name(std::move(other.m_name))
    , m_meshes(std::move(other.m_meshes))
    , m_meshMaterials(std::move(other.m_meshMaterials))
    , m_material(std::move(other.m_material))
    , m_position(other.m_position)
    , m_rotation(other.m_rotation)
    , m_scale(other.m_scale)
    , m_visible(other.m_visible)
    , m_modelMatrix(other.m_modelMatrix)
    , m_modelMatrixDirty(other.m_modelMatrixDirty)
{
}

// Move assignment
Model& Model::operator=(Model&& other) noexcept {
    if (this != &other) {
        m_name = std::move(other.m_name);
        m_meshes = std::move(other.m_meshes);
        m_meshMaterials = std::move(other.m_meshMaterials);
        m_material = std::move(other.m_material);
        m_position = other.m_position;
        m_rotation = other.m_rotation;
        m_scale = other.m_scale;
        m_visible = other.m_visible;
        m_modelMatrix = other.m_modelMatrix;
        m_modelMatrixDirty = other.m_modelMatrixDirty;
    }
    return *this;
}

// =============================================================================
// Mesh Management
// =============================================================================

void Model::addMesh(std::unique_ptr<Mesh> mesh) {
    m_meshes.push_back(std::move(mesh));
    m_meshMaterials.push_back(m_material);
}

void Model::addMesh(std::unique_ptr<Mesh> mesh, const Material& material) {
    m_meshes.push_back(std::move(mesh));
    m_meshMaterials.push_back(material);
}

Mesh* Model::getMesh(size_t index) {
    if (index < m_meshes.size()) {
        return m_meshes[index].get();
    }
    return nullptr;
}

const Mesh* Model::getMesh(size_t index) const {
    if (index < m_meshes.size()) {
        return m_meshes[index].get();
    }
    return nullptr;
}

// =============================================================================
// Transform Operations
// =============================================================================

void Model::setPosition(const glm::vec3& position) {
    m_position = position;
    m_modelMatrixDirty = true;
}

void Model::setRotation(const glm::vec3& rotation) {
    m_rotation = rotation;
    m_modelMatrixDirty = true;
}

void Model::setScale(const glm::vec3& scale) {
    m_scale = scale;
    m_modelMatrixDirty = true;
}

void Model::setScale(float uniformScale) {
    m_scale = glm::vec3(uniformScale);
    m_modelMatrixDirty = true;
}

glm::mat4 Model::getModelMatrix() const {
    if (m_modelMatrixDirty) {
        updateModelMatrix();
    }
    return m_modelMatrix;
}

void Model::updateModelMatrix() const {
    // Build model matrix: Translation * RotationZ * RotationY * RotationX * Scale
    // Order matters! Transformations are applied right to left.
    
    m_modelMatrix = glm::mat4(1.0f);
    
    // Translation
    m_modelMatrix = glm::translate(m_modelMatrix, m_position);
    
    // Rotation (ZYX order for typical Euler angles)
    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
    
    // Scale
    m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
    
    m_modelMatrixDirty = false;
}

// =============================================================================
// Rendering
// =============================================================================

void Model::draw(Shader& shader) const {
    if (!m_visible) return;
    
    draw(shader, glm::mat4(1.0f));
}

void Model::draw(Shader& shader, const glm::mat4& parentTransform) const {
    if (!m_visible) return;
    
    glm::mat4 modelMatrix = parentTransform * getModelMatrix();
    shader.setMat4("model", modelMatrix);
    
    // Calculate normal matrix for lighting
    // Normal matrix = transpose(inverse(model matrix))
    // This correctly transforms normals when the model has non-uniform scaling
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    shader.setMat3("normalMatrix", normalMatrix);
    
    // Draw each mesh
    for (size_t i = 0; i < m_meshes.size(); i++) {
        // Apply material for this mesh
        if (i < m_meshMaterials.size()) {
            m_meshMaterials[i].applyToShader(shader);
        } else {
            m_material.applyToShader(shader);
        }
        
        m_meshes[i]->draw(shader);
    }
}

// =============================================================================
// Material
// =============================================================================

void Model::setMaterial(const Material& material) {
    m_material = material;
    // Update all mesh materials
    for (auto& mat : m_meshMaterials) {
        mat = material;
    }
}
