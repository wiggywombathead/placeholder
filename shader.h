#ifndef _SHADER_H
#define _SHADER_H

#include <glm/glm.hpp>
#include "util.h"

class Shader {
private:
    GLuint handle;

public:
    Shader(void);
    Shader(const char *, const char *, const char * = nullptr);

    void use(void);
    GLint get_handle(void);
    GLint get_attrib(const char *);
    void set_vec3(const char *, glm::vec3);
    void set_mat4(const char *, glm::mat4);
};

#endif
