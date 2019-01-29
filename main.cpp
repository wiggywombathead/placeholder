#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include "util.h"
#include "camera.h"

#define WIN_W 800
#define WIN_H 600

GLFWwindow *window;
bool error_check;

/* camera */
Camera camera;

/* shader programs */
GLuint lighting_shader, lamp_shader;

/* drawable objects */
GLuint tetra_vao;
GLuint cube_vao, cube_tex;

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

    GLint pos = glGetAttribLocation(lamp_shader, "position");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);

    GLint col = glGetAttribLocation(lamp_shader, "color");
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void *) (3*sizeof(float)));

    return vao;
}

GLuint make_cube(void) {

    GLuint vao;

    float verts[] = {
        // X Y Z            // R G B        // tex coords
        -0.5, -0.5,  0.5,   1.0, 0.0, 0.0,  0.0, 0.0,   // bottom left
         0.5, -0.5,  0.5,   0.0, 1.0, 0.0,  1.0, 0.0,   // bottom right
         0.5, -0.5, -0.5,   0.0, 0.0, 1.0,  1.0, 1.0,   // top right
        -0.5, -0.5, -0.5,   1.0, 1.0, 1.0,  0.0, 1.0,   // top left
                            
        -0.5,  0.5,  0.5,   1.0, 1.0, 0.0,  0.0, 0.0,
         0.5,  0.5,  0.5,   0.0, 1.0, 1.0,  1.0, 0.0,
         0.5,  0.5, -0.5,   1.0, 0.0, 1.0,  1.0, 1.0,
        -0.5,  0.5, -0.5,   0.0, 0.0, 0.0,  0.0, 1.0,
    };

    GLuint elems[] = {
        0,1,5,5,4,0,    // front
        1,2,6,6,5,1,    // right
        2,3,7,7,6,2,
        3,0,4,4,7,3,
        4,5,6,6,7,4,
        3,2,1,1,0,3
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

    /* generate texture */
    int width, height;
    unsigned char *img = SOIL_load_image(
            "planks.jpg",
            &width,
            &height,
            0,
            SOIL_LOAD_RGB
        );


    glGenTextures(1, &cube_tex);
    glBindTexture(GL_TEXTURE_2D, cube_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    glEnableVertexAttribArray(0);

    GLint pos = glGetAttribLocation(lighting_shader, "position");
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    glEnableVertexAttribArray(pos);

    GLint col = glGetAttribLocation(lighting_shader, "color");
    glVertexAttribPointer(col, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *) (3*sizeof(float)));
    glEnableVertexAttribArray(col);

    GLint tex = glGetAttribLocation(lighting_shader, "texcoord");
    glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *) (6*sizeof(float)));
    glEnableVertexAttribArray(tex);

    return vao;
}

void draw_cube(void) {
    // glBindTexture(GL_TEXTURE_2D, cube_tex);
    glBindVertexArray(cube_vao);
    glDrawElements(GL_TRIANGLES, 6*2*3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void draw_tetra(void) {
    glBindVertexArray(tetra_vao);

    /* faces * triangles per faces * floats per vertex */
    glDrawElements(GL_TRIANGLES, 6*2*3, GL_UNSIGNED_INT, 0);

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

void keyboard(GLFWwindow *w, int k, int sc, int action, int mods) {

    float step = .5f;

    glm::vec3 to_move;

    if (action == GLFW_RELEASE)
        return;
    
    switch (k) {
    case GLFW_KEY_Q:
        glfwSetWindowShouldClose(w, GLFW_TRUE);
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
    case GLFW_KEY_LEFT_BRACKET:
        camera.fov_up(1.f);
        break;
    case GLFW_KEY_RIGHT_BRACKET:
        camera.fov_down(1.f);
        break;
    };

}

void update(void) {

    t_now = glfwGetTime();

}

void display(void) {

    GLint unimodel;

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(lighting_shader);

    GLint obj_color = glGetUniformLocation(lighting_shader, "obj_color");
    glUniform3f(obj_color, coral.x, coral.y, coral.z);
    GLint light_color = glGetUniformLocation(lighting_shader, "light_color");
    glUniform3f(light_color, sun.x, sun.y, sun.z);

    /* Projection */
    proj = glm::perspective(
            glm::radians(camera.get_fov()),
            (float) WIN_W / WIN_H,
            1.0f,
            50.0f
    );
    GLint uniproj = glGetUniformLocation(lighting_shader, "Proj");
    glUniformMatrix4fv(uniproj, 1, GL_FALSE, glm::value_ptr(proj));

    /* View */
    view = camera.look();
    GLint uniview = glGetUniformLocation(lighting_shader, "View");
    glUniformMatrix4fv(uniview, 1, GL_FALSE, glm::value_ptr(view));

    /* Model */
    model = glm::mat4(1.f);
    unimodel = glGetUniformLocation(lighting_shader, "Model");
    glUniformMatrix4fv(unimodel, 1, GL_FALSE, glm::value_ptr(model));

    draw_cube();

    glUseProgram(lamp_shader);

    uniproj = glGetUniformLocation(lamp_shader, "Proj");
    glUniformMatrix4fv(uniproj, 1, GL_FALSE, glm::value_ptr(proj));

    /* View */
    view = camera.look();
    uniview = glGetUniformLocation(lamp_shader, "View");
    glUniformMatrix4fv(uniview, 1, GL_FALSE, glm::value_ptr(view));

    /* Model */
    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(1.f, 2.f, -4.f));
    model = glm::scale(model, glm::vec3(0.5f));
    model = glm::rotate(model, 2 * t_now, glm::vec3(0, 1, 0));

    unimodel = glGetUniformLocation(lamp_shader, "Model");
    glUniformMatrix4fv(unimodel, 1, GL_FALSE, glm::value_ptr(model));

    draw_tetra();
}

void init(void) {

    /* OpenGL settings */
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    /* glfw settings */
    glfwSetKeyCallback(window, keyboard);
    glfwSetCursorPosCallback(window, cursor);

    // mouse tends to left ???
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Load shaders */
    lighting_shader = create_program("shader.vert", "lighting.frag");
    lamp_shader = create_program("shader.vert", "lamp.frag");

    /* set up objects */
    tetra_vao = make_tetra();
    cube_vao = make_cube();
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

    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    printf("OpenGL version supported: %s\n", version);
    printf("Renderer: %s\n", renderer);

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
