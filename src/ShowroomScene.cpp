/**
 * =============================================================================
 * ShowroomScene.cpp - Scene Management Implementation
 * =============================================================================
 */

#include "ShowroomScene.h"
#include "Model.h"
#include "CarModel.h"
#include "Mesh.h"
#include "Shader.h"
#include "Renderer.h"
#include "Material.h"

// =============================================================================
// Constructor / Destructor
// =============================================================================

ShowroomScene::ShowroomScene()
    : m_showroomSize(30.0f, 10.0f, 20.0f)
{
    createEnvironment();
    createMainCar();
    createBackgroundCars();
    setupLighting();
    setupCollision();
}

ShowroomScene::~ShowroomScene() = default;

// =============================================================================
// Update
// =============================================================================

void ShowroomScene::update(float deltaTime) {
    // Update main car animations
    if (m_mainCar) {
        m_mainCar->update(deltaTime);
    }
    
    // Update background cars (optional idle animations)
    for (auto& car : m_backgroundCars) {
        car->update(deltaTime);
    }
}

// =============================================================================
// Rendering
// =============================================================================

void ShowroomScene::render(Renderer& renderer) const {
    // Submit environment
    for (const auto& env : m_environment) {
        renderer.submit(*env);
    }
    
    // Submit main car
    if (m_mainCar) {
        renderer.submit(*m_mainCar);
    }
    
    // Submit background cars
    for (const auto& car : m_backgroundCars) {
        renderer.submit(*car);
    }
}

void ShowroomScene::draw(Shader& shader) const {
    // Draw environment (opaque)
    for (const auto& env : m_environment) {
        env->draw(shader);
    }
    
    // Draw main car (opaque parts)
    if (m_mainCar) {
        m_mainCar->drawOpaque(shader);
    }
    
    // Draw background cars
    for (const auto& car : m_backgroundCars) {
        car->drawOpaque(shader);
    }
    
    // Draw transparent parts last
    if (m_mainCar) {
        m_mainCar->drawTransparent(shader);
    }
    
    for (const auto& car : m_backgroundCars) {
        car->drawTransparent(shader);
    }
}

// =============================================================================
// Lighting
// =============================================================================

void ShowroomScene::applyLighting(Shader& shader) const {
    // Apply directional light
    m_sunLight.applyToShader(shader, "dirLight");
    
    // Apply point lights
    shader.setInt("numPointLights", static_cast<int>(m_pointLights.size()));
    for (size_t i = 0; i < m_pointLights.size(); i++) {
        std::string name = "pointLights[" + std::to_string(i) + "]";
        m_pointLights[i].applyToShader(shader, name);
    }
    
    // Apply spot lights
    shader.setInt("numSpotLights", static_cast<int>(m_spotLights.size()));
    for (size_t i = 0; i < m_spotLights.size(); i++) {
        std::string name = "spotLights[" + std::to_string(i) + "]";
        m_spotLights[i].applyToShader(shader, name);
    }
}

void ShowroomScene::setLightsEnabled(bool enabled) {
    m_sunLight.enabled = enabled;
    for (auto& light : m_pointLights) {
        light.enabled = enabled;
    }
    for (auto& light : m_spotLights) {
        light.enabled = enabled;
    }
}

// =============================================================================
// Collision
// =============================================================================

bool ShowroomScene::isPositionValid(const glm::vec3& position, const glm::vec3& size) const {
    AABB testBox;
    testBox.min = position - size * 0.5f;
    testBox.max = position + size * 0.5f;
    
    CollisionResult result = m_collisionWorld.testAgainstStatic(testBox);
    return !result.hit;
}

glm::vec3 ShowroomScene::constrainPosition(const glm::vec3& position, 
                                            const glm::vec3& size) const {
    AABB testBox;
    testBox.min = position - size * 0.5f;
    testBox.max = position + size * 0.5f;
    
    return m_collisionWorld.resolveCollisions(testBox, position);
}

// =============================================================================
// Private: Create Environment
// =============================================================================

void ShowroomScene::createEnvironment() {
    // Floor
    auto floor = std::make_unique<Model>("Floor");
    floor->addMesh(std::make_unique<Mesh>(
        MeshGenerator::createPlane(m_showroomSize.x, m_showroomSize.z, 5.0f, 5.0f)),
        Material::Tile());
    floor->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    m_environment.push_back(std::move(floor));
    
    // Ceiling
    auto ceiling = std::make_unique<Model>("Ceiling");
    ceiling->addMesh(std::make_unique<Mesh>(
        MeshGenerator::createPlane(m_showroomSize.x, m_showroomSize.z, 3.0f, 3.0f)),
        Material::Concrete());
    ceiling->setPosition(glm::vec3(0.0f, m_showroomSize.y, 0.0f));
    ceiling->setRotation(glm::vec3(180.0f, 0.0f, 0.0f));  // Flip upside down
    m_environment.push_back(std::move(ceiling));
    
    // Walls
    float wallHeight = m_showroomSize.y;
    float halfWidth = m_showroomSize.x / 2.0f;
    float halfDepth = m_showroomSize.z / 2.0f;
    
    // Back wall
    auto backWall = std::make_unique<Model>("BackWall");
    backWall->addMesh(std::make_unique<Mesh>(
        MeshGenerator::createPlane(m_showroomSize.x, wallHeight, 2.0f, 1.0f)),
        Material::Concrete());
    backWall->setPosition(glm::vec3(0.0f, wallHeight / 2.0f, -halfDepth));
    backWall->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    m_environment.push_back(std::move(backWall));
    
    // Front wall (with opening simulation)
    auto frontWall = std::make_unique<Model>("FrontWall");
    frontWall->addMesh(std::make_unique<Mesh>(
        MeshGenerator::createPlane(m_showroomSize.x, wallHeight, 2.0f, 1.0f)),
        Material::Concrete());
    frontWall->setPosition(glm::vec3(0.0f, wallHeight / 2.0f, halfDepth));
    frontWall->setRotation(glm::vec3(90.0f, 0.0f, 0.0f));
    m_environment.push_back(std::move(frontWall));
    
    // Left wall
    auto leftWall = std::make_unique<Model>("LeftWall");
    leftWall->addMesh(std::make_unique<Mesh>(
        MeshGenerator::createPlane(m_showroomSize.z, wallHeight, 2.0f, 1.0f)),
        Material::Concrete());
    leftWall->setPosition(glm::vec3(-halfWidth, wallHeight / 2.0f, 0.0f));
    leftWall->setRotation(glm::vec3(-90.0f, 0.0f, 90.0f));
    m_environment.push_back(std::move(leftWall));
    
    // Right wall
    auto rightWall = std::make_unique<Model>("RightWall");
    rightWall->addMesh(std::make_unique<Mesh>(
        MeshGenerator::createPlane(m_showroomSize.z, wallHeight, 2.0f, 1.0f)),
        Material::Concrete());
    rightWall->setPosition(glm::vec3(halfWidth, wallHeight / 2.0f, 0.0f));
    rightWall->setRotation(glm::vec3(-90.0f, 0.0f, -90.0f));
    m_environment.push_back(std::move(rightWall));
    
    // Display platform for main car
    auto platform = std::make_unique<Model>("Platform");
    platform->addMesh(std::make_unique<Mesh>(
        MeshGenerator::createCylinder(3.0f, 0.2f, 48)),
        Material::Metal());
    platform->setPosition(glm::vec3(0.0f, 0.1f, 0.0f));
    m_environment.push_back(std::move(platform));
}

// =============================================================================
// Private: Create Cars
// =============================================================================

void ShowroomScene::createMainCar() {
    m_mainCar = std::make_unique<CarModel>();
    m_mainCar->setPosition(glm::vec3(0.0f, 0.2f, 0.0f));  // On platform
}

void ShowroomScene::createBackgroundCars() {
    // Create some placeholder cars around the showroom
    struct CarPlacement {
        glm::vec3 position;
        float rotation;
        Material paint;
    };
    
    std::vector<CarPlacement> placements = {
        {{-8.0f, 0.0f, -5.0f}, 30.0f, Material::CarPaintBlue()},
        {{8.0f, 0.0f, -5.0f}, -30.0f, Material::CarPaintWhite()},
        {{-8.0f, 0.0f, 5.0f}, -45.0f, Material::CarPaintSilver()},
        {{8.0f, 0.0f, 5.0f}, 45.0f, Material::CarPaintBlack()}
    };
    
    for (const auto& placement : placements) {
        auto car = std::make_unique<CarModel>(true);  // Simplified version
        car->setPosition(placement.position);
        car->setRotation(glm::vec3(0.0f, placement.rotation, 0.0f));
        car->setMaterial(placement.paint);
        m_backgroundCars.push_back(std::move(car));
    }
}

// =============================================================================
// Private: Setup Lighting
// =============================================================================

void ShowroomScene::setupLighting() {
    // Main directional light (simulated skylight)
    m_sunLight = DirectionalLight(
        glm::vec3(-0.3f, -1.0f, -0.2f),  // Direction
        glm::vec3(0.2f, 0.2f, 0.25f),    // Ambient
        glm::vec3(0.6f, 0.6f, 0.7f),     // Diffuse
        glm::vec3(0.8f, 0.8f, 0.9f)      // Specular
    );
    
    // Ceiling lights (point lights)
    std::vector<glm::vec3> lightPositions = {
        {-5.0f, 8.0f, -5.0f},
        {5.0f, 8.0f, -5.0f},
        {-5.0f, 8.0f, 5.0f},
        {5.0f, 8.0f, 5.0f}
    };
    
    for (const auto& pos : lightPositions) {
        PointLight light(
            pos,
            glm::vec3(0.1f),              // Ambient
            glm::vec3(0.8f, 0.8f, 0.75f), // Diffuse
            glm::vec3(1.0f)               // Specular
        );
        light.setRange(15.0f);
        m_pointLights.push_back(light);
    }
    
    // Spotlight on the main car
    SpotLight carSpotlight(
        glm::vec3(0.0f, 8.0f, 0.0f),      // Position
        glm::vec3(0.0f, -1.0f, 0.0f),     // Direction (straight down)
        glm::vec3(0.0f),                   // Ambient
        glm::vec3(1.0f, 1.0f, 0.95f),     // Diffuse
        glm::vec3(1.0f),                   // Specular
        15.0f,                             // Inner cutoff
        25.0f                              // Outer cutoff
    );
    m_spotLights.push_back(carSpotlight);
}

// =============================================================================
// Private: Setup Collision
// =============================================================================

void ShowroomScene::setupCollision() {
    float halfWidth = m_showroomSize.x / 2.0f;
    float halfDepth = m_showroomSize.z / 2.0f;
    float wallThickness = 0.5f;
    
    // Back wall
    m_collisionWorld.addStaticAABB(AABB(
        glm::vec3(-halfWidth, 0.0f, -halfDepth - wallThickness),
        glm::vec3(halfWidth, m_showroomSize.y, -halfDepth)
    ));
    
    // Front wall
    m_collisionWorld.addStaticAABB(AABB(
        glm::vec3(-halfWidth, 0.0f, halfDepth),
        glm::vec3(halfWidth, m_showroomSize.y, halfDepth + wallThickness)
    ));
    
    // Left wall
    m_collisionWorld.addStaticAABB(AABB(
        glm::vec3(-halfWidth - wallThickness, 0.0f, -halfDepth),
        glm::vec3(-halfWidth, m_showroomSize.y, halfDepth)
    ));
    
    // Right wall
    m_collisionWorld.addStaticAABB(AABB(
        glm::vec3(halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth + wallThickness, m_showroomSize.y, halfDepth)
    ));
}
