# =============================================================================
# OpenGL 3D Car Showroom - CMake Build Configuration
# =============================================================================
# This CMakeLists.txt configures the build for a C++17 OpenGL application
# demonstrating a 3D interactive car showroom with multiple camera modes,
# Phong/Blinn-Phong lighting, and animated elements.
#
# Build Instructions:
#   mkdir build && cd build
#   cmake ..
#   cmake --build .
# =============================================================================

cmake_minimum_required(VERSION 3.16)
project(CarShowroom VERSION 1.0.0 LANGUAGES C CXX)

# =============================================================================
# C++ Standard Configuration
# =============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =============================================================================
# Build Type Configuration
# =============================================================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# =============================================================================
# Compiler Flags
# =============================================================================
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# =============================================================================
# Find Required Packages
# =============================================================================
# GLFW - Window and Input Management
find_package(glfw3 3.3 QUIET)
if(NOT glfw3_FOUND)
    message(STATUS "GLFW not found via find_package, using pkg-config")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
endif()

# OpenGL
find_package(OpenGL REQUIRED)

# GLM - Mathematics Library (header-only)
find_package(glm QUIET)
if(NOT glm_FOUND)
    message(STATUS "GLM not found via find_package, assuming system headers")
endif()

# =============================================================================
# Source Files
# =============================================================================
set(SOURCES
    src/main.cpp
    src/glad.c
    src/Window.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/CarModel.cpp
    src/ShowroomScene.cpp
    src/Renderer.cpp
    src/Input.cpp
    src/Light.cpp
    src/Material.cpp
    src/Animation.cpp
    src/Collision.cpp
    src/Application.cpp
)

set(HEADERS
    include/glad/glad.h
    include/KHR/khrplatform.h
    include/stb_image.h
    include/Window.h
    include/Shader.h
    include/Camera.h
    include/Mesh.h
    include/Model.h
    include/CarModel.h
    include/ShowroomScene.h
    include/Renderer.h
    include/Input.h
    include/Light.h
    include/Material.h
    include/Animation.h
    include/Collision.h
    include/Application.h
)

# =============================================================================
# Executable Target
# =============================================================================
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIR}
)

if(GLFW3_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW3_INCLUDE_DIRS})
endif()

# =============================================================================
# Link Libraries
# =============================================================================
if(glfw3_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW3_LIBRARIES})
    target_link_directories(${PROJECT_NAME} PRIVATE ${GLFW3_LIBRARY_DIRS})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# Platform-specific libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl pthread)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa" "-framework IOKit")
endif()

# =============================================================================
# Copy Shaders to Build Directory
# =============================================================================
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMENT "Copying shaders to build directory"
)

# =============================================================================
# Installation (Optional)
# =============================================================================
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY shaders DESTINATION share/${PROJECT_NAME})

# =============================================================================
# Print Configuration Summary
# =============================================================================
message(STATUS "=== Car Showroom Build Configuration ===")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenGL: ${OPENGL_LIBRARIES}")
