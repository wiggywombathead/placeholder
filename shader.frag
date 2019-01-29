#version 130

in vec3 _color;
in vec2 _texcoord;

out vec4 frag_color;

uniform vec3 triangle_color;
uniform sampler2D _texture;
uniform sampler2D _texture2;

uniform vec3 obj_color;
uniform vec3 light_color;

void main() {
    frag_color = vec4(_color, 1.0);
    // frag_color = texture(_texture, _texcoord);
    // frag_color = mix(texture(_texture, _texcoord), texture(_texture2, _texcoord), 0.2);
}
