#version 130

in vec3 _color;
out vec4 frag_color;

uniform vec3 triangle_color;

void main() {
    frag_color = vec4(_color.rgb, 1.0);
}
