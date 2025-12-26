/**
 * =============================================================================
 * Camera.h - 3D Camera System
 * =============================================================================
 * Implements multiple camera modes for the car showroom:
 * 
 * 1. FREE_ROAM: FPS-style camera with WASD movement and mouse look
 * 2. ORBIT: Camera orbits around a target point (the car)
 * 3. DRIVER_SEAT: Fixed camera inside the car with limited look-around
 * 
 * Camera Math Explanation:
 * ------------------------
 * The view matrix transforms world coordinates to camera (eye) coordinates.
 * It's computed as: V = R * T, where:
 * - T = translation to move world origin to camera position (negative position)
 * - R = rotation to align camera's forward direction with -Z axis
 * 
 * GLM's lookAt function computes this matrix from:
 * - Camera position (eye)
 * - Target point (center)  
 * - Up vector (usually world Y-axis)
 * 
 * For orbit camera, we use spherical coordinates:
 * - radius: distance from target
 * - yaw (azimuth): horizontal angle around target
 * - pitch (polar): vertical angle from horizontal plane
 * 
 * Position = target + radius * (cos(pitch)*cos(yaw), sin(pitch), cos(pitch)*sin(yaw))
 * =============================================================================
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * Camera operating modes.
 */
enum class CameraMode {
    FREE_ROAM,      // FPS-style free movement
    ORBIT,          // Orbit around a target
    DRIVER_SEAT     // Inside the car, limited movement
};

/**
 * Camera class - Handles view and projection matrices.
 */
class Camera {
public:
    /**
     * Create a camera with default settings.
     * Default position: (0, 2, 5) looking at origin
     */
    Camera();
    
    /**
     * Create a camera at a specific position.
     * 
     * @param position Initial camera position in world space
     * @param up World up vector (usually Y-axis)
     * @param yaw Initial yaw angle in degrees (0 = looking along +Z)
     * @param pitch Initial pitch angle in degrees
     */
    Camera(const glm::vec3& position, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f);
    
    // =========================================================================
    // Matrix Getters
    // =========================================================================
    
    /**
     * Get the view matrix.
     * Transforms world coordinates to camera/eye coordinates.
     */
    glm::mat4 getViewMatrix() const;
    
    /**
     * Get the projection matrix.
     * Transforms camera coordinates to clip coordinates (NDC after division).
     * 
     * @param aspectRatio Width/Height of the viewport
     */
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    
    // =========================================================================
    // Camera Mode
    // =========================================================================
    
    /**
     * Set the camera operating mode.
     */
    void setMode(CameraMode mode);
    CameraMode getMode() const { return m_mode; }
    
    /**
     * Set the target for orbit mode.
     */
    void setOrbitTarget(const glm::vec3& target);
    
    // =========================================================================
    // Input Processing
    // =========================================================================
    
    /**
     * Process keyboard input for movement.
     * 
     * @param forward Move forward/backward
     * @param right Move right/left
     * @param up Move up/down
     * @param deltaTime Time since last frame (for consistent speed)
     */
    void processKeyboard(float forward, float right, float up, float deltaTime);
    
    /**
     * Process mouse movement for looking around.
     * 
     * @param xoffset Mouse movement in X (positive = right)
     * @param yoffset Mouse movement in Y (positive = up)
     * @param constrainPitch Whether to limit pitch to avoid gimbal lock
     */
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    
    /**
     * Process mouse scroll for zoom (changes FOV or orbit radius).
     * 
     * @param yoffset Scroll amount (positive = zoom in)
     */
    void processMouseScroll(float yoffset);
    
    // =========================================================================
    // Getters and Setters
    // =========================================================================
    
    glm::vec3 getPosition() const { return m_position; }
    void setPosition(const glm::vec3& position) { m_position = position; }
    
    glm::vec3 getFront() const { return m_front; }
    glm::vec3 getRight() const { return m_right; }
    glm::vec3 getUp() const { return m_up; }
    
    float getYaw() const { return m_yaw; }
    float getPitch() const { return m_pitch; }
    
    void setYaw(float yaw);
    void setPitch(float pitch);
    
    float getFOV() const { return m_fov; }
    void setFOV(float fov);
    
    float getNearPlane() const { return m_nearPlane; }
    float getFarPlane() const { return m_farPlane; }
    void setClipPlanes(float near, float far);
    
    float getMovementSpeed() const { return m_movementSpeed; }
    void setMovementSpeed(float speed) { m_movementSpeed = speed; }
    
    float getMouseSensitivity() const { return m_mouseSensitivity; }
    void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
    
    // Orbit mode specific
    float getOrbitRadius() const { return m_orbitRadius; }
    void setOrbitRadius(float radius);
    
private:
    // Camera vectors
    glm::vec3 m_position;       // Camera position in world space
    glm::vec3 m_front;          // Direction camera is looking
    glm::vec3 m_up;             // Camera's up vector
    glm::vec3 m_right;          // Camera's right vector
    glm::vec3 m_worldUp;        // World up vector (for recalculating camera axes)
    
    // Euler angles (in degrees)
    float m_yaw;                // Rotation around Y axis
    float m_pitch;              // Rotation around X axis
    
    // Camera options
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_fov;                // Field of view in degrees
    float m_nearPlane;
    float m_farPlane;
    
    // Camera mode
    CameraMode m_mode;
    
    // Orbit mode parameters
    glm::vec3 m_orbitTarget;    // Point to orbit around
    float m_orbitRadius;        // Distance from target
    float m_orbitYaw;           // Horizontal angle around target
    float m_orbitPitch;         // Vertical angle
    
    // Driver seat mode parameters
    glm::vec3 m_driverSeatPosition;
    float m_driverSeatYawLimit;
    float m_driverSeatPitchLimit;
    
    /**
     * Recalculate camera vectors from Euler angles.
     * Called after any angle change to update front, right, and up vectors.
     */
    void updateCameraVectors();
    
    /**
     * Update camera position for orbit mode.
     */
    void updateOrbitPosition();
};

#endif // CAMERA_H
