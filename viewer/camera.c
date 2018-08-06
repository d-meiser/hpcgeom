#include <camera.h>
#include <math.h>
#include <stdio.h>


static void vec3_set_zero(vec3 *v)
{
	(*v)[0] = 0.0;
	(*v)[1] = 0.0;
	(*v)[2] = 0.0;
}

static void vec2_set_zero(vec2 *v)
{
	(*v)[0] = 0.0;
	(*v)[1] = 0.0;
}

void CameraInitialize(struct Camera *camera)
{
	vec3_set_zero(&camera->position);
	vec2_set_zero(&camera->rotation);
	camera->movement_speed_factor = 3.0;
	camera->pitch_sensitivity = 1.0e-3;
	camera->yaw_sensitivity = 1.0e-3;
	camera->holding = 0x0;
}

void CameraHandleMouseMove(struct Camera *camera, GLFWwindow* window,
	double x, double y)
{
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	int mid_x = width / 2;
	int mid_y = height / 2;
	double dx = (x - mid_x) * camera->yaw_sensitivity;
	double dy  = (y - mid_y) * camera->pitch_sensitivity;
	camera->rotation[0] -= dy;
	camera->rotation[1] -= dx;
	camera->rotation[0] = fmax(camera->rotation[0], -90.0);
	camera->rotation[0] = fmin(camera->rotation[0], 90.0);
	if (camera->rotation[1] < 0.0) camera->rotation[1] += 360.0;
	if (camera->rotation[1] > 360.0) camera->rotation[1] -= 360.0;
	glfwSetCursorPos(window, mid_x, mid_y);
}

static double to_rads(double x)
{
	return x * M_PI / 180.0;
}

/*
static void vec3_print(vec3 v)
{
	for (int i = 0; i < 3; ++i) {
		printf("%lf ", v[i]);
	}
	printf("\n");
}
*/

void CameraMove(struct Camera *camera, double dt)
{
	vec3 movement;
	vec3_set_zero(&movement);

	double sin_x = sin(to_rads(camera->rotation[0]));
	double cos_x = cos(to_rads(camera->rotation[0]));
	double sin_y = sin(to_rads(camera->rotation[1]));
	double cos_y = cos(to_rads(camera->rotation[1]));

 	// This cancels out moving on the Z axis when we're looking up or down
	double pitch_limit_factor = cos_x;
	if (camera->holding & kForward) {
		movement[0] += sin_y * pitch_limit_factor;
		movement[1] += -sin_x;
		movement[2] += -cos_y * pitch_limit_factor;
	}
	if (camera->holding & kBackward) {
		movement[0] += -sin_y * pitch_limit_factor;
		movement[1] += sin_x;
		movement[2] += cos_y * pitch_limit_factor;
	}
	if (camera->holding & kLeftStrafe) {
		movement[0] += -cos_y;
		movement[2] += -sin_y;
	}
	if (camera->holding & kRightStrafe) {
		movement[0] += cos_y;
		movement[2] += sin_y;
	}
	if (vec3_len(movement) < 1.0e-9) return;
	vec3_norm(movement, movement);
	vec3_scale(movement, movement, dt * camera->movement_speed_factor);
	vec3_add(camera->position, camera->position, movement);
}

void CameraHandleKeyPress(struct Camera *camera, int key, int action)
{
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_W:
			camera->holding |= kForward;
			break;
		case GLFW_KEY_S:
			camera->holding |= kBackward;
			break;
		case GLFW_KEY_A:
			camera->holding |= kLeftStrafe;
			break;
		case GLFW_KEY_D:
			camera->holding |= kRightStrafe;
			break;
		default:
			break;
		}
	} else if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_W:
			camera->holding &= ~kForward;
			break;
		case GLFW_KEY_S:
			camera->holding &= ~kBackward;
			break;
		case GLFW_KEY_A:
			camera->holding &= ~kLeftStrafe;
			break;
		case GLFW_KEY_D:
			camera->holding &= ~kRightStrafe;
			break;
		default:
			break;
		}
	}
}

void CameraPrint(struct Camera *camera)
{
	printf("%lf %lf %lf %lf %lf\n",
	       camera->position[0],
	       camera->position[1],
	       camera->position[2],
	       camera->rotation[0],
	       camera->rotation[1]);
}
