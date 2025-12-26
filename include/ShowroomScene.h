/**
 * =============================================================================
 * ShowroomScene.h - Scene Management
 * =============================================================================
 * The ShowroomScene class manages all objects in the car showroom:
 * - The main featured car (with interior)
 * - Background/placeholder cars
 * - Showroom environment (floor, walls, ceiling)
 * - Lighting setup
 * - Collision boundaries
 * 
 * Scene Layout:
 * - Central platform with the main car
 * - Surrounding display area with other cars
 * - Glass walls with exterior view (simulated)
 * - Multiple light sources for dramatic effect
 * 
 * Design Decision: The scene owns all models and manages their lifetimes.
 * It provides access to objects for the renderer and input system without
 * exposing internal implementation details.
 * =============================================================================
 */

#ifndef SHOWROOM_SCENE_H
#define SHOWROOM_SCENE_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Light.h"
#include "Collision.h"

class Model;
class CarModel;
class Mesh;
class Shader;
class Camera;
class Renderer;

/**
 * ShowroomScene class - Contains and manages all scene objects.
 */
class ShowroomScene {
public:
    /**
     * Create and initialize the showroom scene.
     */
    ShowroomScene();
    
    /**
     * Destructor.
     */
    ~ShowroomScene();
    
    // Disable copying
    ShowroomScene(const ShowroomScene&) = delete;
    ShowroomScene& operator=(const ShowroomScene&) = delete;
    
    // =========================================================================
    // Scene Update
    // =========================================================================
    
    /**
     * Update all animated objects in the scene.
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime);
    
    // =========================================================================
    // Rendering
    // =========================================================================
    
    /**
     * Submit all scene objects to the renderer.
     */
    void render(Renderer& renderer) const;
    
    /**
     * Draw all scene objects with a specific shader.
     * Handles proper ordering for transparency.
     */
    void draw(Shader& shader) const;
    
    // =========================================================================
    // Object Access
    // =========================================================================
    
    /**
     * Get the main featured car.
     */
    CarModel* getMainCar() { return m_mainCar.get(); }
    const CarModel* getMainCar() const { return m_mainCar.get(); }
    
    /**
     * Get background cars.
     */
    const std::vector<std::unique_ptr<CarModel>>& getBackgroundCars() const {
        return m_backgroundCars;
    }
    
    /**
     * Get environment models.
     */
    const std::vector<std::unique_ptr<Model>>& getEnvironment() const {
        return m_environment;
    }
    
    // =========================================================================
    // Lighting
    // =========================================================================
    
    /**
     * Get the directional light (main sun/sky light).
     */
    const DirectionalLight& getDirectionalLight() const { return m_sunLight; }
    DirectionalLight& getDirectionalLight() { return m_sunLight; }
    
    /**
     * Get point lights.
     */
    const std::vector<PointLight>& getPointLights() const { return m_pointLights; }
    
    /**
     * Get spot lights.
     */
    const std::vector<SpotLight>& getSpotLights() const { return m_spotLights; }
    
    /**
     * Apply all lights to shader.
     */
    void applyLighting(Shader& shader) const;
    
    // =========================================================================
    // Collision
    // =========================================================================
    
    /**
     * Get the collision world for physics/movement.
     */
    CollisionWorld& getCollisionWorld() { return m_collisionWorld; }
    const CollisionWorld& getCollisionWorld() const { return m_collisionWorld; }
    
    /**
     * Check if a position is valid (not colliding with walls).
     */
    bool isPositionValid(const glm::vec3& position, const glm::vec3& size) const;
    
    /**
     * Constrain a position to valid showroom bounds.
     */
    glm::vec3 constrainPosition(const glm::vec3& position, const glm::vec3& size) const;
    
    // =========================================================================
    // Scene Configuration
    // =========================================================================
    
    /**
     * Get showroom dimensions.
     */
    glm::vec3 getShowroomSize() const { return m_showroomSize; }
    
    /**
     * Get the center of the showroom floor.
     */
    glm::vec3 getShowroomCenter() const { return glm::vec3(0.0f, 0.0f, 0.0f); }
    
    /**
     * Toggle showroom lights on/off.
     */
    void setLightsEnabled(bool enabled);
    
private:
    // Main featured car
    std::unique_ptr<CarModel> m_mainCar;
    
    // Background/placeholder cars
    std::vector<std::unique_ptr<CarModel>> m_backgroundCars;
    
    // Environment (floor, walls, ceiling, decorations)
    std::vector<std::unique_ptr<Model>> m_environment;
    
    // Lighting
    DirectionalLight m_sunLight;
    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;
    
    // Collision
    CollisionWorld m_collisionWorld;
    
    // Scene dimensions
    glm::vec3 m_showroomSize;
    
    /**
     * Create the showroom environment (floor, walls, etc.)
     */
    void createEnvironment();
    
    /**
     * Create the main featured car.
     */
    void createMainCar();
    
    /**
     * Create background cars.
     */
    void createBackgroundCars();
    
    /**
     * Set up the lighting.
     */
    void setupLighting();
    
    /**
     * Set up collision boundaries.
     */
    void setupCollision();
};

#endif // SHOWROOM_SCENE_H
