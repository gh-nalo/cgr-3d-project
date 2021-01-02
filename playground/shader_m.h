#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:

    /// <summary>
    ///     Shader program ID
    /// </summary>
    unsigned int ID;

    /// <summary>
    ///     Constructor
    /// </summary>
    /// <param name="vertexPath">Path to the vertex shader</param>
    /// <param name="fragmentPath">Path to the fragmentshader</param>
    /// <param name="geometryPath">Path to the geometry shader (optional)</param>
    /// <returns>Object</returns>
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

    /// <summary>
    ///     Use shader program
    /// </summary>
    void use();

    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="value">Value of the uniform</param>
    void setBool(const std::string& name, bool value) const;
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="value">Value of the uniform</param>
    void setInt(const std::string& name, int value) const;
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="value">Value of the uniform</param>
    void setFloat(const std::string& name, float value) const;
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="value">Value of the uniform</param>
    void setVec2(const std::string& name, const glm::vec2& value) const;
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="x">x value of the uniform</param>
    /// <param name="y">y value of the uniform</param>
    void setVec2(const std::string& name, float x, float y) const;
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="value">Value of the uniform</param>
    void setVec3(const std::string& name, const glm::vec3& value) const;
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="x">x value of the uniform</param>
    /// <param name="y">y value of the uniform</param>
    /// <param name="z">z value of the uniform</param>
    void setVec3(const std::string& name, float x, float y, float z) const;
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="value">Value of the uniform</param>
    void setVec4(const std::string& name, const glm::vec4& value) const;
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="x">x value of the uniform</param>
    /// <param name="y">y value of the uniform</param>
    /// <param name="z">z value of the uniform</param>
    /// <param name="w">w value of the uniform</param>
    void setVec4(const std::string& name, float x, float y, float z, float w);
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="mat">Value of the uniform</param>
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="mat">Value of the uniform</param>
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    
    /// <summary>
    ///     Set uniform in the shader program
    /// </summary>
    /// <param name="name">Name of the uniform</param>
    /// <param name="mat">Value of the uniform</param>
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:

    /// <summary>
    ///     Check for errors with the shader
    /// </summary>
    /// <param name="shader">Shader</param>
    /// <param name="type">Type</param>
    void checkCompileErrors(GLuint shader, std::string type);
};
#endif