#version 130

in vec3 position;
in vec3 color;

out vec3 _color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;

void main() {
    _color = color;
    gl_Position = Proj * View * Model * vec4(position.xyz, 1.0);
}
