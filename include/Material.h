/**
 * =============================================================================
 * Material.h - Surface Material Properties
 * =============================================================================
 * Materials define how surfaces interact with light. Combined with the
 * lighting model, materials determine the final appearance of objects.
 * 
 * Phong Material Components:
 * - Ambient: Color when in shadow (simulates indirect illumination)
 * - Diffuse: Main surface color (matte reflection)
 * - Specular: Highlight color (shiny reflection)
 * - Shininess: How focused the specular highlight is (higher = smaller, sharper)
 * 
 * Common Shininess Values:
 * - 2-10: Very rough surfaces (rubber, chalk)
 * - 10-50: Moderately shiny (plastic, wood)
 * - 50-128: Shiny (metal, polished surfaces)
 * - 128-256: Very shiny (chrome, mirrors)
 * =============================================================================
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include <string>

class Shader;

/**
 * Material class - Defines surface properties for lighting.
 */
class Material {
public:
    /**
     * Default constructor - Creates a white matte material.
     */
    Material();
    
    /**
     * Create a material with specified colors.
     */
    Material(const glm::vec3& ambient,
             const glm::vec3& diffuse,
             const glm::vec3& specular,
             float shininess);
    
    // Material color components
    glm::vec3 ambient;      // Ambient reflection color
    glm::vec3 diffuse;      // Diffuse reflection color (main color)
    glm::vec3 specular;     // Specular reflection color
    float shininess;        // Specular exponent (focus of highlight)
    
    // Transparency
    float opacity;          // 1.0 = fully opaque, 0.0 = fully transparent
    
    // Texture IDs (0 = no texture)
    unsigned int diffuseMap;
    unsigned int specularMap;
    unsigned int normalMap;
    
    /**
     * Apply material properties to a shader.
     */
    void applyToShader(Shader& shader, const std::string& uniformName = "material") const;
    
    /**
     * Check if this material is transparent.
     */
    bool isTransparent() const { return opacity < 1.0f; }
    
    // =========================================================================
    // Preset Materials
    // =========================================================================
    
    static Material Default();
    static Material Chrome();
    static Material Gold();
    static Material Silver();
    static Material Bronze();
    static Material Copper();
    static Material Brass();
    static Material Emerald();
    static Material Ruby();
    static Material Pearl();
    static Material Obsidian();
    static Material Turquoise();
    static Material Jade();
    
    // Car-specific materials
    static Material CarPaintRed();
    static Material CarPaintBlue();
    static Material CarPaintBlack();
    static Material CarPaintWhite();
    static Material CarPaintSilver();
    static Material Glass();
    static Material GlassTinted();
    static Material Rubber();
    static Material Leather();
    static Material DashboardPlastic();
    static Material HeadlightGlass();
    
    // Environment materials
    static Material Concrete();
    static Material Tile();
    static Material Metal();
    static Material Wood();
};

#endif // MATERIAL_H
