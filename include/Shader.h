/**
 * =============================================================================
 * Shader.h - OpenGL Shader Program Management
 * =============================================================================
 * The Shader class handles compilation and linking of vertex and fragment
 * shaders into a shader program. It also provides uniform setting utilities.
 * 
 * OpenGL Rendering Pipeline (simplified):
 * 1. Vertex Shader: Processes each vertex (position, transform to clip space)
 * 2. Primitive Assembly: Groups vertices into triangles
 * 3. Rasterization: Converts triangles to fragments (potential pixels)
 * 4. Fragment Shader: Computes color for each fragment
 * 5. Output: Final pixel colors written to framebuffer
 * 
 * Design Decision: Shaders are loaded from files to allow hot-reloading
 * during development and keep shader code separate from C++ code.
 * =============================================================================
 */

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>

/**
 * Shader class - Manages OpenGL shader programs.
 * 
 * Usage:
 *   Shader shader("vertex.glsl", "fragment.glsl");
 *   shader.use();
 *   shader.setMat4("model", modelMatrix);
 *   // Draw...
 */
class Shader {
public:
    /**
     * Default constructor - creates an empty/invalid shader.
     */
    Shader();
    
    /**
     * Load and compile shaders from source code strings.
     * 
     * @param vertexSource Vertex shader GLSL source code
     * @param fragmentSource Fragment shader GLSL source code
     * @param fromFile If true, treat sources as file paths; if false, treat as source code
     */
    Shader(const std::string& vertexSource, const std::string& fragmentSource, bool fromFile = true);
    
    /**
     * Destructor - Deletes the shader program from GPU memory.
     */
    ~Shader();
    
    // Disable copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    // Allow moving
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;
    
    /**
     * Activate this shader program for subsequent draw calls.
     * Only one shader can be active at a time.
     */
    void use() const;
    
    /**
     * Check if shader compiled and linked successfully.
     */
    bool isValid() const { return m_programID != 0; }
    
    /**
     * Get the OpenGL program ID.
     */
    unsigned int getID() const { return m_programID; }
    
    // =========================================================================
    // Uniform Setters
    // =========================================================================
    // Uniforms are variables passed from CPU to GPU. They remain constant
    // for all vertices/fragments in a single draw call, unlike attributes
    // which vary per-vertex.
    
    /**
     * Set a boolean uniform (stored as int in OpenGL).
     */
    void setBool(const std::string& name, bool value) const;
    
    /**
     * Set an integer uniform.
     */
    void setInt(const std::string& name, int value) const;
    
    /**
     * Set a float uniform.
     */
    void setFloat(const std::string& name, float value) const;
    
    /**
     * Set a vec2 uniform.
     */
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    
    /**
     * Set a vec3 uniform.
     * Common uses: positions, colors, directions
     */
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    
    /**
     * Set a vec4 uniform.
     * Common uses: colors with alpha, homogeneous coordinates
     */
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    
    /**
     * Set a 3x3 matrix uniform.
     * Common uses: normal matrix (inverse transpose of model matrix)
     */
    void setMat3(const std::string& name, const glm::mat3& value) const;
    
    /**
     * Set a 4x4 matrix uniform.
     * Common uses: model, view, projection matrices
     */
    void setMat4(const std::string& name, const glm::mat4& value) const;
    
private:
    unsigned int m_programID;
    
    /**
     * Read shader source code from a file.
     */
    std::string readFile(const std::string& filepath) const;
    
    /**
     * Compile a single shader (vertex or fragment).
     * @return Shader object ID, or 0 on failure
     */
    unsigned int compileShader(unsigned int type, const std::string& source) const;
    
    /**
     * Link vertex and fragment shaders into a program.
     * @return Program ID, or 0 on failure
     */
    unsigned int linkProgram(unsigned int vertexShader, unsigned int fragmentShader) const;
    
    /**
     * Get uniform location with caching.
     * Note: In a production implementation, locations would be cached
     * in a std::unordered_map for performance.
     */
    int getUniformLocation(const std::string& name) const;
};

#endif // SHADER_H
