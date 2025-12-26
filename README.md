# 3D Car Showroom - OpenGL Educational Example

An interactive 3D car showroom demonstrating modern OpenGL (3.3 Core Profile) programming with C++17. This project is designed for educational purposes, prioritizing clarity and comprehensive documentation over optimization.

## Features

### Rendering
- **OpenGL 3.3 Core Profile** with GLSL 330 shaders
- **Blinn-Phong lighting** with ambient, diffuse, and specular components
- **Multiple light types**: Directional, Point, and Spot lights
- **Transparency rendering** with proper back-to-front sorting
- **Material system** with presets for car paint, glass, rubber, metal, etc.

### Scene
- **Detailed main car** with body, wheels, windows, and interior
- **Simplified placeholder cars** around the showroom
- **Complete showroom environment**: floor, walls, ceiling, display platform
- **Collision detection** to keep objects within bounds

### Interaction
- **Multiple camera modes**:
  - Free-roam (FPS-style movement)
  - Orbit (rotate around the car)
  - Driver seat (first-person inside the car)
- **Animated car elements**:
  - Wheel rotation
  - Door opening/closing
  - Headlight toggle
- **Keyboard + Mouse controls**

## Project Structure

```
CarShowroom/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── include/                    # Header files
│   ├── glad/
│   │   └── glad.h              # OpenGL loader
│   ├── KHR/
│   │   └── khrplatform.h       # Platform types
│   ├── stb_image.h             # Image loading (simplified)
│   ├── Animation.h             # Animation system
│   ├── Application.h           # Main application
│   ├── Camera.h                # Camera system
│   ├── CarModel.h              # Car with animations
│   ├── Collision.h             # Collision detection
│   ├── Input.h                 # Input handling
│   ├── Light.h                 # Light types
│   ├── Material.h              # Material properties
│   ├── Mesh.h                  # Mesh and primitives
│   ├── Model.h                 # Model container
│   ├── Renderer.h              # Rendering system
│   ├── Shader.h                # Shader management
│   ├── ShowroomScene.h         # Scene management
│   └── Window.h                # Window management
├── src/                        # Source files
│   ├── glad.c                  # OpenGL loader implementation
│   ├── Animation.cpp
│   ├── Application.cpp
│   ├── Camera.cpp
│   ├── CarModel.cpp
│   ├── Collision.cpp
│   ├── Input.cpp
│   ├── Light.cpp
│   ├── main.cpp                # Entry point
│   ├── Material.cpp
│   ├── Mesh.cpp
│   ├── Model.cpp
│   ├── Renderer.cpp
│   ├── Shader.cpp
│   ├── ShowroomScene.cpp
│   └── Window.cpp
└── shaders/                    # GLSL shaders
    ├── main.vert               # Vertex shader
    └── main.frag               # Fragment shader
```

## Dependencies

- **C++17** compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **OpenGL 3.3+** compatible GPU and drivers
- **GLFW 3.3+** - Window and input management
- **GLM** - Mathematics library (header-only)
- **CMake 3.16+** - Build system

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt install build-essential cmake libglfw3-dev libglm-dev
```

**Fedora:**
```bash
sudo dnf install gcc-c++ cmake glfw-devel glm-devel
```

**macOS (Homebrew):**
```bash
brew install cmake glfw glm
```

**Windows (vcpkg):**
```bash
vcpkg install glfw3 glm
```

## Building

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./CarShowroom
```

For Debug build with symbols:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

For Release build with optimizations:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

## Controls

| Key | Action |
|-----|--------|
| W/S or ↑/↓ | Move forward/backward |
| A/D or ←/→ | Move left/right |
| Space | Move up |
| Ctrl | Move down |
| Mouse | Look around |
| Scroll | Zoom |
| 1 | Free-roam camera |
| 2 | Orbit camera |
| 3 | Driver seat camera |
| I/K | Move car forward/backward |
| J/L | Turn car left/right |
| O | Toggle door |
| H | Toggle headlights |
| R | Reset car position |
| Escape | Release cursor / Exit |

## Architecture Overview

### Rendering Pipeline

```
┌─────────────────────────────────────────────────────────────────┐
│                     Application Loop                             │
├─────────────────────────────────────────────────────────────────┤
│  1. Process Input (keyboard, mouse)                              │
│  2. Update State (animations, physics)                           │
│  3. Render Frame                                                 │
│  4. Swap Buffers                                                 │
└─────────────────────────────────────────────────────────────────┘
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                     Render Pass                                  │
├─────────────────────────────────────────────────────────────────┤
│  a) Clear buffers (color, depth)                                 │
│  b) Set camera matrices (view, projection)                       │
│  c) Configure lighting                                           │
│  d) Render opaque objects                                        │
│  e) Sort transparent objects (back-to-front)                     │
│  f) Render transparent objects                                   │
└─────────────────────────────────────────────────────────────────┘
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                     Per-Object Rendering                         │
├─────────────────────────────────────────────────────────────────┤
│  1. Bind shader program                                          │
│  2. Set uniforms (matrices, material, lights)                    │
│  3. Bind VAO                                                     │
│  4. Draw call (glDrawElements)                                   │
└─────────────────────────────────────────────────────────────────┘
```

### Class Hierarchy

```
Application
├── Window          (GLFW window management)
├── Renderer        (OpenGL state, draw calls)
│   └── Shader      (GLSL program)
├── Camera          (View/projection matrices)
├── Input           (Keyboard/mouse handling)
└── ShowroomScene
    ├── CarModel    (Main detailed car)
    │   ├── Mesh    (Body)
    │   ├── Mesh    (Wheels × 4)
    │   ├── Mesh    (Windows)
    │   └── Mesh    (Interior)
    ├── CarModel[]  (Background cars)
    ├── Model[]     (Environment)
    ├── Light[]     (Lighting setup)
    └── CollisionWorld
```

### Camera Math

**View Matrix** (world → camera space):
```
V = lookAt(eye, center, up)
```
- Translates world so camera is at origin
- Rotates so camera looks down -Z axis

**Projection Matrix** (camera → clip space):
```
P = perspective(fov, aspect, near, far)
```
- Creates perspective frustum
- Maps to NDC (Normalized Device Coordinates)

**Model Matrix** (model → world space):
```
M = Translation × Rotation × Scale
```
- Applied right-to-left

**Final Transformation**:
```
gl_Position = P × V × M × vertex
```

### Blinn-Phong Lighting

```
Final Color = Ambient + Diffuse + Specular

Ambient  = lightAmbient × materialAmbient
Diffuse  = lightDiffuse × materialDiffuse × max(dot(N, L), 0)
Specular = lightSpecular × materialSpecular × pow(max(dot(N, H), 0), shininess)

Where:
  N = surface normal
  L = light direction
  V = view direction
  H = normalize(L + V)  // Halfway vector
```

**Attenuation** (point/spot lights):
```
attenuation = 1 / (constant + linear×d + quadratic×d²)
```

## Educational Notes

### Key OpenGL Concepts Demonstrated

1. **Vertex Array Objects (VAO)**: Store vertex attribute configurations
2. **Vertex Buffer Objects (VBO)**: Store vertex data on GPU
3. **Element Buffer Objects (EBO)**: Store indices for indexed rendering
4. **Shader Programs**: Vertex and fragment shaders linked together
5. **Uniforms**: Per-draw-call constants (matrices, materials, lights)
6. **Attributes**: Per-vertex data (position, normal, texture coords)

### Design Decisions

1. **Object-Oriented**: Each component (Window, Shader, Camera, etc.) has clear responsibilities
2. **RAII**: GPU resources are freed in destructors
3. **Render Queue**: Commands are collected, sorted, then executed
4. **Embedded Shaders**: Shaders are in C++ strings for simplicity (could be external files)
5. **Simple Collision**: AABB (Axis-Aligned Bounding Boxes) for walls

### Extensions You Could Add

1. **Textures**: Add actual texture loading with stb_image
2. **Shadow Mapping**: Add shadows for lights
3. **Model Loading**: Load OBJ/glTF models
4. **Skybox**: Add a cubemap skybox
5. **Post-Processing**: Add bloom, ambient occlusion, tone mapping
6. **Reflections**: Add screen-space or cubemap reflections
7. **Particles**: Add particle effects (dust, sparks)
8. **Physics**: Integrate a physics library (Bullet, PhysX)

## License

This project is provided for educational purposes. Feel free to use, modify, and learn from it.

## References

- [Learn OpenGL](https://learnopengl.com/) - Comprehensive OpenGL tutorials
- [OpenGL Reference](https://www.khronos.org/opengl/) - Official OpenGL specification
- [GLM Documentation](https://glm.g-truc.net/) - Mathematics library
- [GLFW Documentation](https://www.glfw.org/docs/) - Window library
