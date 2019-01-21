#include "camera.h"

Camera::Camera() {
    pos = glm::vec3(0.f, 1.f, 1.f);
    fov = 45.f;
}

void Camera::move(glm::vec3 v) {
    pos += v;
}

glm::vec3 Camera::get_pos(void) {
    return pos;
}

void Camera::fov_up(float amnt) {
    fov += amnt;
}

void Camera::fov_down(float amnt) {
    fov -= amnt;
}

float Camera::get_fov(void) {
    return fov;
}
