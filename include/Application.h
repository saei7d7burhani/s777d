/**
 * =============================================================================
 * Application.h - Main Application Class
 * =============================================================================
 * The Application class ties together all components of the car showroom:
 * - Window and input handling
 * - Scene management
 * - Rendering
 * - Main loop
 * 
 * This is the entry point for the application logic. main() creates an
 * Application instance and calls run().
 * 
 * Frame Timing:
 * -------------
 * The application uses a fixed timestep for physics/animation updates
 * while allowing variable framerate for rendering. This ensures consistent
 * behavior regardless of frame rate.
 * 
 * Main Loop Structure:
 * 1. Calculate delta time
 * 2. Process input
 * 3. Update scene (with fixed timestep if needed)
 * 4. Render
 * 5. Swap buffers and poll events
 * =============================================================================
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <string>

class Window;
class Renderer;
class Camera;
class ShowroomScene;
class Input;

/**
 * Application class - Main application controller.
 */
class Application {
public:
    /**
     * Create the application.
     * 
     * @param width Window width
     * @param height Window height
     * @param title Window title
     */
    Application(int width = 1280, int height = 720, 
                const std::string& title = "3D Car Showroom");
    
    /**
     * Destructor - Cleans up all resources.
     */
    ~Application();
    
    // Disable copying
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    /**
     * Run the main application loop.
     * Returns when the window is closed.
     * 
     * @return Exit code (0 for success)
     */
    int run();
    
    /**
     * Request application shutdown.
     */
    void quit();
    
    // =========================================================================
    // Accessors
    // =========================================================================
    
    Window& getWindow() { return *m_window; }
    const Window& getWindow() const { return *m_window; }
    
    Renderer& getRenderer() { return *m_renderer; }
    const Renderer& getRenderer() const { return *m_renderer; }
    
    Camera& getCamera() { return *m_camera; }
    const Camera& getCamera() const { return *m_camera; }
    
    ShowroomScene& getScene() { return *m_scene; }
    const ShowroomScene& getScene() const { return *m_scene; }
    
    Input& getInput() { return *m_input; }
    const Input& getInput() const { return *m_input; }
    
    // =========================================================================
    // Timing
    // =========================================================================
    
    /**
     * Get time since last frame in seconds.
     */
    float getDeltaTime() const { return m_deltaTime; }
    
    /**
     * Get total elapsed time since start.
     */
    float getElapsedTime() const { return m_elapsedTime; }
    
    /**
     * Get current frames per second.
     */
    float getFPS() const { return m_fps; }
    
private:
    // Core components
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<ShowroomScene> m_scene;
    std::unique_ptr<Input> m_input;
    
    // Application state
    bool m_running;
    
    // Timing
    float m_deltaTime;
    float m_elapsedTime;
    float m_lastFrameTime;
    float m_fps;
    float m_fpsAccumulator;
    int m_frameCount;
    
    // Fixed timestep for physics
    static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
    float m_physicsAccumulator;
    
    /**
     * Initialize all subsystems.
     */
    bool initialize();
    
    /**
     * Process input for the current frame.
     */
    void processInput();
    
    /**
     * Update game logic.
     * @param deltaTime Time since last update
     */
    void update(float deltaTime);
    
    /**
     * Fixed timestep update for physics.
     * @param fixedDeltaTime Fixed time step
     */
    void fixedUpdate(float fixedDeltaTime);
    
    /**
     * Render the current frame.
     */
    void render();
    
    /**
     * Handle window resize.
     */
    void onResize(int width, int height);
    
    /**
     * Handle key press.
     */
    void onKeyPress(int key);
    
    /**
     * Update FPS counter.
     */
    void updateFPS();
};

#endif // APPLICATION_H
