#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;

void main() {
    TexCoords = tex_coords;
    Normal = mat3(transpose(inverse(Model))) * normal;
    FragPos = vec3(Model * vec4(position, 1.0));
    gl_Position = Proj * View * Model * vec4(FragPos, 1.0);
    gl_PointSize = gl_Position.z;
}
