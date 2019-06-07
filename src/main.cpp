#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "util.h"
#include "camera.h"
#include "shader.h"

#define WIN_W 800
#define WIN_H 600

GLFWwindow *window;
bool error_check;
bool wireframe = false;
bool flat_shading = false;
bool paused = false;

struct material_t {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

material_t bronze = {
    {0.2125, 0.1275, 0.054},
    {0.714, 0.4284, 0.18144},
    {0.393548, 0.271906, 0.166721},
    0.2
};

material_t copper = {
    {0.19125, 0.0735, 0.0225},
    {0.7038, 0.27048, 0.0828},
    {0.256777, 0.137622, 0.086014},
    0.1
};

material_t pearl = {
    {0.25, 0.20725,	0.20725},
    {1, 0.829, 0.829},
    {0.296648, 0.296648, 0.296648},
    0.088
};

/* camera */
Camera camera;

/* Shaders */
Shader lighting_shader;
Shader lamp_shader;
Shader material_shader;
Shader simple_shader;

/* drawable objects */
GLuint tetra_vao;
GLuint cube_vao;
GLuint earth_vao;

GLuint container_diff, container_spec;

glm::mat4 model, view, proj;

glm::vec3 sun(1.f, 1.f, 1.f);
glm::vec3 coral(1.f, .5f, .31f);

float t_start, t_last, t_now;

GLuint make_tetra(void) {

    GLuint vao;

    float verts[] = {
         1.0,  0.0, -1/sqrt(2), 1.0, 0.0, 0.0,
        -1.0,  0.0, -1/sqrt(2), 0.0, 1.0, 0.0,
         0.0,  1.0,  1/sqrt(2), 0.0, 0.0, 1.0,
         0.0, -1.0,  1/sqrt(2), 0.0, 1.0, 1.0
    };

    GLuint elems[] = {
        0,1,2,
        1,3,2,
        3,0,2,
        0,3,1
    };

    GLuint tetra_vbo;
    GLuint tetra_ebo;

    /* set up array object */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* copy points into GPU using buffer object */
    glGenBuffers(1, &tetra_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, tetra_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    /* set up element buffer */
    glGenBuffers(1, &tetra_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tetra_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    /* position attribute */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
    glEnableVertexAttribArray(0);
    
    /* color attribute */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void *) (3*sizeof(float)));
    glEnableVertexAttribArray(1);

    return vao;
}

GLuint make_cube(void) {

    GLuint vao;

    float verts[] = {
        // X Y Z              // NORMAL X Y Z      // TEX
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    GLuint elems[] = {
        0,1,5,5,4,0,    // front
        1,2,6,6,5,1,    // right
        2,3,7,7,6,2,    // back
        3,0,4,4,7,3,    // left
        4,5,6,6,7,4,    // top
        3,2,1,1,0,3     // bottom
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint cube_vbo;
    GLuint cube_ebo;

    /* copy points into GPU using buffer object */
    glGenBuffers(1, &cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    /* set up element buffer */
    glGenBuffers(1, &cube_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    /* position */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    glEnableVertexAttribArray(0);

    /* normal */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *) (3*sizeof(float)));
    glEnableVertexAttribArray(1);

    /* texture coords */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *) (6*sizeof(float)));
    glEnableVertexAttribArray(2);

    return vao;
}

int resolution = 20;
float scale = 1;
GLint make_earth() {

    srand(time(0));

    GLuint vao, vbo, ebo;

    float gap = 2.f / (resolution - 1);

    glm::vec3 landscape[2 * resolution * resolution];

    float x_pos, z_pos;

    z_pos = -1.f;
    for (int j = 0; j < resolution; j++) {
        x_pos = -1.f;
        for (int i = 0; i < resolution; i++) {

            int x = x_pos + i * gap;
            int z = z_pos + j * gap;

            int index = j * resolution + i;
            int vertex = 2*index;
            int normal = 2*index+1;

            // vertex info
            landscape[vertex] = glm::vec3(
                    x_pos,
                    .25 - .5 * ((float) rand() / RAND_MAX),
                    z_pos
                );

            // normal info
            if (i == resolution-1) 
                landscape[normal] = landscape[normal - 2];
            else if (j == resolution-1)
                landscape[normal] = landscape[normal - 2 * resolution];
            else
                landscape[normal] = glm::normalize(
                        glm::cross(
                            landscape[vertex + 2 * resolution] - landscape[vertex],
                            landscape[vertex + 2] - landscape[vertex + 2 * resolution]
                        )
                    );

            x_pos += gap;
        }
        z_pos += gap;
    }

    /*
    float landscape[3 * resolution * resolution];
    for (int z = 0; z < resolution; z++) {
        x_pos = -1.f;
        for (int x = 0; x < resolution; x++) {
            int index = z * resolution + x;
            landscape[3*index] = x_pos + x * gap;
            landscape[(3*index) + 1] = .25 - .5 * ((float) rand() / RAND_MAX);
            landscape[(3*index) + 2] = z_pos + z * gap;

            x_pos += gap;
        }

        z_pos += gap;
    }
    */

    /* points per triangle * triangles per square * total squares */
    GLuint entries = 3 * 2 * (resolution-1) * (resolution-1);
    GLuint elems[entries];

    int skip = 0;
    for (int i = 0; 6*i < entries; i++) {
        if (i % (resolution-1) == 0 && i != 0)
            skip++;

        int node = i + skip;
        elems[6*i] = node;
        elems[6*i + 1] = node + resolution;
        elems[6*i + 2] = node + 1;
        elems[6*i + 3] = node + 1;
        elems[6*i + 4] = node + resolution;
        elems[6*i + 5] = node + 1 + resolution;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(landscape), landscape, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_DYNAMIC_DRAW);

    /* position */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), 0);
    glEnableVertexAttribArray(0);

    /* normal */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), (void *) (2*sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    return vao;
}

void draw_earth(void) {
    glBindVertexArray(earth_vao);
    glDrawElements(GL_TRIANGLES, 3*2*(resolution-1)*(resolution-1), GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_POINTS, 0, resolution * 3);
    glBindVertexArray(0);
}

void draw_cube(void) {
    glBindVertexArray(cube_vao);
    // glDrawElements(GL_TRIANGLES, 6*2*3, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6*2*3);
    glBindVertexArray(0);
}

void draw_tetra(void) {
    glBindVertexArray(tetra_vao);

    /* faces * triangles per faces * floats per vertex */
    glDrawElements(GL_TRIANGLES, 4*1*3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

double last_x, last_y;
bool first_mouse = true;
void cursor(GLFWwindow *w, double x, double y) {

    if (first_mouse) {
        last_x = x;
        last_y = y;
        first_mouse = false;
    }

    if (x < WIN_W / 4 || x > 3 * WIN_W / 4) {
        glfwSetCursorPos(w, WIN_W / 2, y);
        first_mouse = true;
    }

    if (y < WIN_H / 4 || y > 3 * WIN_H / 4) {
        glfwSetCursorPos(w, x, WIN_H / 2);
        first_mouse = true;
    }

    double dx = x - last_x;
    double dy = y - last_y;
    last_x = x;
    last_y = y;

    float sensitivity = 0.01f;
    dx *= sensitivity;
    dy *= sensitivity;

    camera.change_pitch(-dy);
    camera.change_yaw(dx);
}

glm::vec3 light_pos(0, 0, 0);
glm::vec3 light_centre(0, 0, 0);
glm::vec3 light_col(1.f);
void keyboard(GLFWwindow *w, int k, int sc, int action, int mods) {

    float step = .5f;

    glm::vec3 to_move;

    if (action == GLFW_RELEASE)
        return;
    
    switch (k) {
    /* GAME STATE */
    case GLFW_KEY_Q:
        glfwSetWindowShouldClose(w, GLFW_TRUE);
    case GLFW_KEY_P:
        paused = !paused;
        break;
    /* CAMERA MOVEMENT */
    case GLFW_KEY_W:
        camera.move(step * camera.get_dir());
        break;
    case GLFW_KEY_S:
        camera.move(-step * camera.get_dir());
        break;
    case GLFW_KEY_A:
        to_move = glm::normalize(glm::cross(camera.get_dir(), camera.get_up()));
        camera.move(-step * to_move);
        break;
    case GLFW_KEY_D:
        to_move = glm::normalize(glm::cross(camera.get_dir(), camera.get_up()));
        camera.move(step * to_move);
        break;
    case GLFW_KEY_Z:
        to_move = glm::vec3(0,1,0);
        camera.move(step * to_move);
        break;
    case GLFW_KEY_X:
        to_move = glm::vec3(0,-1,0);
        camera.move(step * to_move);
        break;
    case GLFW_KEY_SPACE:
        light_centre = camera.get_pos();
        break;
    /* DISPLAY PROPERTIES */
    case GLFW_KEY_LEFT_BRACKET:
        camera.fov_up(1.f);
        break;
    case GLFW_KEY_RIGHT_BRACKET:
        camera.fov_down(1.f);
        break;
    case GLFW_KEY_F:
        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        wireframe = !wireframe;
        break;
    case GLFW_KEY_G:
        if (flat_shading)
            glShadeModel(GL_FLAT);
        else
            glShadeModel(GL_SMOOTH);
        flat_shading = !flat_shading;
    };

}

int ticks = 0;
void update(void) {

    if (paused)
        return;

    t_now = glfwGetTime();

    float rads = ticks * M_PI / 180.f;

    float z = 2*cos(rads);
    float x = 2*sin(rads);
    light_pos = glm::vec3(light_centre.x + x, 0, light_centre.z + z);

    ticks++;
}

void display(void) {

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    lighting_shader.use();

    lighting_shader.set_vec3("view_pos", camera.get_pos());

    lighting_shader.set_vec3("light.position", light_pos);
    lighting_shader.set_vec3("light.ambient", glm::vec3(.2));
    lighting_shader.set_vec3("light.diffuse", glm::vec3(.5));
    lighting_shader.set_vec3("light.specular", glm::vec3(1.));

    lighting_shader.set_int("material.diffuse", 0);
    lighting_shader.set_int("material.specular", 1);
    lighting_shader.set_float("material.shininess", 64.f);

    proj = glm::perspective(glm::radians(camera.get_fov()), (float) WIN_W / WIN_H, 1.0f, 50.0f);
    lighting_shader.set_mat4("Proj", proj);

    view = camera.look();
    lighting_shader.set_mat4("View", view);

    model = glm::mat4(1.f);
    lighting_shader.set_mat4("Model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, container_diff);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, container_spec);
    draw_cube();

    lamp_shader.use();

    lamp_shader.set_vec3("light_color", light_col);

    lamp_shader.set_mat4("Proj", proj);
    lamp_shader.set_mat4("View", view);

    model = glm::mat4(1.f);
    model = glm::translate(model, light_pos);
    model = glm::scale(model, glm::vec3(.1f));
    lamp_shader.set_mat4("Model", model);

    draw_cube();

}

void init(void) {

    /* OpenGL settings */
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glShadeModel(GL_SMOOTH);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);

    /* glfw settings */
    glfwSetKeyCallback(window, keyboard);
    glfwSetCursorPosCallback(window, cursor);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    /* Load shaders */
    // material_shader = Shader("shaders/lighting.vert", "shaders/material.frag");
    // simple_shader = Shader("shaders/simple.vert", "shaders/simple.frag");
    lighting_shader = Shader("shaders/lighting.vert", "shaders/lighting.frag");
    lamp_shader = Shader("shaders/lamp.vert", "shaders/lamp.frag");

    /* set up objects */
    cube_vao = make_cube();
    tetra_vao = make_tetra();
    earth_vao = make_earth();

    container_diff = make_texture("tex/container_diff.png");
    container_spec = make_texture("tex/container_spec.png");
}

int main(int argc, char *argv[]) {

    if (!glfwInit()) {
        fprintf(stderr, "Error: could not init GLFW3\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(WIN_W, WIN_H, "tinyworld", NULL, NULL);

    if (!window) {
        fprintf(stderr, "Error: could not create window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    printf("OpenGL version: %s\n"
            "Renderer: %s\n"
            "GLSL version: %s\n",
            glGetString(GL_VERSION),
            glGetString(GL_RENDERER),
            glGetString(GL_SHADING_LANGUAGE_VERSION)
    );

    init();

    /* main shit */
    while (!glfwWindowShouldClose(window)) {

        update();
        display();

        glfwPollEvents();
        glfwSwapBuffers(window);

    }

    glfwTerminate();

    return 0;
}
