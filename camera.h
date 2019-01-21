#ifndef _CAMERA_H
#define _CAMERA_H

#include <glm/glm.hpp>

class Camera {
private:
    glm::vec3 pos;
    float fov;
public:
    Camera();
    void move(glm::vec3);
    glm::vec3 get_pos(void);
    void fov_up(float);
    void fov_down(float);
    float get_fov(void);
};

#endif
