#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SOIL/SOIL.h>

/* read shader into buffer */
int parse_shader(const char *fname, char *buf) {

    FILE *fp = fopen(fname, "r");

    if (fp == NULL)
        return -1;

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    fread(buf, sizeof(char), size, fp);
    fclose(fp);

    return 1;
}

/* parse and compile shader */
GLuint create_shader(const char *name, char *source, GLenum type) {

    parse_shader(name, source);

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    printf("Compiling %s\n", name);
    // print_shader_status(shader);

    return shader;
}

/* create shader program */
GLuint create_program(const char *vert, const char *frag, const char *geom) {
    GLuint program;
    char source[SHADER_SIZE];

    /* just to be sure */
    bzero(source, SHADER_SIZE);

    GLuint vert_shader = create_shader(vert, source, GL_VERTEX_SHADER);
    // printf("\nVERTEX\n%s\n", source);
    bzero(source, SHADER_SIZE);
    GLuint frag_shader = create_shader(frag, source, GL_FRAGMENT_SHADER);
    // printf("\nFRAGMENT\n%s\n", source);
    bzero(source, SHADER_SIZE);

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

GLuint make_texture(const char *path) {
    GLuint tex;
    glGenTextures(1, &tex);

    /* generate texture */
    int width, height, components;
    unsigned char *img = SOIL_load_image(
            path,
            &width,
            &height,
            &components,
            0
    );

    GLenum format;
    switch (components) {
    case 1: format = GL_RED; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}

