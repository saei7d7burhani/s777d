/**
 * =============================================================================
 * Window.cpp - GLFW Window Management Implementation
 * =============================================================================
 */

#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

// =============================================================================
// Constructor / Destructor
// =============================================================================

Window::Window(int width, int height, const std::string& title)
    : m_window(nullptr)
    , m_width(width)
    , m_height(height)
    , m_title(title)
{
    // Initialize GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    // Configure GLFW for OpenGL 3.3 Core Profile
    // Core Profile = Modern OpenGL without deprecated features
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // For macOS compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Enable MSAA (Multi-Sample Anti-Aliasing) for smoother edges
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    // Create the window
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    
    // Make the OpenGL context current
    glfwMakeContextCurrent(m_window);
    
    // Initialize GLAD to load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }
    
    // Enable V-Sync (synchronize with monitor refresh rate)
    glfwSwapInterval(1);
    
    // Store 'this' pointer in window for callbacks
    glfwSetWindowUserPointer(m_window, this);
    
    // Set up callbacks
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallbackStatic);
    glfwSetKeyCallback(m_window, keyCallbackStatic);
    glfwSetCursorPosCallback(m_window, mouseMoveCallbackStatic);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallbackStatic);
    glfwSetScrollCallback(m_window, scrollCallbackStatic);
    
    // Print OpenGL info
    std::cout << "OpenGL Version: " << glGetString(0x1F02) << std::endl;  // GL_VERSION
    std::cout << "GLSL Version: " << glGetString(0x8B8C) << std::endl;    // GL_SHADING_LANGUAGE_VERSION
    std::cout << "Renderer: " << glGetString(0x1F01) << std::endl;        // GL_RENDERER
}

Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

// Move constructor
Window::Window(Window&& other) noexcept
    : m_window(other.m_window)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_title(std::move(other.m_title))
{
    other.m_window = nullptr;
    if (m_window) {
        glfwSetWindowUserPointer(m_window, this);
    }
}

// Move assignment
Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {
        if (m_window) {
            glfwDestroyWindow(m_window);
        }
        m_window = other.m_window;
        m_width = other.m_width;
        m_height = other.m_height;
        m_title = std::move(other.m_title);
        other.m_window = nullptr;
        if (m_window) {
            glfwSetWindowUserPointer(m_window, this);
        }
    }
    return *this;
}

// =============================================================================
// Window State
// =============================================================================

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::close() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

double Window::getTime() {
    return glfwGetTime();
}

// =============================================================================
// Cursor Control
// =============================================================================

void Window::setCursorCaptured(bool captured) {
    if (captured) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

bool Window::isKeyPressed(int key) const {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

void Window::getMousePosition(double& x, double& y) const {
    glfwGetCursorPos(m_window, &x, &y);
}

// =============================================================================
// Callback Setters
// =============================================================================

void Window::setFramebufferSizeCallback(FramebufferSizeCallback callback) {
    m_framebufferSizeCallback = std::move(callback);
}

void Window::setKeyCallback(KeyCallback callback) {
    m_keyCallback = std::move(callback);
}

void Window::setMouseMoveCallback(MouseMoveCallback callback) {
    m_mouseMoveCallback = std::move(callback);
}

void Window::setMouseButtonCallback(MouseButtonCallback callback) {
    m_mouseButtonCallback = std::move(callback);
}

void Window::setScrollCallback(ScrollCallback callback) {
    m_scrollCallback = std::move(callback);
}

// =============================================================================
// Static Callbacks (forward to instance methods)
// =============================================================================

void Window::framebufferSizeCallbackStatic(GLFWwindow* window, int width, int height) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->m_width = width;
        self->m_height = height;
        glViewport(0, 0, width, height);
        if (self->m_framebufferSizeCallback) {
            self->m_framebufferSizeCallback(width, height);
        }
    }
}

void Window::keyCallbackStatic(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self && self->m_keyCallback) {
        self->m_keyCallback(key, scancode, action, mods);
    }
}

void Window::mouseMoveCallbackStatic(GLFWwindow* window, double xpos, double ypos) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self && self->m_mouseMoveCallback) {
        self->m_mouseMoveCallback(xpos, ypos);
    }
}

void Window::mouseButtonCallbackStatic(GLFWwindow* window, int button, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self && self->m_mouseButtonCallback) {
        self->m_mouseButtonCallback(button, action, mods);
    }
}

void Window::scrollCallbackStatic(GLFWwindow* window, double xoffset, double yoffset) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self && self->m_scrollCallback) {
        self->m_scrollCallback(xoffset, yoffset);
    }
}
