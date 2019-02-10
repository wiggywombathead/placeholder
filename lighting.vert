#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;

out vec3 _color;

void main() {
    _color = color;
    gl_Position = Proj * View * Model * vec4(position, 1.0);
}
