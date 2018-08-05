#ifndef CAMERA_H
#define CAMERA_H

#include "linmath.h"
#include <stdint.h>
#include <GLFW/glfw3.h>


static const uint8_t kForward =     0x1 << 0;
static const uint8_t kBackward =    0x1 << 1;
static const uint8_t kLeftStrafe =  0x1 << 2;
static const uint8_t kRightStrafe = 0x1 << 3;

struct Camera {
        vec3 position;
        vec3 rotation;
        vec3 speed;
        double movement_speed_factor;
        double pitch_sensitivity;
        double yaw_sensitivity;
        uint8_t holding;
};

void CameraInitialize(struct Camera *camera);
void CameraHandleMouseMove(struct Camera *camera, GLFWwindow* window);
void CameraHandleKeyPress(struct Camera *camera, int key, int action);
void CameraMove(struct Camera *camera, double dt);

#endif
