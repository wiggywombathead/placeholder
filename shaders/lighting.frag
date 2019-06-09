#version 330

struct material_t {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct plight_t {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

struct dlight_t {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct spotlight_t {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutoff;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 view_pos;
uniform material_t material;

// uniform plight_t light;
uniform spotlight_t light;

void main(void) {
    vec3 norm = normalize(Normal);

    vec3 light_dir = normalize(light.position - FragPos);
    // vec3 light_dir = normalize(-light.direction); directional light

    float theta = dot(light_dir, normalize(-light.direction));
    if (theta > light.cutoff) {

        float diff = max(dot(norm, light_dir), 0.0);

        vec3 view_dir = normalize(view_pos - FragPos);
        vec3 reflect_dir = reflect(-light_dir, norm);
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

        vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
        vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

        float distance = length(light.position - FragPos);
        float att = 1.0 / (light.attenuation.x * (distance * distance) +
                light.attenuation.y * distance + light.attenuation.z);

        ambient *= att;
        diffuse *= att;
        specular *= att;

        vec3 result = ambient + diffuse + specular;

        FragColor = vec4(result, 1.0);

    } else {
        FragColor = vec4(light.ambient * vec3(texture(material.diffuse,
                        TexCoords)), 1.0);
    }
}
