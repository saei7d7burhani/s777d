/**
 * =============================================================================
 * Input.h - Input Handling System
 * =============================================================================
 * Manages keyboard and mouse input for the car showroom:
 * - Key state tracking (pressed, released, held)
 * - Mouse position and movement
 * - Mouse button state
 * - Scroll wheel
 * 
 * Input Mapping:
 * - WASD / Arrow keys: Camera movement
 * - Mouse: Camera look
 * - Scroll: Zoom
 * - 1/2/3: Camera mode (Free/Orbit/Driver)
 * - Space: Toggle door
 * - H: Toggle headlights
 * - R: Reset car position
 * - Escape: Exit/Release cursor
 * 
 * Design Decision: Polling-based input with event callbacks. Polling is used
 * for continuous input (movement), while callbacks handle discrete events
 * (key press, mode switch).
 * =============================================================================
 */

#ifndef INPUT_H
#define INPUT_H

#include <functional>
#include <unordered_map>
#include <glm/glm.hpp>

class Window;
class Camera;
class CarModel;

/**
 * Key state enum.
 */
enum class KeyState {
    RELEASED,   // Key is not pressed
    PRESSED,    // Key was just pressed this frame
    HELD,       // Key is being held down
    RELEASED_THIS_FRAME  // Key was just released
};

/**
 * Action binding - Maps an action name to key/mouse input.
 */
struct ActionBinding {
    int key;                    // GLFW key code
    bool requiresShift;         // Requires Shift modifier
    bool requiresCtrl;          // Requires Ctrl modifier
    bool requiresAlt;           // Requires Alt modifier
};

/**
 * Input class - Handles all input for the application.
 */
class Input {
public:
    // Callback types
    using KeyPressCallback = std::function<void(int key)>;
    using MouseMoveCallback = std::function<void(double xpos, double ypos)>;
    using ScrollCallback = std::function<void(double offset)>;
    
    /**
     * Initialize input system for a window.
     */
    explicit Input(Window& window);
    
    /**
     * Update input state. Call once per frame before processing input.
     */
    void update();
    
    // =========================================================================
    // Keyboard Input
    // =========================================================================
    
    /**
     * Check if a key is currently held down.
     */
    bool isKeyHeld(int key) const;
    
    /**
     * Check if a key was just pressed this frame.
     */
    bool isKeyPressed(int key) const;
    
    /**
     * Check if a key was just released this frame.
     */
    bool isKeyReleased(int key) const;
    
    /**
     * Get the state of a key.
     */
    KeyState getKeyState(int key) const;
    
    /**
     * Register a callback for key press events.
     */
    void onKeyPress(KeyPressCallback callback);
    
    // =========================================================================
    // Mouse Input
    // =========================================================================
    
    /**
     * Get current mouse position.
     */
    glm::vec2 getMousePosition() const { return m_mousePosition; }
    
    /**
     * Get mouse movement since last frame.
     */
    glm::vec2 getMouseDelta() const { return m_mouseDelta; }
    
    /**
     * Check if a mouse button is held.
     * @param button GLFW_MOUSE_BUTTON_LEFT, RIGHT, or MIDDLE
     */
    bool isMouseButtonHeld(int button) const;
    
    /**
     * Check if a mouse button was just pressed.
     */
    bool isMouseButtonPressed(int button) const;
    
    /**
     * Get scroll wheel offset since last frame.
     */
    float getScrollOffset() const { return m_scrollOffset; }
    
    /**
     * Register a callback for mouse movement.
     */
    void onMouseMove(MouseMoveCallback callback);
    
    /**
     * Register a callback for scroll events.
     */
    void onScroll(ScrollCallback callback);
    
    // =========================================================================
    // Cursor Control
    // =========================================================================
    
    /**
     * Capture the cursor (hide and lock to window).
     */
    void captureCursor();
    
    /**
     * Release the cursor (show and unlock).
     */
    void releaseCursor();
    
    /**
     * Toggle cursor capture state.
     */
    void toggleCursorCapture();
    
    /**
     * Check if cursor is captured.
     */
    bool isCursorCaptured() const { return m_cursorCaptured; }
    
    // =========================================================================
    // Action Mapping
    // =========================================================================
    
    /**
     * Bind an action to a key.
     */
    void bindAction(const std::string& action, int key);
    
    /**
     * Check if an action is active (key held).
     */
    bool isActionActive(const std::string& action) const;
    
    /**
     * Check if an action was just triggered (key pressed).
     */
    bool isActionTriggered(const std::string& action) const;
    
    // =========================================================================
    // Camera Control
    // =========================================================================
    
    /**
     * Process input for camera movement.
     * @param camera The camera to control
     * @param deltaTime Time since last frame
     */
    void processCamera(Camera& camera, float deltaTime);
    
    /**
     * Process input for car control.
     * @param car The car to control
     * @param deltaTime Time since last frame
     */
    void processCar(CarModel& car, float deltaTime);
    
private:
    Window& m_window;
    
    // Keyboard state
    std::unordered_map<int, KeyState> m_keyStates;
    std::unordered_map<int, KeyState> m_previousKeyStates;
    
    // Mouse state
    glm::vec2 m_mousePosition;
    glm::vec2 m_lastMousePosition;
    glm::vec2 m_mouseDelta;
    bool m_firstMouse;
    
    // Mouse buttons
    std::unordered_map<int, KeyState> m_mouseButtonStates;
    std::unordered_map<int, KeyState> m_previousMouseButtonStates;
    
    // Scroll
    float m_scrollOffset;
    float m_accumulatedScroll;
    
    // Cursor state
    bool m_cursorCaptured;
    
    // Action bindings
    std::unordered_map<std::string, ActionBinding> m_actionBindings;
    
    // Callbacks
    std::vector<KeyPressCallback> m_keyPressCallbacks;
    std::vector<MouseMoveCallback> m_mouseMoveCallbacks;
    std::vector<ScrollCallback> m_scrollCallbacks;
    
    /**
     * Set up default action bindings.
     */
    void setupDefaultBindings();
    
    /**
     * Handle GLFW key callback.
     */
    void handleKey(int key, int scancode, int action, int mods);
    
    /**
     * Handle GLFW mouse move callback.
     */
    void handleMouseMove(double xpos, double ypos);
    
    /**
     * Handle GLFW mouse button callback.
     */
    void handleMouseButton(int button, int action, int mods);
    
    /**
     * Handle GLFW scroll callback.
     */
    void handleScroll(double xoffset, double yoffset);
};

#endif // INPUT_H
