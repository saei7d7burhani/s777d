/**
 * =============================================================================
 * main.cpp - Application Entry Point
 * =============================================================================
 * 
 * 3D Car Showroom - OpenGL Educational Example
 * 
 * This application demonstrates:
 * - OpenGL 3.3 Core Profile rendering
 * - Object-oriented architecture for 3D graphics
 * - Multiple camera modes (free-roam, orbit, first-person)
 * - Phong/Blinn-Phong lighting model
 * - Transparent object rendering
 * - Basic animations and input handling
 * - Simple collision detection
 * 
 * =============================================================================
 * RENDERING PIPELINE OVERVIEW
 * =============================================================================
 * 
 * 1. APPLICATION LOOP
 *    - Process input (keyboard, mouse)
 *    - Update game state (animations, physics)
 *    - Render frame
 *    - Swap buffers
 * 
 * 2. RENDERING PASS
 *    a) Clear buffers (color, depth)
 *    b) Set camera matrices (view, projection)
 *    c) Configure lighting
 *    d) Render opaque objects (any order, depth testing handles visibility)
 *    e) Sort transparent objects back-to-front
 *    f) Render transparent objects with blending
 * 
 * 3. PER-OBJECT RENDERING
 *    a) Bind shader program
 *    b) Set model matrix uniform
 *    c) Set material uniforms
 *    d) Bind VAO (vertex array object)
 *    e) Issue draw call (glDrawElements)
 * 
 * =============================================================================
 * CAMERA MATH
 * =============================================================================
 * 
 * VIEW MATRIX (transforms world to camera space):
 *   V = lookAt(eye, center, up)
 *   
 *   Where:
 *   - eye: camera position in world space
 *   - center: point the camera is looking at
 *   - up: world up vector (usually Y-axis)
 *   
 *   The lookAt function creates a matrix that:
 *   1. Translates the world so camera is at origin
 *   2. Rotates so camera looks down -Z axis
 * 
 * PROJECTION MATRIX (transforms camera to clip space):
 *   P = perspective(fov, aspect, near, far)
 *   
 *   Where:
 *   - fov: field of view angle (determines zoom)
 *   - aspect: width/height ratio (prevents distortion)
 *   - near/far: clipping plane distances
 *   
 *   This creates a frustum (truncated pyramid) that maps to NDC cube [-1,1]³
 * 
 * MODEL MATRIX (transforms model to world space):
 *   M = Translation * Rotation * Scale
 *   
 *   Applied right-to-left:
 *   1. Scale the model
 *   2. Rotate around origin
 *   3. Translate to world position
 * 
 * FINAL TRANSFORMATION:
 *   gl_Position = P * V * M * vertex_position
 * 
 * =============================================================================
 * LIGHTING MODEL (Blinn-Phong)
 * =============================================================================
 * 
 * AMBIENT:
 *   ambient = lightAmbient * materialAmbient
 *   - Constant illumination to simulate indirect light
 *   - Prevents completely black shadows
 * 
 * DIFFUSE:
 *   diffuse = lightDiffuse * materialDiffuse * max(dot(N, L), 0)
 *   - N: surface normal (perpendicular to surface)
 *   - L: direction from surface to light
 *   - Brightest when surface directly faces light (N parallel to L)
 *   - Creates the main surface color
 * 
 * SPECULAR:
 *   H = normalize(L + V)  // Halfway vector (Blinn optimization)
 *   specular = lightSpecular * materialSpecular * pow(max(dot(N, H), 0), shininess)
 *   - V: direction from surface to viewer
 *   - H: halfway between light and view directions
 *   - Creates shiny highlights
 *   - shininess controls highlight size (higher = smaller, sharper)
 * 
 * FINAL COLOR:
 *   color = ambient + diffuse + specular
 * 
 * ATTENUATION (for point/spot lights):
 *   attenuation = 1.0 / (constant + linear*d + quadratic*d²)
 *   - d: distance from light to fragment
 *   - Makes lights fade with distance (inverse square law)
 * 
 * =============================================================================
 * DESIGN DECISIONS
 * =============================================================================
 * 
 * 1. OBJECT-ORIENTED ARCHITECTURE
 *    - Separates concerns (Window, Shader, Camera, Model, etc.)
 *    - Uses RAII for resource management (GPU resources freed in destructors)
 *    - Allows easy extension and modification
 * 
 * 2. COMPONENT-BASED SCENE
 *    - Scene contains Models, which contain Meshes
 *    - Each level handles its own transforms
 *    - Hierarchical transforms allow parent-child relationships
 * 
 * 3. RENDER QUEUE SYSTEM
 *    - Collects render commands during frame
 *    - Sorts transparent objects for correct blending
 *    - Could be extended for batching similar objects
 * 
 * 4. EMBEDDED SHADERS
 *    - Shaders are embedded in C++ for simplicity
 *    - Could be loaded from files for easier iteration
 *    - Uses #version 330 for OpenGL 3.3 compatibility
 * 
 * 5. SIMPLE COLLISION
 *    - AABB (Axis-Aligned Bounding Boxes) for walls
 *    - Sufficient for keeping car in bounds
 *    - Could be extended with physics library
 * 
 * =============================================================================
 */

#include "Application.h"
#include <iostream>
#include <exception>

/**
 * Main entry point.
 * 
 * Creates and runs the car showroom application.
 * Catches and reports any exceptions that escape.
 */
int main() {
    try {
        std::cout << "=== OpenGL 3D Car Showroom ===" << std::endl;
        std::cout << "Educational Example Project" << std::endl;
        std::cout << "=============================" << std::endl;
        
        Application app(1280, 720, "3D Car Showroom - OpenGL Example");
        return app.run();
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception" << std::endl;
        return 1;
    }
}
