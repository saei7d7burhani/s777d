/**
 * =============================================================================
 * Fragment Shader - main.frag
 * =============================================================================
 * 
 * This shader calculates the final color of each fragment (potential pixel)
 * using the Blinn-Phong lighting model with support for multiple light types.
 * 
 * Lighting Model:
 * Final Color = Ambient + Diffuse + Specular
 * 
 * Each light type (directional, point, spot) contributes to the final color
 * with different characteristics.
 * 
 * =============================================================================
 */

#version 330 core

// =============================================================================
// Fragment Output
// =============================================================================

out vec4 FragColor;   // Final color (RGBA) written to the framebuffer

// =============================================================================
// Input from Vertex Shader
// =============================================================================
// These values are interpolated across the triangle from the vertex values.

in vec3 FragPos;      // Fragment position in world space
in vec3 Normal;       // Interpolated normal (needs renormalization!)
in vec2 TexCoords;    // Texture coordinates

// =============================================================================
// Structures
// =============================================================================

/**
 * Material - Defines how a surface interacts with light.
 * 
 * ambient:   Color reflected under ambient lighting (indirect illumination)
 * diffuse:   Main surface color (matte reflection)
 * specular:  Highlight color (shiny reflection)
 * shininess: Focus of specular highlight (higher = smaller, sharper)
 * opacity:   Transparency (1.0 = opaque, 0.0 = invisible)
 */
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float opacity;
};

/**
 * DirectionalLight - Light with parallel rays (like the sun).
 * 
 * Has no position, only direction. All fragments receive light from
 * the same direction, regardless of their position.
 * No attenuation (light doesn't fade with distance).
 */
struct DirLight {
    bool enabled;
    vec3 direction;   // Direction the light is shining (TO the scene)
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

/**
 * PointLight - Light emanating from a point in all directions.
 * 
 * Has position but no direction. Light radiates outward uniformly.
 * Attenuates with distance using: 1 / (c + l*d + q*d²)
 */
struct PointLight {
    bool enabled;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;   // Attenuation constant term
    float linear;     // Attenuation linear term
    float quadratic;  // Attenuation quadratic term
};

/**
 * SpotLight - Cone of light from a point (like a flashlight).
 * 
 * Has both position and direction. Light is strongest within the
 * inner cone and fades to zero at the outer cone edge.
 * Also attenuates with distance.
 */
struct SpotLight {
    bool enabled;
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;       // Cosine of inner cone angle
    float outerCutOff;  // Cosine of outer cone angle
    float constant;
    float linear;
    float quadratic;
};

// =============================================================================
// Uniforms
// =============================================================================

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 2

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int numPointLights;
uniform int numSpotLights;
uniform vec3 viewPos;   // Camera position in world space

// =============================================================================
// Function Declarations
// =============================================================================

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// =============================================================================
// Main Fragment Shader Function
// =============================================================================

void main() {
    // -------------------------------------------------------------------------
    // Prepare Lighting Inputs
    // -------------------------------------------------------------------------
    
    // Renormalize the interpolated normal
    // Interpolation across a triangle can denormalize vectors
    vec3 norm = normalize(Normal);
    
    // Calculate view direction (from fragment to camera)
    // Used for specular reflection calculation
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // -------------------------------------------------------------------------
    // Accumulate Light Contributions
    // -------------------------------------------------------------------------
    
    vec3 result = vec3(0.0);
    
    // Directional light contribution
    if (dirLight.enabled) {
        result += CalcDirLight(dirLight, norm, viewDir);
    }
    
    // Point light contributions
    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; i++) {
        if (pointLights[i].enabled) {
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
        }
    }
    
    // Spot light contributions
    for (int i = 0; i < numSpotLights && i < MAX_SPOT_LIGHTS; i++) {
        if (spotLights[i].enabled) {
            result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
        }
    }
    
    // -------------------------------------------------------------------------
    // Output Final Color
    // -------------------------------------------------------------------------
    
    FragColor = vec4(result, material.opacity);
}

// =============================================================================
// Directional Light Calculation
// =============================================================================
/**
 * Calculate contribution from a directional light.
 * 
 * @param light The directional light
 * @param normal Surface normal (normalized)
 * @param viewDir Direction to camera (normalized)
 * @return RGB color contribution from this light
 */
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    // Light direction is opposite of the "direction" uniform
    // (uniform points TO scene, we need FROM surface TO light)
    vec3 lightDir = normalize(-light.direction);
    
    // -------------------------------------------------------------------------
    // Ambient Component
    // -------------------------------------------------------------------------
    // Constant illumination regardless of surface orientation.
    // Simulates light bouncing around the environment.
    vec3 ambient = light.ambient * material.ambient;
    
    // -------------------------------------------------------------------------
    // Diffuse Component (Lambertian Reflection)
    // -------------------------------------------------------------------------
    // Brightest when surface directly faces the light (normal parallel to lightDir).
    // Based on Lambert's cosine law: intensity ∝ cos(θ) = dot(N, L)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // -------------------------------------------------------------------------
    // Specular Component (Blinn-Phong)
    // -------------------------------------------------------------------------
    // Creates shiny highlights where light would reflect toward the viewer.
    // 
    // Blinn-Phong uses the halfway vector (between light and view directions)
    // instead of the reflection vector. It's cheaper to compute and looks similar.
    // 
    // Original Phong:  spec = pow(max(dot(R, V), 0), shininess)
    // Blinn-Phong:     spec = pow(max(dot(N, H), 0), shininess)
    //
    // H = halfway vector between L and V
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    
    return ambient + diffuse + specular;
}

// =============================================================================
// Point Light Calculation
// =============================================================================
/**
 * Calculate contribution from a point light.
 * 
 * @param light The point light
 * @param normal Surface normal (normalized)
 * @param fragPos Fragment position in world space
 * @param viewDir Direction to camera (normalized)
 * @return RGB color contribution from this light
 */
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    // Light direction: from fragment to light
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    
    // -------------------------------------------------------------------------
    // Attenuation
    // -------------------------------------------------------------------------
    // Point lights fade with distance following the inverse square law.
    // We use a modified formula for artistic control:
    // attenuation = 1 / (constant + linear * d + quadratic * d²)
    //
    // - constant: ensures we don't divide by zero, typically 1.0
    // - linear: gentle distance falloff
    // - quadratic: realistic inverse-square falloff
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                               light.quadratic * distance * distance);
    
    // Apply attenuation to all components
    return (ambient + diffuse + specular) * attenuation;
}

// =============================================================================
// Spot Light Calculation
// =============================================================================
/**
 * Calculate contribution from a spot light.
 * 
 * @param light The spot light
 * @param normal Surface normal (normalized)
 * @param fragPos Fragment position in world space
 * @param viewDir Direction to camera (normalized)
 * @return RGB color contribution from this light
 */
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    
    // -------------------------------------------------------------------------
    // Spotlight Cone Intensity
    // -------------------------------------------------------------------------
    // Calculate angle between spotlight direction and fragment direction.
    // Use cosine because it's what we stored and avoids expensive acos().
    //
    // theta: cosine of angle between spotlight direction and lightDir
    // If theta > cutOff (inside inner cone): full intensity
    // If theta < outerCutOff (outside outer cone): zero intensity
    // In between: smooth fade (soft edges)
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // -------------------------------------------------------------------------
    // Attenuation
    // -------------------------------------------------------------------------
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                               light.quadratic * distance * distance);
    
    // Apply spotlight intensity to diffuse and specular (not ambient)
    // Ambient should still be visible even outside the spotlight cone
    return (ambient + (diffuse + specular) * intensity) * attenuation;
}
