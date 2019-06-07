#version 330

out vec4 FragColor;

uniform vec3 light_color;

void main(void) {
    FragColor = vec4(light_color, 1.0);
}
