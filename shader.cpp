#include "shader.h"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(void) { }

Shader::Shader(const char *vs, const char *fs, const char *gs) {
    handle = create_program(vs, fs, gs);
}

void Shader::use(void) {
    glUseProgram(handle);
}

GLint Shader::get_handle(void) {
    return handle;
}

GLint Shader::get_attrib(const char *var) {
    return glGetAttribLocation(handle, var);
}

void Shader::set_int(const char *var, int i) {
    GLint location = glGetUniformLocation(handle, var);
    glUniform1f(location, i);
}

void Shader::set_float(const char *var, float f) {
    GLint location = glGetUniformLocation(handle, var);
    glUniform1f(location, f);
}

void Shader::set_vec3(const char *var, glm::vec3 vec) {
    GLint location = glGetUniformLocation(handle, var);
    glUniform3fv(location, 1, glm::value_ptr(vec));
}

void Shader::set_mat4(const char *var, glm::mat4 mat) {
    GLint location = glGetUniformLocation(handle, var);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}
