#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

out vec3 Color;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;

void main() {
    Color = color;
    Normal = mat3(transpose(inverse(Model))) * normal;
    FragPos = vec3(Model * vec4(position, 1.0));
    gl_Position = Proj * View * Model * vec4(position, 1.0);
    gl_PointSize = gl_Position.z;
}
