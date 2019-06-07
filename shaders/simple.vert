#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;

out vec3 FragPos;
out vec3 Normal;

void main(void) {
    Normal = mat3(transpose(inverse(Model))) * normal;
    FragPos = vec3(Model * vec4(position, 1.0));

    gl_Position = Proj * View * Model * vec4(position, 1.0);
}
