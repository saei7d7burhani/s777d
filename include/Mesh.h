/**
 * =============================================================================
 * Mesh.h - 3D Mesh Representation
 * =============================================================================
 * A mesh represents a collection of vertices, indices, and textures that
 * together form a renderable 3D object. This class handles:
 * 
 * - Vertex data storage (positions, normals, texture coordinates)
 * - GPU buffer management (VAO, VBO, EBO)
 * - Drawing operations
 * 
 * Vertex Layout:
 * Each vertex contains:
 * - Position (vec3): XYZ coordinates in model space
 * - Normal (vec3): Direction perpendicular to surface (for lighting)
 * - TexCoords (vec2): UV coordinates for texture mapping
 * 
 * Design Decision: Using indexed rendering with Element Buffer Objects (EBO)
 * to reduce vertex duplication. Shared vertices only need to be stored once.
 * =============================================================================
 */

#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

class Shader;

/**
 * Vertex structure - Holds all per-vertex data.
 * 
 * Memory layout is important for GPU upload:
 * - Position: 3 floats (12 bytes)
 * - Normal: 3 floats (12 bytes)
 * - TexCoords: 2 floats (8 bytes)
 * Total: 32 bytes per vertex
 */
struct Vertex {
    glm::vec3 Position;     // Vertex position in model space
    glm::vec3 Normal;       // Surface normal for lighting
    glm::vec2 TexCoords;    // Texture coordinates (UV)
    
    // Default constructor
    Vertex() : Position(0.0f), Normal(0.0f, 1.0f, 0.0f), TexCoords(0.0f) {}
    
    // Convenience constructor
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex)
        : Position(pos), Normal(norm), TexCoords(tex) {}
};

/**
 * Texture structure - References a loaded texture.
 */
struct Texture {
    unsigned int id;        // OpenGL texture ID
    std::string type;       // "diffuse", "specular", "normal", etc.
    std::string path;       // Original file path (for deduplication)
};

/**
 * Mesh class - Manages a renderable 3D mesh.
 * 
 * Usage:
 *   std::vector<Vertex> vertices = {...};
 *   std::vector<unsigned int> indices = {...};
 *   Mesh mesh(vertices, indices);
 *   mesh.draw(shader);
 */
class Mesh {
public:
    // Mesh data (public for easy access)
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    
    /**
     * Construct a mesh from vertex and index data.
     * 
     * @param vertices Vector of vertex data
     * @param indices Vector of indices for indexed rendering
     * @param textures Vector of textures (optional)
     */
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<unsigned int>& indices,
         const std::vector<Texture>& textures = {});
    
    /**
     * Destructor - Releases GPU resources.
     */
    ~Mesh();
    
    // Disable copying (GPU resources shouldn't be duplicated)
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    
    // Allow moving
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    
    /**
     * Render the mesh using the provided shader.
     * 
     * @param shader Shader program to use for rendering
     */
    void draw(const Shader& shader) const;
    
    /**
     * Get the VAO ID for external use.
     */
    unsigned int getVAO() const { return m_VAO; }
    
private:
    // OpenGL buffer objects
    unsigned int m_VAO;     // Vertex Array Object - stores vertex attribute configuration
    unsigned int m_VBO;     // Vertex Buffer Object - stores vertex data
    unsigned int m_EBO;     // Element Buffer Object - stores indices
    
    /**
     * Set up the mesh GPU resources.
     * Creates VAO, VBO, EBO and configures vertex attributes.
     */
    void setupMesh();
};

// =============================================================================
// Primitive Mesh Generators
// =============================================================================
// These functions create common shapes for building scenes

namespace MeshGenerator {
    /**
     * Create a cube mesh.
     * @param size Side length of the cube
     * @return Mesh object representing a cube
     */
    Mesh createCube(float size = 1.0f);
    
    /**
     * Create a plane/quad mesh.
     * @param width Width of the plane (X axis)
     * @param depth Depth of the plane (Z axis)
     * @param uScale Texture U coordinate scale
     * @param vScale Texture V coordinate scale
     * @return Mesh object representing a horizontal plane
     */
    Mesh createPlane(float width = 10.0f, float depth = 10.0f, 
                     float uScale = 1.0f, float vScale = 1.0f);
    
    /**
     * Create a sphere mesh.
     * @param radius Sphere radius
     * @param sectors Number of horizontal divisions (longitude)
     * @param stacks Number of vertical divisions (latitude)
     * @return Mesh object representing a sphere
     */
    Mesh createSphere(float radius = 1.0f, int sectors = 36, int stacks = 18);
    
    /**
     * Create a cylinder mesh.
     * @param radius Cylinder radius
     * @param height Cylinder height
     * @param sectors Number of divisions around the circumference
     * @return Mesh object representing a cylinder
     */
    Mesh createCylinder(float radius = 0.5f, float height = 1.0f, int sectors = 36);
    
    /**
     * Create a simple car body mesh.
     * @return Mesh object representing a simplified car body
     */
    Mesh createCarBody();
    
    /**
     * Create a wheel mesh.
     * @param radius Wheel radius
     * @param width Wheel width
     * @return Mesh object representing a wheel
     */
    Mesh createWheel(float radius = 0.4f, float width = 0.2f);
}

#endif // MESH_H
