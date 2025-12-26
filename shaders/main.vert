/**
 * =============================================================================
 * Vertex Shader - main.vert
 * =============================================================================
 * 
 * This shader transforms vertices from model space to clip space and prepares
 * data for the fragment shader's lighting calculations.
 * 
 * Transformation Pipeline:
 * Model Space -> World Space -> View Space -> Clip Space -> NDC -> Screen Space
 *               (model mat)   (view mat)    (proj mat)   (perspective div)
 * 
 * =============================================================================
 */

#version 330 core

// =============================================================================
// Vertex Attributes (Input)
// =============================================================================
// These are per-vertex data uploaded from the CPU via VBOs.
// The layout(location = N) must match the glVertexAttribPointer calls.

layout (location = 0) in vec3 aPos;       // Position in model space
layout (location = 1) in vec3 aNormal;    // Normal vector in model space
layout (location = 2) in vec2 aTexCoords; // Texture coordinates (UV)

// =============================================================================
// Output to Fragment Shader
// =============================================================================
// These are interpolated across the primitive (triangle) for each fragment.

out vec3 FragPos;     // Position in world space (for lighting distance calc)
out vec3 Normal;      // Normal in world space (for lighting direction calc)
out vec2 TexCoords;   // Texture coordinates (passed through)

// =============================================================================
// Uniforms
// =============================================================================
// These are constant for all vertices in a single draw call.
// They are set by the CPU before drawing.

uniform mat4 model;         // Model matrix: model space -> world space
uniform mat4 view;          // View matrix: world space -> view space
uniform mat4 projection;    // Projection matrix: view space -> clip space
uniform mat3 normalMatrix;  // Normal matrix: correctly transforms normals

// =============================================================================
// Main Vertex Shader Function
// =============================================================================

void main() {
    // -------------------------------------------------------------------------
    // Calculate World Space Position
    // -------------------------------------------------------------------------
    // Multiply the vertex position by the model matrix to get world position.
    // The w component (1.0) is needed for translation to work correctly.
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // -------------------------------------------------------------------------
    // Transform Normal to World Space
    // -------------------------------------------------------------------------
    // Normals must be transformed using the normal matrix, NOT the model matrix.
    // 
    // Why? If the model has non-uniform scaling (e.g., stretched in one axis),
    // simply using the model matrix would distort the normals.
    // 
    // Normal Matrix = transpose(inverse(mat3(model)))
    // This preserves perpendicularity under non-uniform scaling.
    //
    // Note: We're not normalizing here because the fragment shader will
    // normalize anyway (interpolation can denormalize).
    Normal = normalMatrix * aNormal;
    
    // -------------------------------------------------------------------------
    // Pass Through Texture Coordinates
    // -------------------------------------------------------------------------
    // Texture coordinates are typically in range [0, 1] and don't need
    // transformation.
    TexCoords = aTexCoords;
    
    // -------------------------------------------------------------------------
    // Calculate Final Clip Space Position
    // -------------------------------------------------------------------------
    // Apply the full MVP (Model-View-Projection) transformation.
    // This outputs a 4D homogeneous coordinate in clip space.
    // 
    // OpenGL will then:
    // 1. Perform perspective division (x/w, y/w, z/w) to get NDC
    // 2. Perform viewport transform to get screen coordinates
    // 3. Perform depth test and rasterization
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
