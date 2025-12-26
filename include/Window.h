/**
 * =============================================================================
 * Window.h - GLFW Window Management
 * =============================================================================
 * Encapsulates GLFW window creation and management. The Window class handles:
 * - Window creation with OpenGL context
 * - Input callbacks registration
 * - Window state management (resize, close, etc.)
 * 
 * Design Decision: Using RAII pattern - the constructor creates the window,
 * and the destructor cleans up resources. This ensures proper cleanup even
 * if exceptions occur.
 * =============================================================================
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <functional>

// Forward declaration to avoid including GLFW in header
struct GLFWwindow;

/**
 * Callback types for window events.
 * Using std::function allows binding member functions and lambdas.
 */
using FramebufferSizeCallback = std::function<void(int width, int height)>;
using KeyCallback = std::function<void(int key, int scancode, int action, int mods)>;
using MouseMoveCallback = std::function<void(double xpos, double ypos)>;
using MouseButtonCallback = std::function<void(int button, int action, int mods)>;
using ScrollCallback = std::function<void(double xoffset, double yoffset)>;

/**
 * Window class - Manages the application window and OpenGL context.
 * 
 * Usage:
 *   Window window(1280, 720, "My App");
 *   while (!window.shouldClose()) {
 *       // Render...
 *       window.swapBuffers();
 *       window.pollEvents();
 *   }
 */
class Window {
public:
    /**
     * Create a new window with OpenGL 3.3 Core Profile context.
     * 
     * @param width Initial window width in pixels
     * @param height Initial window height in pixels
     * @param title Window title
     * @throws std::runtime_error if window creation fails
     */
    Window(int width, int height, const std::string& title);
    
    /**
     * Destructor - Destroys window and terminates GLFW.
     */
    ~Window();
    
    // Disable copying - window resources shouldn't be duplicated
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    
    // Allow moving - transfers ownership of window handle
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;
    
    /**
     * Check if window should close (user clicked X or pressed Alt+F4).
     */
    bool shouldClose() const;
    
    /**
     * Swap front and back buffers (double buffering).
     * This displays the rendered frame.
     */
    void swapBuffers();
    
    /**
     * Process pending window events (keyboard, mouse, resize, etc.)
     */
    void pollEvents();
    
    /**
     * Get current window dimensions.
     */
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    float getAspectRatio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }
    
    /**
     * Get the underlying GLFW window handle.
     * Needed for setting callbacks and other GLFW operations.
     */
    GLFWwindow* getHandle() const { return m_window; }
    
    /**
     * Set cursor mode.
     * @param captured true to hide and capture cursor (FPS-style), false for normal
     */
    void setCursorCaptured(bool captured);
    
    /**
     * Check if a key is currently pressed.
     */
    bool isKeyPressed(int key) const;
    
    /**
     * Get mouse position.
     */
    void getMousePosition(double& x, double& y) const;
    
    // Event callback setters
    void setFramebufferSizeCallback(FramebufferSizeCallback callback);
    void setKeyCallback(KeyCallback callback);
    void setMouseMoveCallback(MouseMoveCallback callback);
    void setMouseButtonCallback(MouseButtonCallback callback);
    void setScrollCallback(ScrollCallback callback);
    
    /**
     * Request window close.
     */
    void close();
    
    /**
     * Get current time since GLFW initialization.
     */
    static double getTime();
    
private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;
    
    // Store callbacks as member variables
    FramebufferSizeCallback m_framebufferSizeCallback;
    KeyCallback m_keyCallback;
    MouseMoveCallback m_mouseMoveCallback;
    MouseButtonCallback m_mouseButtonCallback;
    ScrollCallback m_scrollCallback;
    
    // Static callbacks that forward to instance methods
    static void framebufferSizeCallbackStatic(GLFWwindow* window, int width, int height);
    static void keyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseMoveCallbackStatic(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallbackStatic(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallbackStatic(GLFWwindow* window, double xoffset, double yoffset);
};

#endif // WINDOW_H
