#version 330

in vec3 _color;
out vec4 frag_color;

uniform vec3 obj_color;
uniform vec3 light_color;

void main(void) {

    float amb_pow = 0.1;
    vec3 ambient = amb_pow * light_color;
    vec3 result = ambient * obj_color;

    frag_color = vec4(result, 1.0);
    // frag_color = vec4(_color, 1.0);
    // frag_color = vec4(obj_color * light_color, 1.0);
}
