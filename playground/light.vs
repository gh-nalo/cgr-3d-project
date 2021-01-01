#version 330 core

layout (location = 0) in vec3 initialVertexPositions;
layout (location = 1) in vec3 initialNormals;
layout (location = 2) in vec2 initialTextureCoordinates;

out vec3 vertexPosition;
out vec3 normalPosition;
out vec2 textureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vertexPosition = vec3(model * vec4(initialVertexPositions, 1.0));
    normalPosition = mat3(transpose(inverse(model))) * initialNormals;
    textureCoordinates = initialTextureCoordinates;
    
    gl_Position = projection * view * vec4(vertexPosition, 1.0);
}