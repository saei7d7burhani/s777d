/**
 * =============================================================================
 * Camera.cpp - 3D Camera System Implementation
 * =============================================================================
 */

#include "Camera.h"
#include <algorithm>
#include <cmath>

// =============================================================================
// Constructors
// =============================================================================

Camera::Camera()
    : m_position(0.0f, 2.0f, 5.0f)
    , m_front(0.0f, 0.0f, -1.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_right(1.0f, 0.0f, 0.0f)
    , m_worldUp(0.0f, 1.0f, 0.0f)
    , m_yaw(-90.0f)           // Looking along -Z axis
    , m_pitch(0.0f)
    , m_movementSpeed(5.0f)
    , m_mouseSensitivity(0.1f)
    , m_fov(45.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(100.0f)
    , m_mode(CameraMode::FREE_ROAM)
    , m_orbitTarget(0.0f)
    , m_orbitRadius(5.0f)
    , m_orbitYaw(0.0f)
    , m_orbitPitch(20.0f)
    , m_driverSeatPosition(0.0f, 1.0f, 0.5f)
    , m_driverSeatYawLimit(120.0f)
    , m_driverSeatPitchLimit(45.0f)
{
    updateCameraVectors();
}

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
    : m_position(position)
    , m_worldUp(up)
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_movementSpeed(5.0f)
    , m_mouseSensitivity(0.1f)
    , m_fov(45.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(100.0f)
    , m_mode(CameraMode::FREE_ROAM)
    , m_orbitTarget(0.0f)
    , m_orbitRadius(5.0f)
    , m_orbitYaw(0.0f)
    , m_orbitPitch(20.0f)
    , m_driverSeatPosition(0.0f, 1.0f, 0.5f)
    , m_driverSeatYawLimit(120.0f)
    , m_driverSeatPitchLimit(45.0f)
{
    updateCameraVectors();
}

// =============================================================================
// Matrix Getters
// =============================================================================

glm::mat4 Camera::getViewMatrix() const {
    // lookAt constructs a view matrix that:
    // 1. Translates the world so the camera is at the origin
    // 2. Rotates the world so the camera looks down -Z
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    // Perspective projection:
    // - FOV: field of view angle (larger = wider view)
    // - Aspect ratio: width/height (prevents distortion)
    // - Near/Far: clipping planes (objects outside are not rendered)
    return glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
}

// =============================================================================
// Camera Mode
// =============================================================================

void Camera::setMode(CameraMode mode) {
    m_mode = mode;
    
    switch (mode) {
        case CameraMode::FREE_ROAM:
            // Nothing special needed
            break;
            
        case CameraMode::ORBIT:
            // Initialize orbit from current position relative to target
            updateOrbitPosition();
            break;
            
        case CameraMode::DRIVER_SEAT:
            // Move to driver seat position
            m_position = m_driverSeatPosition;
            m_yaw = -90.0f;  // Looking forward
            m_pitch = 0.0f;
            updateCameraVectors();
            break;
    }
}

void Camera::setOrbitTarget(const glm::vec3& target) {
    m_orbitTarget = target;
    if (m_mode == CameraMode::ORBIT) {
        updateOrbitPosition();
    }
}

// =============================================================================
// Input Processing
// =============================================================================

void Camera::processKeyboard(float forward, float right, float up, float deltaTime) {
    float velocity = m_movementSpeed * deltaTime;
    
    switch (m_mode) {
        case CameraMode::FREE_ROAM:
            // Move in the direction the camera is facing
            m_position += m_front * forward * velocity;
            m_position += m_right * right * velocity;
            m_position += m_worldUp * up * velocity;
            break;
            
        case CameraMode::ORBIT:
            // In orbit mode, keyboard rotates around target
            m_orbitYaw += right * velocity * 20.0f;
            m_orbitPitch += up * velocity * 20.0f;
            m_orbitPitch = std::clamp(m_orbitPitch, -80.0f, 80.0f);
            
            // Forward/backward adjusts radius
            m_orbitRadius -= forward * velocity * 2.0f;
            m_orbitRadius = std::clamp(m_orbitRadius, 2.0f, 20.0f);
            
            updateOrbitPosition();
            break;
            
        case CameraMode::DRIVER_SEAT:
            // No movement in driver seat mode
            break;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;
    
    switch (m_mode) {
        case CameraMode::FREE_ROAM:
            m_yaw += xoffset;
            m_pitch += yoffset;
            
            if (constrainPitch) {
                // Prevent flipping when looking too far up/down
                m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
            }
            
            updateCameraVectors();
            break;
            
        case CameraMode::ORBIT:
            // Mouse movement rotates around target
            m_orbitYaw -= xoffset;
            m_orbitPitch += yoffset;
            m_orbitPitch = std::clamp(m_orbitPitch, -80.0f, 80.0f);
            
            updateOrbitPosition();
            break;
            
        case CameraMode::DRIVER_SEAT:
            // Limited look-around from driver seat
            m_yaw += xoffset;
            m_pitch += yoffset;
            
            // Constrain to limits
            m_yaw = std::clamp(m_yaw, -90.0f - m_driverSeatYawLimit, -90.0f + m_driverSeatYawLimit);
            m_pitch = std::clamp(m_pitch, -m_driverSeatPitchLimit, m_driverSeatPitchLimit);
            
            updateCameraVectors();
            break;
    }
}

void Camera::processMouseScroll(float yoffset) {
    switch (m_mode) {
        case CameraMode::FREE_ROAM:
            // Scroll changes FOV (zoom)
            m_fov -= yoffset;
            m_fov = std::clamp(m_fov, 1.0f, 90.0f);
            break;
            
        case CameraMode::ORBIT:
            // Scroll changes orbit radius
            m_orbitRadius -= yoffset * 0.5f;
            m_orbitRadius = std::clamp(m_orbitRadius, 2.0f, 20.0f);
            updateOrbitPosition();
            break;
            
        case CameraMode::DRIVER_SEAT:
            // Scroll changes FOV (simulates leaning forward)
            m_fov -= yoffset;
            m_fov = std::clamp(m_fov, 30.0f, 60.0f);
            break;
    }
}

// =============================================================================
// Setters
// =============================================================================

void Camera::setYaw(float yaw) {
    m_yaw = yaw;
    updateCameraVectors();
}

void Camera::setPitch(float pitch) {
    m_pitch = std::clamp(pitch, -89.0f, 89.0f);
    updateCameraVectors();
}

void Camera::setFOV(float fov) {
    m_fov = std::clamp(fov, 1.0f, 120.0f);
}

void Camera::setClipPlanes(float near, float far) {
    m_nearPlane = near;
    m_farPlane = far;
}

void Camera::setOrbitRadius(float radius) {
    m_orbitRadius = std::clamp(radius, 1.0f, 50.0f);
    if (m_mode == CameraMode::ORBIT) {
        updateOrbitPosition();
    }
}

// =============================================================================
// Private Methods
// =============================================================================

void Camera::updateCameraVectors() {
    // Calculate new front vector from Euler angles
    // This converts spherical coordinates (yaw, pitch) to Cartesian (x, y, z)
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    
    // Recalculate right and up vectors
    // Right = Cross product of front and world up
    // Up = Cross product of right and front
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::updateOrbitPosition() {
    // Calculate camera position on a sphere around the target
    // Using spherical coordinates:
    // x = r * cos(pitch) * cos(yaw)
    // y = r * sin(pitch)
    // z = r * cos(pitch) * sin(yaw)
    
    float pitchRad = glm::radians(m_orbitPitch);
    float yawRad = glm::radians(m_orbitYaw);
    
    glm::vec3 offset;
    offset.x = m_orbitRadius * cos(pitchRad) * cos(yawRad);
    offset.y = m_orbitRadius * sin(pitchRad);
    offset.z = m_orbitRadius * cos(pitchRad) * sin(yawRad);
    
    m_position = m_orbitTarget + offset;
    
    // Look at the target
    m_front = glm::normalize(m_orbitTarget - m_position);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
