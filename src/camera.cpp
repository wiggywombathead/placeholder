#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {
    pos = glm::vec3(0.f, 0.f, 5.f);
    dir = glm::vec3(0.f, 0.f, -1.f);
    up = glm::vec3(0.f, 1.f, 0.f);

    pitch = 0.f;
    yaw = -M_PI/2;
    fov = 75.f;
}

void Camera::move(glm::vec3 v) {
    pos += v;
}

glm::mat4 Camera::look(void) {
    calc_dir();
    
    glm::vec3 y(0.f, 1.f, 0.f);
    glm::vec3 right = glm::normalize(glm::cross(y, dir));

    up = glm::cross(dir, right);

    return glm::lookAt(pos, pos + dir, up);
}

void Camera::calc_dir(void) {
    dir = glm::vec3(
            cos(pitch) * cos(yaw),
            sin(pitch),
            cos(pitch) * sin(yaw)
    );
    dir = glm::normalize(dir);
}

void Camera::change_pitch(float rad) {
    pitch += rad;

    if (pitch > M_PI/2 - M_PI/72)
        pitch = M_PI/2 - M_PI/72;
    
    if (pitch < -M_PI/2 + M_PI/72)
        pitch = -M_PI/2 + M_PI/72;
}

void Camera::change_yaw(float rad) {
    yaw += rad;
}

void Camera::fov_up(float amnt) {
    fov += amnt;
}

void Camera::fov_down(float amnt) {
    fov -= amnt;
}

glm::vec3 Camera::get_pos(void) { return pos; };
glm::vec3 Camera::get_dir(void) { return dir; };
glm::vec3 Camera::get_up(void) { return up; };
float Camera::get_pitch(void) { return pitch; };
float Camera::get_yaw(void) { return yaw; };
float Camera::get_fov(void) { return fov; };
