/**
 * =============================================================================
 * CarModel.cpp - Detailed Car Model Implementation
 * =============================================================================
 */

#include "CarModel.h"
#include "Shader.h"
#include "Mesh.h"

#include <cmath>

// =============================================================================
// Constructors / Destructor
// =============================================================================

CarModel::CarModel()
    : Model("DetailedCar")
    , m_bodyMeshIndex(0)
    , m_windowMeshIndex(0)
    , m_interiorMeshIndex(0)
    , m_wheelRotation(0.0f)
    , m_wheelSpeed(0.0f)
    , m_doorAnimSpeed(90.0f)  // Degrees per second
    , m_currentSpeed(0.0f)
    , m_heading(0.0f)
    , m_headlightsOn(false)
    , m_hasInterior(true)
    , m_length(4.0f)
    , m_width(1.8f)
    , m_height(1.5f)
    , m_wheelRadius(0.4f)
{
    m_doorOpenAmount.fill(0.0f);
    m_doorTargetOpen.fill(false);
    m_wheelMeshIndices.fill(0);
    m_doorMeshIndices.fill(0);
    
    createDetailedCar();
}

CarModel::CarModel(bool simplified)
    : Model("SimplifiedCar")
    , m_bodyMeshIndex(0)
    , m_windowMeshIndex(0)
    , m_interiorMeshIndex(0)
    , m_wheelRotation(0.0f)
    , m_wheelSpeed(0.0f)
    , m_doorAnimSpeed(90.0f)
    , m_currentSpeed(0.0f)
    , m_heading(0.0f)
    , m_headlightsOn(false)
    , m_hasInterior(!simplified)
    , m_length(4.0f)
    , m_width(1.8f)
    , m_height(1.5f)
    , m_wheelRadius(0.4f)
{
    m_doorOpenAmount.fill(0.0f);
    m_doorTargetOpen.fill(false);
    m_wheelMeshIndices.fill(0);
    m_doorMeshIndices.fill(0);
    
    if (simplified) {
        createSimplifiedCar();
    } else {
        createDetailedCar();
    }
}

CarModel::~CarModel() = default;

// =============================================================================
// Animation
// =============================================================================

void CarModel::update(float deltaTime) {
    // Update wheel rotation based on speed
    if (std::abs(m_currentSpeed) > 0.01f) {
        // Calculate wheel rotation from speed
        // Circumference = 2 * PI * radius
        // Rotation = distance / circumference * 360 degrees
        float distance = m_currentSpeed * deltaTime;
        float circumference = 2.0f * 3.14159265359f * m_wheelRadius;
        m_wheelRotation += (distance / circumference) * 360.0f;
        
        // Keep rotation in reasonable range
        if (m_wheelRotation > 360.0f) m_wheelRotation -= 360.0f;
        if (m_wheelRotation < -360.0f) m_wheelRotation += 360.0f;
    }
    
    // Update door animations
    for (size_t i = 0; i < 4; i++) {
        float target = m_doorTargetOpen[i] ? 1.0f : 0.0f;
        float diff = target - m_doorOpenAmount[i];
        
        if (std::abs(diff) > 0.001f) {
            float change = m_doorAnimSpeed * deltaTime / 60.0f;  // Normalize to 0-1 range
            if (diff > 0) {
                m_doorOpenAmount[i] = std::min(m_doorOpenAmount[i] + change, 1.0f);
            } else {
                m_doorOpenAmount[i] = std::max(m_doorOpenAmount[i] - change, 0.0f);
            }
        }
    }
}

void CarModel::setWheelSpeed(float speed) {
    m_wheelSpeed = speed;
}

void CarModel::setDoorOpen(DoorPosition door, bool open) {
    m_doorTargetOpen[static_cast<size_t>(door)] = open;
}

float CarModel::getDoorOpenAmount(DoorPosition door) const {
    return m_doorOpenAmount[static_cast<size_t>(door)];
}

void CarModel::setHeadlightsOn(bool on) {
    m_headlightsOn = on;
}

// =============================================================================
// Movement
// =============================================================================

void CarModel::move(float amount, float deltaTime) {
    m_currentSpeed = amount;
    
    // Calculate movement direction based on heading
    float headingRad = glm::radians(m_heading);
    glm::vec3 direction(std::sin(headingRad), 0.0f, std::cos(headingRad));
    
    // Update position
    m_position += direction * amount * deltaTime;
    m_modelMatrixDirty = true;
}

void CarModel::turn(float angle, float deltaTime) {
    m_heading += angle * deltaTime;
    m_rotation.y = m_heading;
    m_modelMatrixDirty = true;
}

// =============================================================================
// Camera Positions
// =============================================================================

glm::vec3 CarModel::getOrbitTarget() const {
    // Target point is slightly above the car center
    return m_position + glm::vec3(0.0f, m_height * 0.5f, 0.0f);
}

glm::vec3 CarModel::getDriverSeatPosition() const {
    // Driver seat is on the left side, forward of center
    float headingRad = glm::radians(m_heading);
    glm::vec3 forward(std::sin(headingRad), 0.0f, std::cos(headingRad));
    glm::vec3 right(std::cos(headingRad), 0.0f, -std::sin(headingRad));
    
    return m_position 
           + glm::vec3(0.0f, 1.0f, 0.0f)  // Height
           + forward * 0.3f               // Slightly forward
           - right * 0.4f;                // Left side
}

// =============================================================================
// Rendering
// =============================================================================

void CarModel::draw(Shader& shader) const {
    drawOpaque(shader);
    drawTransparent(shader);
}

void CarModel::drawOpaque(Shader& shader) const {
    if (!m_visible) return;
    
    glm::mat4 modelMatrix = getModelMatrix();
    
    // Draw body
    if (m_bodyMeshIndex < m_meshes.size()) {
        shader.setMat4("model", modelMatrix);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        shader.setMat3("normalMatrix", normalMatrix);
        
        if (m_bodyMeshIndex < m_meshMaterials.size()) {
            m_meshMaterials[m_bodyMeshIndex].applyToShader(shader);
        }
        m_meshes[m_bodyMeshIndex]->draw(shader);
    }
    
    // Draw wheels with rotation
    for (size_t i = 0; i < 4; i++) {
        if (m_wheelMeshIndices[i] < m_meshes.size()) {
            // Calculate wheel position
            float xOffset = (i < 2) ? m_length * 0.35f : -m_length * 0.35f;  // Front/rear
            float zOffset = (i % 2 == 0) ? -m_width * 0.5f : m_width * 0.5f;  // Left/right
            
            glm::mat4 wheelMatrix = modelMatrix;
            wheelMatrix = glm::translate(wheelMatrix, glm::vec3(xOffset, m_wheelRadius, zOffset));
            
            // Rotate wheel on its axis
            wheelMatrix = glm::rotate(wheelMatrix, glm::radians(m_wheelRotation), glm::vec3(0, 0, 1));
            
            // Rotate wheel to face sideways
            if (i % 2 == 0) {
                wheelMatrix = glm::rotate(wheelMatrix, glm::radians(90.0f), glm::vec3(0, 0, 1));
            } else {
                wheelMatrix = glm::rotate(wheelMatrix, glm::radians(-90.0f), glm::vec3(0, 0, 1));
            }
            
            shader.setMat4("model", wheelMatrix);
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(wheelMatrix)));
            shader.setMat3("normalMatrix", normalMatrix);
            
            if (m_wheelMeshIndices[i] < m_meshMaterials.size()) {
                m_meshMaterials[m_wheelMeshIndices[i]].applyToShader(shader);
            }
            m_meshes[m_wheelMeshIndices[i]]->draw(shader);
        }
    }
    
    // Draw interior if present
    if (m_hasInterior && m_interiorMeshIndex < m_meshes.size()) {
        shader.setMat4("model", modelMatrix);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        shader.setMat3("normalMatrix", normalMatrix);
        
        if (m_interiorMeshIndex < m_meshMaterials.size()) {
            m_meshMaterials[m_interiorMeshIndex].applyToShader(shader);
        }
        m_meshes[m_interiorMeshIndex]->draw(shader);
    }
}

void CarModel::drawTransparent(Shader& shader) const {
    if (!m_visible) return;
    
    // Draw windows (transparent)
    if (m_windowMeshIndex < m_meshes.size()) {
        glm::mat4 modelMatrix = getModelMatrix();
        shader.setMat4("model", modelMatrix);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        shader.setMat3("normalMatrix", normalMatrix);
        
        if (m_windowMeshIndex < m_meshMaterials.size()) {
            m_meshMaterials[m_windowMeshIndex].applyToShader(shader);
        }
        m_meshes[m_windowMeshIndex]->draw(shader);
    }
}

// =============================================================================
// Collision
// =============================================================================

void CarModel::getBoundingBox(glm::vec3& min, glm::vec3& max) const {
    glm::vec3 halfSize(m_length / 2.0f, m_height / 2.0f, m_width / 2.0f);
    min = m_position - halfSize;
    max = m_position + halfSize;
    min.y = m_position.y;  // Bottom at ground level
    max.y = m_position.y + m_height;
}

// =============================================================================
// Private Methods
// =============================================================================

void CarModel::createDetailedCar() {
    // Create car body
    m_bodyMeshIndex = m_meshes.size();
    addMesh(std::make_unique<Mesh>(MeshGenerator::createCarBody()), Material::CarPaintRed());
    
    // Create wheels
    for (size_t i = 0; i < 4; i++) {
        m_wheelMeshIndices[i] = m_meshes.size();
        addMesh(std::make_unique<Mesh>(MeshGenerator::createWheel(m_wheelRadius, 0.2f)), 
                Material::Rubber());
    }
    
    // Create windows (simplified - just the windshield area)
    std::vector<Vertex> windowVerts;
    std::vector<unsigned int> windowInds;
    
    float hl = m_length / 2.0f;
    float hw = m_width / 2.0f * 0.9f;
    float bodyHeight = 0.8f;
    float cabinHeight = 0.7f;
    float cabinTop = bodyHeight + cabinHeight;
    float hoodLength = 1.2f;
    float trunkLength = 0.8f;
    float cabinFront = hl - hoodLength;
    float cabinBack = -hl + trunkLength;
    
    // Windshield
    windowVerts.push_back({{cabinFront + 0.05f, bodyHeight + 0.05f, -hw + 0.05f}, {0.7f, 0.7f, 0}, {0, 0}});
    windowVerts.push_back({{cabinFront + 0.35f, cabinTop - 0.05f, -hw + 0.05f}, {0.7f, 0.7f, 0}, {1, 0}});
    windowVerts.push_back({{cabinFront + 0.35f, cabinTop - 0.05f, hw - 0.05f}, {0.7f, 0.7f, 0}, {1, 1}});
    windowVerts.push_back({{cabinFront + 0.05f, bodyHeight + 0.05f, hw - 0.05f}, {0.7f, 0.7f, 0}, {0, 1}});
    
    windowInds = {0, 1, 2, 2, 3, 0};
    
    m_windowMeshIndex = m_meshes.size();
    addMesh(std::make_unique<Mesh>(windowVerts, windowInds), Material::Glass());
    
    // Create simple interior
    std::vector<Vertex> interiorVerts;
    std::vector<unsigned int> interiorInds;
    
    // Dashboard
    float dashY = bodyHeight + 0.1f;
    interiorVerts.push_back({{cabinFront - 0.1f, dashY, -hw + 0.1f}, {0, 1, 0}, {0, 0}});
    interiorVerts.push_back({{cabinFront + 0.2f, dashY + 0.3f, -hw + 0.1f}, {0, 1, 0}, {1, 0}});
    interiorVerts.push_back({{cabinFront + 0.2f, dashY + 0.3f, hw - 0.1f}, {0, 1, 0}, {1, 1}});
    interiorVerts.push_back({{cabinFront - 0.1f, dashY, hw - 0.1f}, {0, 1, 0}, {0, 1}});
    
    interiorInds = {0, 1, 2, 2, 3, 0};
    
    m_interiorMeshIndex = m_meshes.size();
    addMesh(std::make_unique<Mesh>(interiorVerts, interiorInds), Material::DashboardPlastic());
}

void CarModel::createSimplifiedCar() {
    // Just the body and wheels, no interior or detailed windows
    m_bodyMeshIndex = m_meshes.size();
    addMesh(std::make_unique<Mesh>(MeshGenerator::createCarBody()), Material::CarPaintBlue());
    
    for (size_t i = 0; i < 4; i++) {
        m_wheelMeshIndices[i] = m_meshes.size();
        addMesh(std::make_unique<Mesh>(MeshGenerator::createWheel(m_wheelRadius, 0.15f)), 
                Material::Rubber());
    }
    
    m_windowMeshIndex = m_meshes.size();  // No window mesh for simplified
    m_interiorMeshIndex = m_meshes.size();  // No interior mesh
}
