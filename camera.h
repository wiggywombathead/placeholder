#ifndef _CAMERA_H
#define _CAMERA_H

#include <glm/glm.hpp>

class Camera {
private:
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 up;

    float pitch, yaw, roll;
    float fov;

    void calc_dir(void);
public:
    Camera();
    void move(glm::vec3);
    glm::mat4 look(void);
    glm::vec3 get_pos(void);

    glm::vec3 get_dir(void);
    glm::vec3 get_up(void);

    void pitch_up(float);
    void pitch_down(float);
    float get_pitch(void);

    void yaw_left(float);
    void yaw_right(float);
    float get_yaw(void);

    void fov_up(float);
    void fov_down(float);
    float get_fov(void);
};

#endif
