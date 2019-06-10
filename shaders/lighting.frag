#version 330

struct material_t {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct dlight_t {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct plight_t {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

struct spotlight_t {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
    float cutoff;
    float outer_cutoff;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 view_pos;
uniform material_t material;

uniform dlight_t dlight;
uniform plight_t plight;
uniform spotlight_t spotlight;

vec3 calc_dlight(dlight_t light, vec3 norm, vec3 view_dir);
vec3 calc_plight(plight_t light, vec3 norm, vec3 view_dir, vec3 frag_pos);
vec3 calc_spotlight(spotlight_t light, vec3 norm, vec3 view_dir, vec3 frag_pos);

void main(void) {

    vec3 norm = normalize(Normal);
    vec3 view_dir = normalize(view_pos - FragPos);

    vec3 result = calc_dlight(dlight, norm, view_dir);

    result += calc_plight(plight, norm, view_dir, FragPos);
    result += calc_spotlight(spotlight, norm, view_dir, FragPos);

    FragColor = vec4(result, 1.0);
}

vec3 calc_dlight(dlight_t light, vec3 norm, vec3 view_dir) {

    vec3 light_dir = normalize(-light.direction);

    /* diffuse */
    float diff = max(dot(norm, light_dir), 0.0);

    /* specular */
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    return ambient + diffuse + specular;
}

vec3 calc_plight(plight_t light, vec3 norm, vec3 view_dir, vec3 frag_pos) {

    vec3 light_dir = normalize(light.position - frag_pos);

    float diff = max(dot(norm, light_dir), 0.0);

    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    /* attenuation */
    float distance = length(light.position - frag_pos);
    float att = 1.0 / (light.attenuation.x * (distance * distance) +
            light.attenuation.y * distance + light.attenuation.z);

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    ambient *= att;
    diffuse *= att;
    specular *= att;

    return ambient + diffuse + specular;
}

vec3 calc_spotlight(spotlight_t light, vec3 norm, vec3 view_dir, vec3 frag_pos) {

    vec3 light_dir = normalize(light.position - frag_pos);

    float diff = max(dot(norm, light_dir), 0.0);

    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    /* soft edges */
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

    diffuse *= intensity;
    specular *= intensity;

    return ambient + diffuse + specular;
}
