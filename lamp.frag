#version 330

out vec4 frag_color;

uniform vec3 light_color;

void main(void) {
    frag_color = vec4(light_color, 1.0);
}
