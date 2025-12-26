/**
 * =============================================================================
 * Renderer.h - OpenGL Rendering System
 * =============================================================================
 * The Renderer class encapsulates all OpenGL rendering operations:
 * - Shader management
 * - Drawing objects with proper state
 * - Handling transparent objects (sorted back-to-front)
 * - Post-processing (optional)
 * 
 * Rendering Pipeline Overview:
 * ----------------------------
 * 1. Clear buffers (color, depth, stencil)
 * 2. Set up view/projection matrices
 * 3. Render opaque objects (any order, with depth testing)
 * 4. Render transparent objects (sorted back-to-front, blending enabled)
 * 5. Post-processing (if any)
 * 6. Swap buffers
 * 
 * Design Decision: Using a deferred-style approach for collecting render
 * commands, then executing them in the correct order. This allows proper
 * handling of transparency without requiring the scene to be structured
 * in any particular way.
 * =============================================================================
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>

class Shader;
class Camera;
class Model;
class Light;
class DirectionalLight;
class PointLight;
class SpotLight;

/**
 * RenderCommand - Stores information needed to render an object.
 */
struct RenderCommand {
    const Model* model;
    glm::mat4 transform;
    bool transparent;
    float distanceToCamera;  // For sorting transparent objects
};

/**
 * Renderer class - Handles all OpenGL rendering operations.
 */
class Renderer {
public:
    /**
     * Initialize the renderer.
     * @param width Viewport width
     * @param height Viewport height
     */
    Renderer(int width, int height);
    
    /**
     * Destructor - Cleans up OpenGL resources.
     */
    ~Renderer();
    
    // Disable copying
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    
    // =========================================================================
    // Frame Management
    // =========================================================================
    
    /**
     * Begin a new frame. Clears buffers and resets state.
     */
    void beginFrame();
    
    /**
     * End the frame. Executes all queued render commands.
     */
    void endFrame();
    
    /**
     * Handle viewport resize.
     */
    void resize(int width, int height);
    
    // =========================================================================
    // Camera Setup
    // =========================================================================
    
    /**
     * Set the camera for this frame.
     */
    void setCamera(const Camera& camera);
    
    // =========================================================================
    // Lighting Setup
    // =========================================================================
    
    /**
     * Set the directional light.
     */
    void setDirectionalLight(const DirectionalLight& light);
    
    /**
     * Add a point light (up to MAX_POINT_LIGHTS).
     */
    void addPointLight(const PointLight& light);
    
    /**
     * Add a spot light (up to MAX_SPOT_LIGHTS).
     */
    void addSpotLight(const SpotLight& light);
    
    /**
     * Clear all lights for new frame.
     */
    void clearLights();
    
    // =========================================================================
    // Rendering
    // =========================================================================
    
    /**
     * Submit a model for rendering.
     * The model will be rendered during endFrame() in the correct order.
     */
    void submit(const Model& model, const glm::mat4& transform = glm::mat4(1.0f));
    
    /**
     * Draw a model immediately (bypasses the command queue).
     * Use for debugging or UI elements.
     */
    void drawImmediate(const Model& model, Shader& shader);
    
    // =========================================================================
    // Render Settings
    // =========================================================================
    
    /**
     * Set the clear color.
     */
    void setClearColor(const glm::vec3& color);
    
    /**
     * Enable/disable wireframe mode.
     */
    void setWireframe(bool enabled);
    
    /**
     * Enable/disable face culling.
     */
    void setCulling(bool enabled);
    
    /**
     * Get the main shader.
     */
    Shader& getShader() { return *m_shader; }
    const Shader& getShader() const { return *m_shader; }
    
    // =========================================================================
    // Statistics
    // =========================================================================
    
    /**
     * Get number of draw calls this frame.
     */
    int getDrawCallCount() const { return m_drawCallCount; }
    
    /**
     * Get number of triangles rendered this frame.
     */
    int getTriangleCount() const { return m_triangleCount; }
    
    // =========================================================================
    // Constants
    // =========================================================================
    
    static constexpr int MAX_POINT_LIGHTS = 4;
    static constexpr int MAX_SPOT_LIGHTS = 2;
    
private:
    // Viewport dimensions
    int m_width;
    int m_height;
    
    // Shaders
    std::unique_ptr<Shader> m_shader;
    
    // Camera matrices (cached for the frame)
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    glm::vec3 m_cameraPosition;
    
    // Render queue
    std::vector<RenderCommand> m_opaqueCommands;
    std::vector<RenderCommand> m_transparentCommands;
    
    // Lights
    DirectionalLight* m_directionalLight;
    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;
    
    // Settings
    glm::vec3 m_clearColor;
    bool m_wireframeMode;
    bool m_cullingEnabled;
    
    // Statistics
    int m_drawCallCount;
    int m_triangleCount;
    
    /**
     * Set up OpenGL state for rendering.
     */
    void setupRenderState();
    
    /**
     * Apply lighting to the shader.
     */
    void applyLighting();
    
    /**
     * Sort transparent objects back-to-front.
     */
    void sortTransparentCommands();
    
    /**
     * Execute a render command.
     */
    void executeCommand(const RenderCommand& cmd);
    
    /**
     * Create and compile shaders.
     */
    void createShaders();
};

#endif // RENDERER_H
