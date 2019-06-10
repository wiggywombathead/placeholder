#ifndef _UTIL_H
#define _UTIL_H

#define SHADER_SIZE (1 << 20)

#include <GL/glew.h>

int parse_shader(const char *fname, char *buf);
GLuint create_shader(const char *name, char *source, GLenum type);
GLuint create_program(const char *vert, const char *frag, const char *geom = nullptr);
void print_shader_status(GLuint shader);
GLuint make_texture(const char *path);

/*
unsigned long getFileLength(std::ifstream& file) {
    if (!file.good())
        return 0;

    unsigned long pos=file.tellg();
    file.seekg(0, ios::end);
    unsigned long len = file.tellg();
    file.seekg(ios::beg);

    return len;
}

int loadshader(char *filename, GLchar **source, unsigned long *len) {

    std::ifstream file;
    file.open(filename, ios::in); // opens as ASCII!
    if (!file)
        return -1;

    len = getFileLength(file);

    if (len == 0)
        return -2;   // Error: Empty File 

    *source = (GLubyte *) new char[len+1];
    if (*source == 0)
        return -3;   // can't reserve memory

    // len isn't always strlen cause some characters are stripped in ascii read...
    // it is important to 0-terminate the real length later, len is just max possible value... 
    *source[len] = 0; 

    unsigned int i=0;
    while (file.good()) {
        *source[i] = file.get();       // get character from file.

        if (!file.eof())
            i++;
    }

    *source[i] = 0;  // 0-terminate it at the correct position

    file.close();

    return 0; // No Error
}


int unloadshader(GLubyte **source) {
    if (*source != 0)
        delete[] *source;
    *source = 0;
}
*/

#endif
