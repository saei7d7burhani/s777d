/**
 * =============================================================================
 * Model.h - 3D Model Container
 * =============================================================================
 * A Model is a collection of meshes that together represent a complete 3D
 * object. For example, a car model might contain separate meshes for the
 * body, wheels, windows, interior, etc.
 * 
 * This class handles:
 * - Hierarchical mesh organization
 * - Transform management (position, rotation, scale)
 * - Rendering all meshes with a single draw call
 * 
 * Design Decision: Composition over inheritance - Model contains Meshes
 * rather than being a special type of Mesh. This allows flexible organization
 * of complex objects.
 * =============================================================================
 */

#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Material.h"

class Shader;

/**
 * Model class - Container for multiple meshes with transform.
 * 
 * Usage:
 *   Model model;
 *   model.addMesh(std::make_unique<Mesh>(...));
 *   model.setPosition(glm::vec3(0, 0, 0));
 *   model.draw(shader);
 */
class Model {
public:
    /**
     * Default constructor - creates an empty model at origin.
     */
    Model();
    
    /**
     * Constructor with name.
     * @param name Identifier for the model
     */
    explicit Model(const std::string& name);
    
    /**
     * Virtual destructor for inheritance.
     */
    virtual ~Model();
    
    // Allow moving
    Model(Model&& other) noexcept;
    Model& operator=(Model&& other) noexcept;
    
    // =========================================================================
    // Mesh Management
    // =========================================================================
    
    /**
     * Add a mesh to this model.
     * Ownership is transferred to the model.
     */
    void addMesh(std::unique_ptr<Mesh> mesh);
    
    /**
     * Add a mesh with associated material.
     */
    void addMesh(std::unique_ptr<Mesh> mesh, const Material& material);
    
    /**
     * Get the number of meshes in this model.
     */
    size_t getMeshCount() const { return m_meshes.size(); }
    
    /**
     * Get a mesh by index.
     */
    Mesh* getMesh(size_t index);
    const Mesh* getMesh(size_t index) const;
    
    // =========================================================================
    // Transform Operations
    // =========================================================================
    
    /**
     * Set the model's world position.
     */
    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const { return m_position; }
    
    /**
     * Set the model's rotation (Euler angles in degrees).
     */
    void setRotation(const glm::vec3& rotation);
    glm::vec3 getRotation() const { return m_rotation; }
    
    /**
     * Set the model's scale.
     */
    void setScale(const glm::vec3& scale);
    void setScale(float uniformScale);
    glm::vec3 getScale() const { return m_scale; }
    
    /**
     * Get the model matrix (combines position, rotation, scale).
     * This transforms from model space to world space.
     * 
     * Order of transformations (applied right to left):
     * M = Translation * RotationZ * RotationY * RotationX * Scale
     */
    glm::mat4 getModelMatrix() const;
    
    // =========================================================================
    // Rendering
    // =========================================================================
    
    /**
     * Draw all meshes with the given shader.
     * Sets the model matrix uniform before drawing.
     */
    virtual void draw(Shader& shader) const;
    
    /**
     * Draw with a specific model matrix (for hierarchical transforms).
     */
    virtual void draw(Shader& shader, const glm::mat4& parentTransform) const;
    
    // =========================================================================
    // Properties
    // =========================================================================
    
    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }
    
    /**
     * Set the material for all meshes.
     */
    void setMaterial(const Material& material);
    Material& getMaterial() { return m_material; }
    const Material& getMaterial() const { return m_material; }
    
protected:
    std::string m_name;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<Material> m_meshMaterials;  // Per-mesh materials
    Material m_material;                     // Default material
    
    // Transform components
    glm::vec3 m_position;
    glm::vec3 m_rotation;   // Euler angles in degrees
    glm::vec3 m_scale;
    
    bool m_visible;
    
    // Cached model matrix (marked mutable for lazy evaluation)
    mutable glm::mat4 m_modelMatrix;
    mutable bool m_modelMatrixDirty;
    
    /**
     * Update the cached model matrix.
     */
    void updateModelMatrix() const;
};

#endif // MODEL_H
