/**
 * =============================================================================
 * CarModel.h - Detailed Car Model with Animations
 * =============================================================================
 * Extends the Model class to represent a car with:
 * - Separate meshes for body, wheels, doors, windows
 * - Animated components (wheel rotation, door opening)
 * - Interior details for driver-seat camera view
 * 
 * Car Coordinate System:
 * - X axis: Left to right (positive right)
 * - Y axis: Down to up (positive up)
 * - Z axis: Back to front (positive forward)
 * - Origin: Center of the car at ground level
 * =============================================================================
 */

#ifndef CAR_MODEL_H
#define CAR_MODEL_H

#include "Model.h"
#include <array>

class Shader;

/**
 * Wheel positions for the car.
 */
enum class WheelPosition {
    FRONT_LEFT = 0,
    FRONT_RIGHT = 1,
    REAR_LEFT = 2,
    REAR_RIGHT = 3
};

/**
 * Door positions.
 */
enum class DoorPosition {
    FRONT_LEFT = 0,
    FRONT_RIGHT = 1,
    REAR_LEFT = 2,
    REAR_RIGHT = 3
};

/**
 * CarModel class - Represents a detailed car with animations.
 */
class CarModel : public Model {
public:
    /**
     * Create a car model with default geometry.
     */
    CarModel();
    
    /**
     * Create a simplified placeholder car (for background cars).
     * @param simplified If true, creates a low-detail version
     */
    explicit CarModel(bool simplified);
    
    /**
     * Destructor.
     */
    ~CarModel() override;
    
    // =========================================================================
    // Animation Controls
    // =========================================================================
    
    /**
     * Update animations based on elapsed time.
     * @param deltaTime Time since last frame in seconds
     */
    void update(float deltaTime);
    
    /**
     * Set wheel rotation speed (simulates movement).
     * @param speed Rotation speed in degrees per second
     */
    void setWheelSpeed(float speed);
    
    /**
     * Get current wheel rotation angle.
     */
    float getWheelRotation() const { return m_wheelRotation; }
    
    /**
     * Open or close a door.
     * @param door Which door to animate
     * @param open True to open, false to close
     */
    void setDoorOpen(DoorPosition door, bool open);
    
    /**
     * Get door open angle (0 = closed, 1 = fully open).
     */
    float getDoorOpenAmount(DoorPosition door) const;
    
    /**
     * Turn headlights on/off.
     */
    void setHeadlightsOn(bool on);
    bool areHeadlightsOn() const { return m_headlightsOn; }
    
    // =========================================================================
    // Movement
    // =========================================================================
    
    /**
     * Move the car forward/backward.
     * @param amount Distance to move (positive = forward)
     * @param deltaTime Time since last frame
     */
    void move(float amount, float deltaTime);
    
    /**
     * Turn the car.
     * @param angle Turning angle in degrees (positive = right)
     * @param deltaTime Time since last frame
     */
    void turn(float angle, float deltaTime);
    
    /**
     * Get current speed for wheel animation.
     */
    float getCurrentSpeed() const { return m_currentSpeed; }
    
    // =========================================================================
    // Camera Positions
    // =========================================================================
    
    /**
     * Get position for orbit camera target.
     */
    glm::vec3 getOrbitTarget() const;
    
    /**
     * Get driver seat camera position.
     */
    glm::vec3 getDriverSeatPosition() const;
    
    /**
     * Get recommended orbit distance.
     */
    float getOrbitDistance() const { return 5.0f; }
    
    // =========================================================================
    // Rendering
    // =========================================================================
    
    /**
     * Draw the car with proper material handling for transparent parts.
     */
    void draw(Shader& shader) const override;
    
    /**
     * Draw only opaque parts (for first pass).
     */
    void drawOpaque(Shader& shader) const;
    
    /**
     * Draw only transparent parts (windows, for second pass).
     */
    void drawTransparent(Shader& shader) const;
    
    // =========================================================================
    // Collision
    // =========================================================================
    
    /**
     * Get bounding box for collision detection.
     * @param min Output: minimum corner
     * @param max Output: maximum corner
     */
    void getBoundingBox(glm::vec3& min, glm::vec3& max) const;
    
private:
    // Sub-meshes (owned by parent Model::m_meshes)
    // We store indices into the meshes vector for identification
    size_t m_bodyMeshIndex;
    std::array<size_t, 4> m_wheelMeshIndices;
    std::array<size_t, 4> m_doorMeshIndices;
    size_t m_windowMeshIndex;
    size_t m_interiorMeshIndex;
    
    // Animation state
    float m_wheelRotation;          // Current wheel rotation in degrees
    float m_wheelSpeed;             // Rotation speed in degrees/second
    std::array<float, 4> m_doorOpenAmount;  // 0.0 = closed, 1.0 = open
    std::array<bool, 4> m_doorTargetOpen;   // Target state for animation
    float m_doorAnimSpeed;          // Door animation speed
    
    // Movement state
    float m_currentSpeed;           // Current movement speed
    float m_heading;                // Current heading angle in degrees
    
    // Features
    bool m_headlightsOn;
    bool m_hasInterior;             // False for simplified cars
    
    // Car dimensions (for bounding box and camera positioning)
    float m_length;
    float m_width;
    float m_height;
    float m_wheelRadius;
    
    /**
     * Create the detailed car geometry.
     */
    void createDetailedCar();
    
    /**
     * Create simplified car geometry.
     */
    void createSimplifiedCar();
    
    /**
     * Create a wheel mesh at the given position.
     */
    std::unique_ptr<Mesh> createWheelMesh(WheelPosition pos);
    
    /**
     * Create the car body mesh.
     */
    std::unique_ptr<Mesh> createBodyMesh();
    
    /**
     * Create the car interior mesh.
     */
    std::unique_ptr<Mesh> createInteriorMesh();
    
    /**
     * Create window meshes.
     */
    std::unique_ptr<Mesh> createWindowMesh();
    
    /**
     * Update wheel transforms based on rotation.
     */
    void updateWheelTransforms();
    
    /**
     * Update door transforms based on open amount.
     */
    void updateDoorTransforms();
};

#endif // CAR_MODEL_H
