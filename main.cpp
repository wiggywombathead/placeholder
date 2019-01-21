#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "util.h"
#include "camera.h"

GLFWwindow *window;
bool error_check;

/* camera */
Camera camera;

GLuint vert_shader, frag_shader, shader_program;
char *vert_source, *frag_source;
char *shaders[] = {
    vert_source, frag_source
};

GLuint tetra_vbo;
GLuint tetra_vao;
GLuint tetra_ebo;

GLuint cube_vao;
GLuint cube_vbo;
GLuint cube_ebo;

glm::mat4 view, proj;

float t_start, t_last, t_now;

void make_cube(void) {

    float verts[] = {
        -0.5, -0.5,  0.5, 1.0, 0.0, 0.0,
         0.5, -0.5,  0.5, 0.0, 1.0, 0.0,
         0.5, -0.5, -0.5, 0.0, 0.0, 1.0,
        -0.5, -0.5, -0.5, 1.0, 1.0, 1.0,

        -0.5,  0.5,  0.5, 1.0, 1.0, 0.0,
         0.5,  0.5,  0.5, 0.0, 1.0, 1.0,
         0.5,  0.5, -0.5, 1.0, 0.0, 1.0,
        -0.5,  0.5, -0.5, 0.0, 0.0, 0.0,
    };

    GLuint elems[] = {
        0, 1, 2,
        2, 3, 0,
        0, 4, 7,
        7, 3, 0,
        4, 5, 6,
        6, 7, 4,
        1, 5, 6,
        6, 2, 1,
        0, 1, 5,
        5, 4, 0,
        2, 3, 7,
        7, 6, 2
    };

    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);

    /* copy points into GPU using buffer object */
    glGenBuffers(1, &cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    /* set up element buffer */
    glGenBuffers(1, &cube_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    GLint pos = glGetAttribLocation(shader_program, "position");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);

    GLint col = glGetAttribLocation(shader_program, "color");
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void *) (3*sizeof(float)));
}

void make_tetra(void) {

    float verts[] = {
         1.0,  0.0, -1/sqrt(2), 1.0, 0.0, 0.0,
        -1.0,  0.0, -1/sqrt(2), 0.0, 1.0, 0.0,
         0.0,  1.0,  1/sqrt(2), 0.0, 0.0, 1.0,
         0.0, -1.0,  1/sqrt(2), 0.0, 1.0, 1.0
    };

    GLuint elems[] = {
        0, 1, 2,
        2, 3, 1,
        1, 3, 0,
        0, 2, 3
    };

    /* set up array object */
    glGenVertexArrays(1, &tetra_vao);
    glBindVertexArray(tetra_vao);

    /* copy points into GPU using buffer object */
    glGenBuffers(1, &tetra_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, tetra_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    /* set up element buffer */
    glGenBuffers(1, &tetra_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tetra_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    GLint pos = glGetAttribLocation(shader_program, "position");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);

    GLint col = glGetAttribLocation(shader_program, "color");
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void *) (3*sizeof(float)));
}

void look(void) {

    view = glm::lookAt(
            camera.get_pos(),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
    );

    GLint uniview = glGetUniformLocation(shader_program, "View");
    glUniformMatrix4fv(uniview, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 proj = glm::perspective(
            glm::radians(camera.get_fov()),
            640.f / 480.f,
            1.0f,
            10.0f
    );

    GLint uniproj = glGetUniformLocation(shader_program, "Proj");
    glUniformMatrix4fv(uniproj, 1, GL_FALSE, glm::value_ptr(proj));
}

void cursor(GLFWwindow *w, double x, double y) {

}

void keyboard(GLFWwindow *w, int k, int sc, int action, int mods) {

    float step = 0.2f;

    if (action == GLFW_RELEASE)
        return;
    
    switch (k) {
    case GLFW_KEY_Q:
        glfwSetWindowShouldClose(w, GLFW_TRUE);
    /* CAMERA MOVEMENT */
    case GLFW_KEY_W:
        camera.move(glm::vec3(step, 0, 0));
        break;
    case GLFW_KEY_S:
        camera.move(glm::vec3(-step, 0, 0));
        break;
    case GLFW_KEY_A:
        camera.move(glm::vec3(0, 0, step));
        break;
    case GLFW_KEY_D:
        camera.move(glm::vec3(0, 0, -step));
        break;
    case GLFW_KEY_LEFT_BRACKET:
        camera.fov_up(1.f);
        break;
    case GLFW_KEY_RIGHT_BRACKET:
        camera.fov_down(1.f);
        break;
    };

}

void init(void) {

    glfwSetKeyCallback(window, keyboard);
    glfwSetCursorPosCallback(window, cursor);

    glEnable(GL_DEPTH_TEST | GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    vert_source = (char *) malloc(512);
    frag_source = (char *) malloc(512);

    /* Load shaders */
    parse_shader("shader.vert", vert_source);
    parse_shader("shader.frag", frag_source);

    /* Compile the shaders */
    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_source, NULL);
    glCompileShader(vert_shader);

    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_source, NULL);
    glCompileShader(frag_shader);

    /* Check shader compilation */
    if (!error_check) {
        shader_status(vert_shader);
        shader_status(frag_shader);
    }

    /* Combine vertex and frag shader into a program */
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);

    free(vert_source);
    free(frag_source);

    view = glm::lookAt(
            camera.get_pos(),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
    );

    /* set up objects */
    make_tetra();
    make_cube();
}

void display(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    t_now = glfwGetTime();
    
    glUseProgram(shader_program);

    look();

    glm::mat4 model = glm::mat4(1.0f);
    /*
    model = glm::rotate(
            model,
            t_now * glm::radians(180.f),
            glm::vec3(1.f, 0.f, 1.f)
    );
    */

    GLint unimodel = glGetUniformLocation(shader_program, "Model");
    glUniformMatrix4fv(unimodel, 1, GL_FALSE, glm::value_ptr(model));

    GLint unicolor = glGetUniformLocation(shader_program, "triangle_color");
    glUniform3f(unicolor, sin(t_now), cos(t_now), -cos(t_now));

    glBindVertexArray(tetra_vao);
    // glDrawElements(GL_TRIANGLES, 4*3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindVertexArray(cube_vao);
    glDrawElements(GL_LINE_STRIP, 8*3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwPollEvents();
    glfwSwapBuffers(window);
}

int main(int argc, char *argv[]) {

    if (!glfwInit()) {
        fprintf(stderr, "Error: could not init GLFW3\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(640, 480, "tinyworld", NULL, NULL);

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

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported: %s\n", version);

    init();

    /* main shit */
    while (!glfwWindowShouldClose(window)) {

        display();

    }

    glfwTerminate();

    return 0;
}
