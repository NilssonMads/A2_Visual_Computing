#version 330 core

// Input vertex data
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data for fragment shader
out vec2 UV;

// Transformation matrices
uniform mat4 MVP;

void main() {
    // Pass UV coordinates to fragment shader
    UV = vertexUV;
    
    // Output position
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
}