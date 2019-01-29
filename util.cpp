#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

/* read shader into buffer */
int parse_shader(const char *fname, char *buf) {

    FILE *fp = fopen(fname, "r");

    if (fp == NULL)
        return -1;

    fread(buf, sizeof(char), 512, fp);
    fclose(fp);

    return 1;
}

/* parse and compile shader */
GLuint create_shader(const char *name, char *source, GLenum type) {

    parse_shader(name, source);

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    print_shader_status(shader);

    return shader;
}

/* create shader program */
GLuint create_program(const char *vert, const char *frag, const char *geom) {
    GLuint program;
    char *source = (char *) malloc(512);

    GLuint vert_shader = create_shader(vert, source, GL_VERTEX_SHADER);
    bzero(source, 512);
    GLuint frag_shader = create_shader(frag, source, GL_FRAGMENT_SHADER);

    free(source);

    program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    return program;
}


/* print debugging info */
void print_shader_status(GLuint shader) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    char buf[512];
    glGetShaderInfoLog(shader, 512, NULL, buf);

    printf("%s", buf);
}
