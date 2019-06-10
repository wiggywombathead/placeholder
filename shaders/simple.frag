#version 330

struct light_t {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct material_t {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 FragPos;
in vec3 Normal;

uniform light_t sun;
uniform material_t ground;
uniform vec3 view;

out vec4 FragColor;

void main() {

    vec3 ambient = sun.ambient * ground.diffuse;

    vec3 norm = normalize(Normal);
    vec3 light_dir = normalize(sun.position - FragPos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = sun.diffuse * diff * ground.diffuse;

    vec3 view_dir = normalize(view - FragPos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), ground.shininess);
    vec3 specular = sun.specular * spec * ground.specular;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(1.0);
}
