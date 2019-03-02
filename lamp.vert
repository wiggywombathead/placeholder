#version 330

layout (location = 0) in vec3 position;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;

void main() {
    gl_Position = Proj * View * Model * vec4(position, 1.0);
}
