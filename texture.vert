#version 130

in vec3 position;
in vec3 color;
in vec2 texcoord;

out vec3 _color;
out vec2 _texcoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;

void main() {
    _color = color;
    _texcoord = texcoord;
    gl_Position = Proj * View * Model * vec4(position, 1.0);
}
