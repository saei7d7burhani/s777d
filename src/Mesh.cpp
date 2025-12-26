/**
 * =============================================================================
 * Mesh.cpp - 3D Mesh Implementation
 * =============================================================================
 */

#include "Mesh.h"
#include "Shader.h"

#include <glad/glad.h>
#include <cmath>

// =============================================================================
// Constructor / Destructor
// =============================================================================

Mesh::Mesh(const std::vector<Vertex>& verts,
           const std::vector<unsigned int>& inds,
           const std::vector<Texture>& texs)
    : vertices(verts)
    , indices(inds)
    , textures(texs)
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
{
    setupMesh();
}

Mesh::~Mesh() {
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }
}

// Move constructor
Mesh::Mesh(Mesh&& other) noexcept
    : vertices(std::move(other.vertices))
    , indices(std::move(other.indices))
    , textures(std::move(other.textures))
    , m_VAO(other.m_VAO)
    , m_VBO(other.m_VBO)
    , m_EBO(other.m_EBO)
{
    other.m_VAO = 0;
    other.m_VBO = 0;
    other.m_EBO = 0;
}

// Move assignment
Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        // Clean up existing resources
        if (m_VAO != 0) {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_VBO);
            glDeleteBuffers(1, &m_EBO);
        }
        
        // Move data
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        textures = std::move(other.textures);
        m_VAO = other.m_VAO;
        m_VBO = other.m_VBO;
        m_EBO = other.m_EBO;
        
        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_EBO = 0;
    }
    return *this;
}

// =============================================================================
// Rendering
// =============================================================================

void Mesh::draw(const Shader& shader) const {
    (void)shader; // Shader is used for texture binding in full implementation
    
    // Bind textures
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    
    for (size_t i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i));
        
        std::string name;
        if (textures[i].type == "diffuse") {
            name = "texture_diffuse" + std::to_string(diffuseNr++);
        } else if (textures[i].type == "specular") {
            name = "texture_specular" + std::to_string(specularNr++);
        }
        
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    
    // Draw mesh
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Reset texture unit
    glActiveTexture(GL_TEXTURE0);
}

// =============================================================================
// Private Methods
// =============================================================================

void Mesh::setupMesh() {
    // Generate buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    // Bind VAO first
    glBindVertexArray(m_VAO);
    
    // Upload vertex data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 vertices.size() * sizeof(Vertex),
                 vertices.data(),
                 GL_STATIC_DRAW);
    
    // Upload index data to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(),
                 GL_STATIC_DRAW);
    
    // Configure vertex attributes
    // Attribute 0: Position (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, Position));
    
    // Attribute 1: Normal (vec3)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, Normal));
    
    // Attribute 2: Texture Coordinates (vec2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, TexCoords));
    
    // Unbind VAO
    glBindVertexArray(0);
}

// =============================================================================
// Primitive Mesh Generators
// =============================================================================

namespace MeshGenerator {

Mesh createCube(float size) {
    float h = size / 2.0f;
    
    std::vector<Vertex> vertices = {
        // Front face
        {{-h, -h,  h}, { 0,  0,  1}, {0, 0}},
        {{ h, -h,  h}, { 0,  0,  1}, {1, 0}},
        {{ h,  h,  h}, { 0,  0,  1}, {1, 1}},
        {{-h,  h,  h}, { 0,  0,  1}, {0, 1}},
        // Back face
        {{ h, -h, -h}, { 0,  0, -1}, {0, 0}},
        {{-h, -h, -h}, { 0,  0, -1}, {1, 0}},
        {{-h,  h, -h}, { 0,  0, -1}, {1, 1}},
        {{ h,  h, -h}, { 0,  0, -1}, {0, 1}},
        // Top face
        {{-h,  h,  h}, { 0,  1,  0}, {0, 0}},
        {{ h,  h,  h}, { 0,  1,  0}, {1, 0}},
        {{ h,  h, -h}, { 0,  1,  0}, {1, 1}},
        {{-h,  h, -h}, { 0,  1,  0}, {0, 1}},
        // Bottom face
        {{-h, -h, -h}, { 0, -1,  0}, {0, 0}},
        {{ h, -h, -h}, { 0, -1,  0}, {1, 0}},
        {{ h, -h,  h}, { 0, -1,  0}, {1, 1}},
        {{-h, -h,  h}, { 0, -1,  0}, {0, 1}},
        // Right face
        {{ h, -h,  h}, { 1,  0,  0}, {0, 0}},
        {{ h, -h, -h}, { 1,  0,  0}, {1, 0}},
        {{ h,  h, -h}, { 1,  0,  0}, {1, 1}},
        {{ h,  h,  h}, { 1,  0,  0}, {0, 1}},
        // Left face
        {{-h, -h, -h}, {-1,  0,  0}, {0, 0}},
        {{-h, -h,  h}, {-1,  0,  0}, {1, 0}},
        {{-h,  h,  h}, {-1,  0,  0}, {1, 1}},
        {{-h,  h, -h}, {-1,  0,  0}, {0, 1}},
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20  // Left
    };
    
    return Mesh(vertices, indices);
}

Mesh createPlane(float width, float depth, float uScale, float vScale) {
    float hw = width / 2.0f;
    float hd = depth / 2.0f;
    
    std::vector<Vertex> vertices = {
        {{-hw, 0, -hd}, {0, 1, 0}, {0, 0}},
        {{ hw, 0, -hd}, {0, 1, 0}, {uScale, 0}},
        {{ hw, 0,  hd}, {0, 1, 0}, {uScale, vScale}},
        {{-hw, 0,  hd}, {0, 1, 0}, {0, vScale}},
    };
    
    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    
    return Mesh(vertices, indices);
}

Mesh createSphere(float radius, int sectors, int stacks) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float sectorStep = 2 * 3.14159265359f / sectors;
    float stackStep = 3.14159265359f / stacks;
    
    // Generate vertices
    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = 3.14159265359f / 2 - i * stackStep;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);
        
        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;
            
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            
            glm::vec3 pos(x, z, y);
            glm::vec3 normal = glm::normalize(pos);
            glm::vec2 tex(static_cast<float>(j) / sectors,
                         static_cast<float>(i) / stacks);
            
            vertices.push_back({pos, normal, tex});
        }
    }
    
    // Generate indices
    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;
        
        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    
    return Mesh(vertices, indices);
}

Mesh createCylinder(float radius, float height, int sectors) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float halfHeight = height / 2.0f;
    float sectorStep = 2 * 3.14159265359f / sectors;
    
    // Side vertices
    for (int i = 0; i <= 1; ++i) {
        float y = (i == 0) ? -halfHeight : halfHeight;
        float v = static_cast<float>(i);
        
        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);
            
            glm::vec3 pos(x, y, z);
            glm::vec3 normal = glm::normalize(glm::vec3(x, 0, z));
            glm::vec2 tex(static_cast<float>(j) / sectors, v);
            
            vertices.push_back({pos, normal, tex});
        }
    }
    
    // Side indices
    for (int j = 0; j < sectors; ++j) {
        int k1 = j;
        int k2 = j + sectors + 1;
        
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
        
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
    }
    
    // Top cap
    int baseIndex = static_cast<int>(vertices.size());
    vertices.push_back({{0, halfHeight, 0}, {0, 1, 0}, {0.5f, 0.5f}});
    for (int j = 0; j <= sectors; ++j) {
        float sectorAngle = j * sectorStep;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);
        
        glm::vec3 pos(x, halfHeight, z);
        glm::vec2 tex(0.5f + 0.5f * cosf(sectorAngle),
                     0.5f + 0.5f * sinf(sectorAngle));
        
        vertices.push_back({pos, {0, 1, 0}, tex});
    }
    
    for (int j = 0; j < sectors; ++j) {
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + j + 2);
        indices.push_back(baseIndex + j + 1);
    }
    
    // Bottom cap
    baseIndex = static_cast<int>(vertices.size());
    vertices.push_back({{0, -halfHeight, 0}, {0, -1, 0}, {0.5f, 0.5f}});
    for (int j = 0; j <= sectors; ++j) {
        float sectorAngle = j * sectorStep;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);
        
        glm::vec3 pos(x, -halfHeight, z);
        glm::vec2 tex(0.5f + 0.5f * cosf(sectorAngle),
                     0.5f + 0.5f * sinf(sectorAngle));
        
        vertices.push_back({pos, {0, -1, 0}, tex});
    }
    
    for (int j = 0; j < sectors; ++j) {
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + j + 1);
        indices.push_back(baseIndex + j + 2);
    }
    
    return Mesh(vertices, indices);
}

Mesh createCarBody() {
    // Create a simplified car body shape
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Car dimensions
    float length = 4.0f;
    float width = 1.8f;
    float bodyHeight = 0.8f;
    float cabinHeight = 0.7f;
    float hoodLength = 1.2f;
    float trunkLength = 0.8f;
    
    float hl = length / 2.0f;
    float hw = width / 2.0f;
    
    // Lower body (box shape)
    // Front
    vertices.push_back({{-hl, 0, -hw}, {0, 0, -1}, {0, 0}});
    vertices.push_back({{ hl, 0, -hw}, {0, 0, -1}, {1, 0}});
    vertices.push_back({{ hl, bodyHeight, -hw}, {0, 0, -1}, {1, 1}});
    vertices.push_back({{-hl, bodyHeight, -hw}, {0, 0, -1}, {0, 1}});
    
    // Back
    vertices.push_back({{ hl, 0, hw}, {0, 0, 1}, {0, 0}});
    vertices.push_back({{-hl, 0, hw}, {0, 0, 1}, {1, 0}});
    vertices.push_back({{-hl, bodyHeight, hw}, {0, 0, 1}, {1, 1}});
    vertices.push_back({{ hl, bodyHeight, hw}, {0, 0, 1}, {0, 1}});
    
    // Left side
    vertices.push_back({{-hl, 0, hw}, {-1, 0, 0}, {0, 0}});
    vertices.push_back({{-hl, 0, -hw}, {-1, 0, 0}, {1, 0}});
    vertices.push_back({{-hl, bodyHeight, -hw}, {-1, 0, 0}, {1, 1}});
    vertices.push_back({{-hl, bodyHeight, hw}, {-1, 0, 0}, {0, 1}});
    
    // Right side
    vertices.push_back({{ hl, 0, -hw}, {1, 0, 0}, {0, 0}});
    vertices.push_back({{ hl, 0, hw}, {1, 0, 0}, {1, 0}});
    vertices.push_back({{ hl, bodyHeight, hw}, {1, 0, 0}, {1, 1}});
    vertices.push_back({{ hl, bodyHeight, -hw}, {1, 0, 0}, {0, 1}});
    
    // Bottom
    vertices.push_back({{-hl, 0, hw}, {0, -1, 0}, {0, 0}});
    vertices.push_back({{ hl, 0, hw}, {0, -1, 0}, {1, 0}});
    vertices.push_back({{ hl, 0, -hw}, {0, -1, 0}, {1, 1}});
    vertices.push_back({{-hl, 0, -hw}, {0, -1, 0}, {0, 1}});
    
    // Hood (top of front section)
    float hoodStart = hl - hoodLength;
    vertices.push_back({{hoodStart, bodyHeight, -hw}, {0, 1, 0}, {0, 0}});
    vertices.push_back({{hl, bodyHeight, -hw}, {0, 1, 0}, {1, 0}});
    vertices.push_back({{hl, bodyHeight, hw}, {0, 1, 0}, {1, 1}});
    vertices.push_back({{hoodStart, bodyHeight, hw}, {0, 1, 0}, {0, 1}});
    
    // Trunk (top of rear section)
    float trunkEnd = -hl + trunkLength;
    vertices.push_back({{-hl, bodyHeight, -hw}, {0, 1, 0}, {0, 0}});
    vertices.push_back({{trunkEnd, bodyHeight, -hw}, {0, 1, 0}, {1, 0}});
    vertices.push_back({{trunkEnd, bodyHeight, hw}, {0, 1, 0}, {1, 1}});
    vertices.push_back({{-hl, bodyHeight, hw}, {0, 1, 0}, {0, 1}});
    
    // Cabin (raised middle section)
    float cabinFront = hoodStart;
    float cabinBack = trunkEnd;
    float cabinTop = bodyHeight + cabinHeight;
    float cabinWidth = hw * 0.9f;
    
    // Cabin front (windshield area)
    vertices.push_back({{cabinFront, bodyHeight, -cabinWidth}, {0.7f, 0.7f, 0}, {0, 0}});
    vertices.push_back({{cabinFront + 0.3f, cabinTop, -cabinWidth}, {0.7f, 0.7f, 0}, {1, 0}});
    vertices.push_back({{cabinFront + 0.3f, cabinTop, cabinWidth}, {0.7f, 0.7f, 0}, {1, 1}});
    vertices.push_back({{cabinFront, bodyHeight, cabinWidth}, {0.7f, 0.7f, 0}, {0, 1}});
    
    // Cabin back (rear window area)
    vertices.push_back({{cabinBack, bodyHeight, cabinWidth}, {-0.7f, 0.7f, 0}, {0, 0}});
    vertices.push_back({{cabinBack - 0.3f, cabinTop, cabinWidth}, {-0.7f, 0.7f, 0}, {1, 0}});
    vertices.push_back({{cabinBack - 0.3f, cabinTop, -cabinWidth}, {-0.7f, 0.7f, 0}, {1, 1}});
    vertices.push_back({{cabinBack, bodyHeight, -cabinWidth}, {-0.7f, 0.7f, 0}, {0, 1}});
    
    // Cabin roof
    vertices.push_back({{cabinFront + 0.3f, cabinTop, -cabinWidth}, {0, 1, 0}, {0, 0}});
    vertices.push_back({{cabinBack - 0.3f, cabinTop, -cabinWidth}, {0, 1, 0}, {1, 0}});
    vertices.push_back({{cabinBack - 0.3f, cabinTop, cabinWidth}, {0, 1, 0}, {1, 1}});
    vertices.push_back({{cabinFront + 0.3f, cabinTop, cabinWidth}, {0, 1, 0}, {0, 1}});
    
    // Generate indices for all quads
    for (unsigned int i = 0; i < vertices.size(); i += 4) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i);
    }
    
    return Mesh(vertices, indices);
}

Mesh createWheel(float radius, float width) {
    return createCylinder(radius, width, 24);
}

} // namespace MeshGenerator
