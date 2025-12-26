/**
 * =============================================================================
 * Shader.cpp - OpenGL Shader Program Implementation
 * =============================================================================
 */

#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

// =============================================================================
// Constructors / Destructor
// =============================================================================

Shader::Shader() : m_programID(0) {}

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource, bool fromFile)
    : m_programID(0)
{
    std::string vertCode, fragCode;
    
    if (fromFile) {
        vertCode = readFile(vertexSource);
        fragCode = readFile(fragmentSource);
        
        if (vertCode.empty() || fragCode.empty()) {
            std::cerr << "ERROR: Failed to read shader files" << std::endl;
            return;
        }
    } else {
        vertCode = vertexSource;
        fragCode = fragmentSource;
    }
    
    // Compile shaders
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertCode);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragCode);
    
    if (vertexShader == 0 || fragmentShader == 0) {
        if (vertexShader) glDeleteShader(vertexShader);
        if (fragmentShader) glDeleteShader(fragmentShader);
        return;
    }
    
    // Link program
    m_programID = linkProgram(vertexShader, fragmentShader);
    
    // Shaders are no longer needed after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
    }
}

// Move constructor
Shader::Shader(Shader&& other) noexcept : m_programID(other.m_programID) {
    other.m_programID = 0;
}

// Move assignment
Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_programID != 0) {
            glDeleteProgram(m_programID);
        }
        m_programID = other.m_programID;
        other.m_programID = 0;
    }
    return *this;
}

// =============================================================================
// Usage
// =============================================================================

void Shader::use() const {
    glUseProgram(m_programID);
}

// =============================================================================
// Uniform Setters
// =============================================================================

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(getUniformLocation(name), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2f(getUniformLocation(name), value.x, value.y);
}

void Shader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(getUniformLocation(name), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(getUniformLocation(name), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(getUniformLocation(name), x, y, z, w);
}

void Shader::setMat3(const std::string& name, const glm::mat3& value) const {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

// =============================================================================
// Private Helper Functions
// =============================================================================

std::string Shader::readFile(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file: " << filepath << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) const {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    // Check for compilation errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR: Shader compilation failed ("
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << "):\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

unsigned int Shader::linkProgram(unsigned int vertexShader, unsigned int fragmentShader) const {
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Check for linking errors
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR: Shader program linking failed:\n" << infoLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }
    
    return program;
}

int Shader::getUniformLocation(const std::string& name) const {
    // In a production implementation, this would cache uniform locations
    // in a std::unordered_map to avoid repeated lookups
    return glGetUniformLocation(m_programID, name.c_str());
}
