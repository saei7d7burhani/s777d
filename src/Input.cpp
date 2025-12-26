/**
 * =============================================================================
 * Input.cpp - Input Handling System Implementation
 * =============================================================================
 */

#include "Input.h"
#include "Window.h"
#include "Camera.h"
#include "CarModel.h"

#include <GLFW/glfw3.h>

// =============================================================================
// Constructor
// =============================================================================

Input::Input(Window& window)
    : m_window(window)
    , m_mousePosition(0.0f)
    , m_lastMousePosition(0.0f)
    , m_mouseDelta(0.0f)
    , m_firstMouse(true)
    , m_scrollOffset(0.0f)
    , m_accumulatedScroll(0.0f)
    , m_cursorCaptured(false)
{
    setupDefaultBindings();
    
    // Set up window callbacks to forward to this input handler
    window.setKeyCallback([this](int key, int scancode, int action, int mods) {
        handleKey(key, scancode, action, mods);
    });
    
    window.setMouseMoveCallback([this](double xpos, double ypos) {
        handleMouseMove(xpos, ypos);
    });
    
    window.setMouseButtonCallback([this](int button, int action, int mods) {
        handleMouseButton(button, action, mods);
    });
    
    window.setScrollCallback([this](double xoffset, double yoffset) {
        handleScroll(xoffset, yoffset);
    });
}

// =============================================================================
// Update
// =============================================================================

void Input::update() {
    // Update previous key states
    m_previousKeyStates = m_keyStates;
    m_previousMouseButtonStates = m_mouseButtonStates;
    
    // Reset per-frame values
    m_mouseDelta = glm::vec2(0.0f);
    m_scrollOffset = m_accumulatedScroll;
    m_accumulatedScroll = 0.0f;
    
    // Calculate mouse delta
    double x, y;
    m_window.getMousePosition(x, y);
    glm::vec2 currentPos(static_cast<float>(x), static_cast<float>(y));
    
    if (m_firstMouse) {
        m_lastMousePosition = currentPos;
        m_firstMouse = false;
    }
    
    m_mouseDelta = currentPos - m_lastMousePosition;
    m_lastMousePosition = currentPos;
    m_mousePosition = currentPos;
}

// =============================================================================
// Keyboard Input
// =============================================================================

bool Input::isKeyHeld(int key) const {
    auto it = m_keyStates.find(key);
    if (it != m_keyStates.end()) {
        return it->second == KeyState::PRESSED || it->second == KeyState::HELD;
    }
    return false;
}

bool Input::isKeyPressed(int key) const {
    auto it = m_keyStates.find(key);
    auto prevIt = m_previousKeyStates.find(key);
    
    bool currentlyHeld = (it != m_keyStates.end() && 
                          (it->second == KeyState::PRESSED || it->second == KeyState::HELD));
    bool previouslyHeld = (prevIt != m_previousKeyStates.end() && 
                           (prevIt->second == KeyState::PRESSED || prevIt->second == KeyState::HELD));
    
    return currentlyHeld && !previouslyHeld;
}

bool Input::isKeyReleased(int key) const {
    auto it = m_keyStates.find(key);
    auto prevIt = m_previousKeyStates.find(key);
    
    bool currentlyHeld = (it != m_keyStates.end() && 
                          (it->second == KeyState::PRESSED || it->second == KeyState::HELD));
    bool previouslyHeld = (prevIt != m_previousKeyStates.end() && 
                           (prevIt->second == KeyState::PRESSED || prevIt->second == KeyState::HELD));
    
    return !currentlyHeld && previouslyHeld;
}

KeyState Input::getKeyState(int key) const {
    auto it = m_keyStates.find(key);
    if (it != m_keyStates.end()) {
        return it->second;
    }
    return KeyState::RELEASED;
}

void Input::onKeyPress(KeyPressCallback callback) {
    m_keyPressCallbacks.push_back(std::move(callback));
}

// =============================================================================
// Mouse Input
// =============================================================================

bool Input::isMouseButtonHeld(int button) const {
    auto it = m_mouseButtonStates.find(button);
    if (it != m_mouseButtonStates.end()) {
        return it->second == KeyState::PRESSED || it->second == KeyState::HELD;
    }
    return false;
}

bool Input::isMouseButtonPressed(int button) const {
    auto it = m_mouseButtonStates.find(button);
    auto prevIt = m_previousMouseButtonStates.find(button);
    
    bool currentlyHeld = (it != m_mouseButtonStates.end() && 
                          (it->second == KeyState::PRESSED || it->second == KeyState::HELD));
    bool previouslyHeld = (prevIt != m_previousMouseButtonStates.end() && 
                           (prevIt->second == KeyState::PRESSED || prevIt->second == KeyState::HELD));
    
    return currentlyHeld && !previouslyHeld;
}

void Input::onMouseMove(MouseMoveCallback callback) {
    m_mouseMoveCallbacks.push_back(std::move(callback));
}

void Input::onScroll(ScrollCallback callback) {
    m_scrollCallbacks.push_back(std::move(callback));
}

// =============================================================================
// Cursor Control
// =============================================================================

void Input::captureCursor() {
    m_cursorCaptured = true;
    m_window.setCursorCaptured(true);
    m_firstMouse = true;  // Reset to avoid jump on capture
}

void Input::releaseCursor() {
    m_cursorCaptured = false;
    m_window.setCursorCaptured(false);
}

void Input::toggleCursorCapture() {
    if (m_cursorCaptured) {
        releaseCursor();
    } else {
        captureCursor();
    }
}

// =============================================================================
// Action Mapping
// =============================================================================

void Input::bindAction(const std::string& action, int key) {
    ActionBinding binding;
    binding.key = key;
    binding.requiresShift = false;
    binding.requiresCtrl = false;
    binding.requiresAlt = false;
    m_actionBindings[action] = binding;
}

bool Input::isActionActive(const std::string& action) const {
    auto it = m_actionBindings.find(action);
    if (it != m_actionBindings.end()) {
        return isKeyHeld(it->second.key);
    }
    return false;
}

bool Input::isActionTriggered(const std::string& action) const {
    auto it = m_actionBindings.find(action);
    if (it != m_actionBindings.end()) {
        return isKeyPressed(it->second.key);
    }
    return false;
}

// =============================================================================
// Camera Control
// =============================================================================

void Input::processCamera(Camera& camera, float deltaTime) {
    // Only process input if cursor is captured
    if (!m_cursorCaptured) {
        return;
    }
    
    // Keyboard movement
    float forward = 0.0f, right = 0.0f, up = 0.0f;
    
    if (isKeyHeld(GLFW_KEY_W) || isKeyHeld(GLFW_KEY_UP)) forward += 1.0f;
    if (isKeyHeld(GLFW_KEY_S) || isKeyHeld(GLFW_KEY_DOWN)) forward -= 1.0f;
    if (isKeyHeld(GLFW_KEY_D) || isKeyHeld(GLFW_KEY_RIGHT)) right += 1.0f;
    if (isKeyHeld(GLFW_KEY_A) || isKeyHeld(GLFW_KEY_LEFT)) right -= 1.0f;
    if (isKeyHeld(GLFW_KEY_SPACE)) up += 1.0f;
    if (isKeyHeld(GLFW_KEY_LEFT_CONTROL)) up -= 1.0f;
    
    camera.processKeyboard(forward, right, up, deltaTime);
    
    // Mouse look
    if (m_mouseDelta.x != 0.0f || m_mouseDelta.y != 0.0f) {
        camera.processMouseMovement(m_mouseDelta.x, -m_mouseDelta.y);
    }
    
    // Scroll zoom
    if (m_scrollOffset != 0.0f) {
        camera.processMouseScroll(m_scrollOffset);
    }
}

void Input::processCar(CarModel& car, float deltaTime) {
    float move = 0.0f;
    float turn = 0.0f;
    
    if (isKeyHeld(GLFW_KEY_I)) move += 5.0f;
    if (isKeyHeld(GLFW_KEY_K)) move -= 5.0f;
    if (isKeyHeld(GLFW_KEY_J)) turn += 60.0f;
    if (isKeyHeld(GLFW_KEY_L)) turn -= 60.0f;
    
    car.move(move, deltaTime);
    car.turn(turn, deltaTime);
}

// =============================================================================
// Private Methods
// =============================================================================

void Input::setupDefaultBindings() {
    bindAction("move_forward", GLFW_KEY_W);
    bindAction("move_backward", GLFW_KEY_S);
    bindAction("move_left", GLFW_KEY_A);
    bindAction("move_right", GLFW_KEY_D);
    bindAction("move_up", GLFW_KEY_SPACE);
    bindAction("move_down", GLFW_KEY_LEFT_CONTROL);
    
    bindAction("camera_free", GLFW_KEY_1);
    bindAction("camera_orbit", GLFW_KEY_2);
    bindAction("camera_driver", GLFW_KEY_3);
    
    bindAction("toggle_door", GLFW_KEY_O);
    bindAction("toggle_headlights", GLFW_KEY_H);
    bindAction("reset_car", GLFW_KEY_R);
    
    bindAction("quit", GLFW_KEY_ESCAPE);
}

void Input::handleKey(int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
    if (action == GLFW_PRESS) {
        m_keyStates[key] = KeyState::PRESSED;
        
        // Notify callbacks
        for (auto& callback : m_keyPressCallbacks) {
            callback(key);
        }
    } else if (action == GLFW_RELEASE) {
        m_keyStates[key] = KeyState::RELEASED;
    } else if (action == GLFW_REPEAT) {
        m_keyStates[key] = KeyState::HELD;
    }
}

void Input::handleMouseMove(double xpos, double ypos) {
    for (auto& callback : m_mouseMoveCallbacks) {
        callback(xpos, ypos);
    }
}

void Input::handleMouseButton(int button, int action, [[maybe_unused]] int mods) {
    if (action == GLFW_PRESS) {
        m_mouseButtonStates[button] = KeyState::PRESSED;
    } else if (action == GLFW_RELEASE) {
        m_mouseButtonStates[button] = KeyState::RELEASED;
    }
}

void Input::handleScroll([[maybe_unused]] double xoffset, double yoffset) {
    m_accumulatedScroll += static_cast<float>(yoffset);
    
    for (auto& callback : m_scrollCallbacks) {
        callback(yoffset);
    }
}
