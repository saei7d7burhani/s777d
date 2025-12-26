/**
 * =============================================================================
 * Light.h - Lighting System
 * =============================================================================
 * Implements three types of lights commonly used in 3D graphics:
 * 
 * 1. Directional Light: Simulates distant light sources like the sun.
 *    - Parallel rays, no position (only direction)
 *    - No attenuation (intensity doesn't decrease with distance)
 * 
 * 2. Point Light: Emits light in all directions from a point.
 *    - Has position but no direction
 *    - Attenuates with distance (inverse square law, approximated)
 * 
 * 3. Spot Light: Cone of light from a point.
 *    - Has both position and direction
 *    - Has cutoff angles (inner and outer) for smooth falloff
 *    - Attenuates with distance
 * 
 * Lighting Calculation (Phong Model):
 * -----------------------------------
 * Final Color = Ambient + Diffuse + Specular
 * 
 * Ambient = lightAmbient * materialAmbient
 *   - Constant illumination to simulate indirect light
 * 
 * Diffuse = lightDiffuse * materialDiffuse * max(dot(N, L), 0)
 *   - N = surface normal, L = direction to light
 *   - Brightest when surface faces the light directly
 * 
 * Specular = lightSpecular * materialSpecular * pow(max(dot(R, V), 0), shininess)
 *   - R = reflection of light direction, V = direction to viewer
 *   - Creates shiny highlights
 * =============================================================================
 */

#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <string>

class Shader;

/**
 * Base Light class - Common properties for all light types.
 */
class Light {
public:
    Light();
    virtual ~Light() = default;
    
    // Light color components
    glm::vec3 ambient;      // Ambient contribution
    glm::vec3 diffuse;      // Diffuse contribution  
    glm::vec3 specular;     // Specular contribution
    
    // Light state
    bool enabled;
    
    /**
     * Apply this light's properties to a shader.
     * @param shader The shader program
     * @param uniformName Base name for the uniform (e.g., "dirLight" or "pointLights[0]")
     */
    virtual void applyToShader(Shader& shader, const std::string& uniformName) const = 0;
};

/**
 * Directional Light - Simulates distant light sources like the sun.
 * 
 * Key property: direction (not position)
 * The direction points FROM the light TO the scene.
 */
class DirectionalLight : public Light {
public:
    DirectionalLight();
    DirectionalLight(const glm::vec3& direction, 
                     const glm::vec3& ambient,
                     const glm::vec3& diffuse,
                     const glm::vec3& specular);
    
    glm::vec3 direction;    // Direction the light is shining
    
    void applyToShader(Shader& shader, const std::string& uniformName) const override;
};

/**
 * Point Light - Emits light in all directions from a point.
 * 
 * Attenuation formula: 1.0 / (constant + linear * d + quadratic * d^2)
 * where d = distance from light to fragment
 * 
 * Common attenuation values for different ranges:
 * Distance | Constant | Linear  | Quadratic
 * ---------|----------|---------|----------
 *    7     |   1.0    |  0.7    |   1.8
 *   13     |   1.0    |  0.35   |   0.44
 *   20     |   1.0    |  0.22   |   0.20
 *   32     |   1.0    |  0.14   |   0.07
 *   50     |   1.0    |  0.09   |   0.032
 *  100     |   1.0    |  0.045  |   0.0075
 */
class PointLight : public Light {
public:
    PointLight();
    PointLight(const glm::vec3& position,
               const glm::vec3& ambient,
               const glm::vec3& diffuse,
               const glm::vec3& specular,
               float constant = 1.0f,
               float linear = 0.09f,
               float quadratic = 0.032f);
    
    glm::vec3 position;     // World position
    
    // Attenuation factors
    float constant;         // Usually 1.0
    float linear;           // Linear falloff
    float quadratic;        // Quadratic falloff
    
    void applyToShader(Shader& shader, const std::string& uniformName) const override;
    
    /**
     * Set attenuation for a specific range.
     * @param range Approximate distance where light intensity becomes negligible
     */
    void setRange(float range);
};

/**
 * Spot Light - Cone of light from a point.
 * 
 * Cutoff angles define the cone:
 * - Inner cutoff: Full intensity within this angle
 * - Outer cutoff: Light fades to zero at this angle
 * 
 * Smooth falloff is calculated using:
 * intensity = (theta - outerCutoff) / (innerCutoff - outerCutoff)
 * where theta = angle between spotlight direction and fragment direction
 */
class SpotLight : public Light {
public:
    SpotLight();
    SpotLight(const glm::vec3& position,
              const glm::vec3& direction,
              const glm::vec3& ambient,
              const glm::vec3& diffuse,
              const glm::vec3& specular,
              float innerCutoff = 12.5f,
              float outerCutoff = 17.5f);
    
    glm::vec3 position;     // World position
    glm::vec3 direction;    // Direction spotlight is pointing
    
    // Cutoff angles (in degrees, stored as cosine for shader)
    float innerCutoff;      // Angle of inner cone
    float outerCutoff;      // Angle of outer cone
    
    // Attenuation (same as point light)
    float constant;
    float linear;
    float quadratic;
    
    void applyToShader(Shader& shader, const std::string& uniformName) const override;
    
    /**
     * Set cutoff angles in degrees.
     */
    void setCutoff(float innerDegrees, float outerDegrees);
};

#endif // LIGHT_H
