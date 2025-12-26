/**
 * =============================================================================
 * Application.cpp - Main Application Class Implementation
 * =============================================================================
 */

#include "Application.h"
#include "Window.h"
#include "Renderer.h"
#include "Camera.h"
#include "ShowroomScene.h"
#include "Input.h"
#include "CarModel.h"

#include <GLFW/glfw3.h>
#include <iostream>

// =============================================================================
// Constructor / Destructor
// =============================================================================

Application::Application(int width, int height, const std::string& title)
    : m_running(false)
    , m_deltaTime(0.0f)
    , m_elapsedTime(0.0f)
    , m_lastFrameTime(0.0f)
    , m_fps(0.0f)
    , m_fpsAccumulator(0.0f)
    , m_frameCount(0)
    , m_physicsAccumulator(0.0f)
{
    // Create window first (initializes OpenGL context)
    m_window = std::make_unique<Window>(width, height, title);
    
    // Create renderer
    m_renderer = std::make_unique<Renderer>(width, height);
    
    // Create camera
    m_camera = std::make_unique<Camera>(
        glm::vec3(0.0f, 3.0f, 10.0f),  // Position
        glm::vec3(0.0f, 1.0f, 0.0f),   // Up
        -90.0f,                         // Yaw
        -15.0f                          // Pitch
    );
    m_camera->setMode(CameraMode::ORBIT);
    
    // Create scene
    m_scene = std::make_unique<ShowroomScene>();
    
    // Set orbit target to main car
    if (m_scene->getMainCar()) {
        m_camera->setOrbitTarget(m_scene->getMainCar()->getOrbitTarget());
    }
    
    // Create input handler
    m_input = std::make_unique<Input>(*m_window);
    
    // Set up window callbacks
    m_window->setFramebufferSizeCallback([this](int w, int h) {
        onResize(w, h);
    });
    
    // Set up input callbacks
    m_input->onKeyPress([this](int key) {
        onKeyPress(key);
    });
}

Application::~Application() = default;

// =============================================================================
// Main Loop
// =============================================================================

int Application::run() {
    m_running = true;
    m_lastFrameTime = static_cast<float>(Window::getTime());
    
    // Capture cursor for camera control
    m_input->captureCursor();
    
    std::cout << "\n=== 3D Car Showroom Controls ===" << std::endl;
    std::cout << "WASD / Arrow Keys: Move camera" << std::endl;
    std::cout << "Mouse: Look around" << std::endl;
    std::cout << "Scroll: Zoom" << std::endl;
    std::cout << "1: Free-roam camera" << std::endl;
    std::cout << "2: Orbit camera" << std::endl;
    std::cout << "3: Driver seat camera" << std::endl;
    std::cout << "I/K: Move car forward/backward" << std::endl;
    std::cout << "J/L: Turn car left/right" << std::endl;
    std::cout << "O: Toggle door" << std::endl;
    std::cout << "H: Toggle headlights" << std::endl;
    std::cout << "R: Reset car position" << std::endl;
    std::cout << "Escape: Release cursor / Exit" << std::endl;
    std::cout << "================================\n" << std::endl;
    
    while (m_running && !m_window->shouldClose()) {
        // Calculate delta time
        float currentTime = static_cast<float>(Window::getTime());
        m_deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;
        m_elapsedTime = currentTime;
        
        // Cap delta time to prevent physics explosions
        if (m_deltaTime > 0.1f) {
            m_deltaTime = 0.1f;
        }
        
        // Update FPS counter
        updateFPS();
        
        // Process input
        processInput();
        
        // Fixed timestep for physics
        m_physicsAccumulator += m_deltaTime;
        while (m_physicsAccumulator >= FIXED_TIMESTEP) {
            fixedUpdate(FIXED_TIMESTEP);
            m_physicsAccumulator -= FIXED_TIMESTEP;
        }
        
        // Variable timestep update
        update(m_deltaTime);
        
        // Render
        render();
        
        // Swap buffers and poll events
        m_window->swapBuffers();
        m_window->pollEvents();
    }
    
    return 0;
}

void Application::quit() {
    m_running = false;
}

// =============================================================================
// Private Methods
// =============================================================================

void Application::processInput() {
    m_input->update();
    
    // Camera movement
    m_input->processCamera(*m_camera, m_deltaTime);
    
    // Car control
    if (m_scene->getMainCar()) {
        m_input->processCar(*m_scene->getMainCar(), m_deltaTime);
    }
}

void Application::update(float deltaTime) {
    // Update scene
    m_scene->update(deltaTime);
    
    // Update camera orbit target (in case car moved)
    if (m_camera->getMode() == CameraMode::ORBIT && m_scene->getMainCar()) {
        m_camera->setOrbitTarget(m_scene->getMainCar()->getOrbitTarget());
    }
    
    // Update driver seat camera position
    if (m_camera->getMode() == CameraMode::DRIVER_SEAT && m_scene->getMainCar()) {
        m_camera->setPosition(m_scene->getMainCar()->getDriverSeatPosition());
    }
}

void Application::fixedUpdate([[maybe_unused]] float fixedDeltaTime) {
    // Physics updates would go here
    // For now, we just handle collision
    
    if (m_scene->getMainCar()) {
        CarModel* car = m_scene->getMainCar();
        glm::vec3 carPos = car->getPosition();
        glm::vec3 carSize(4.0f, 1.5f, 1.8f);  // Approximate car dimensions
        
        // Constrain car to showroom bounds
        glm::vec3 constrainedPos = m_scene->constrainPosition(carPos, carSize);
        if (constrainedPos != carPos) {
            car->setPosition(constrainedPos);
        }
    }
}

void Application::render() {
    // Begin frame
    m_renderer->beginFrame();
    
    // Set camera
    m_renderer->setCamera(*m_camera);
    
    // Set lighting
    m_renderer->setDirectionalLight(m_scene->getDirectionalLight());
    for (const auto& light : m_scene->getPointLights()) {
        m_renderer->addPointLight(light);
    }
    for (const auto& light : m_scene->getSpotLights()) {
        m_renderer->addSpotLight(light);
    }
    
    // Draw scene
    m_scene->draw(m_renderer->getShader());
    
    // End frame
    m_renderer->endFrame();
}

void Application::onResize(int width, int height) {
    m_renderer->resize(width, height);
}

void Application::onKeyPress(int key) {
    // Camera mode switching
    if (key == GLFW_KEY_1) {
        m_camera->setMode(CameraMode::FREE_ROAM);
        std::cout << "Camera mode: Free-roam" << std::endl;
    } else if (key == GLFW_KEY_2) {
        m_camera->setMode(CameraMode::ORBIT);
        if (m_scene->getMainCar()) {
            m_camera->setOrbitTarget(m_scene->getMainCar()->getOrbitTarget());
        }
        std::cout << "Camera mode: Orbit" << std::endl;
    } else if (key == GLFW_KEY_3) {
        m_camera->setMode(CameraMode::DRIVER_SEAT);
        if (m_scene->getMainCar()) {
            m_camera->setPosition(m_scene->getMainCar()->getDriverSeatPosition());
        }
        std::cout << "Camera mode: Driver seat" << std::endl;
    }
    
    // Car controls
    if (m_scene->getMainCar()) {
        CarModel* car = m_scene->getMainCar();
        
        if (key == GLFW_KEY_O) {
            // Toggle front left door
            static bool doorOpen = false;
            doorOpen = !doorOpen;
            car->setDoorOpen(DoorPosition::FRONT_LEFT, doorOpen);
            std::cout << "Door: " << (doorOpen ? "Open" : "Closed") << std::endl;
        }
        
        if (key == GLFW_KEY_H) {
            car->setHeadlightsOn(!car->areHeadlightsOn());
            std::cout << "Headlights: " << (car->areHeadlightsOn() ? "On" : "Off") << std::endl;
        }
        
        if (key == GLFW_KEY_R) {
            car->setPosition(glm::vec3(0.0f, 0.2f, 0.0f));
            car->setRotation(glm::vec3(0.0f));
            std::cout << "Car position reset" << std::endl;
        }
    }
    
    // Escape handling
    if (key == GLFW_KEY_ESCAPE) {
        if (m_input->isCursorCaptured()) {
            m_input->releaseCursor();
            std::cout << "Cursor released (press mouse to recapture)" << std::endl;
        } else {
            quit();
        }
    }
}

void Application::updateFPS() {
    m_frameCount++;
    m_fpsAccumulator += m_deltaTime;
    
    if (m_fpsAccumulator >= 1.0f) {
        m_fps = static_cast<float>(m_frameCount) / m_fpsAccumulator;
        m_frameCount = 0;
        m_fpsAccumulator = 0.0f;
        
        // Update window title with FPS
        // (Not implemented to keep code simple)
    }
}
