#include <basic_types.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <linmath.h>
#include <camera.h>


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static GLint check_err()
{
	GLint err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("err == %d\n", err);
	}
	return err;
}

#define GL_SAFE_CALL(f) do {\
	f;\
	assert(GL_NO_ERROR == check_err());\
} while(0);

#define GL_CHECK do {\
	assert(GL_NO_ERROR == check_err());\
} while(0);


static const char* vertex_shader_text =
"#version 150\n"
"uniform mat4 mvp;\n"
"in vec3 vpos;\n"
"void main()\n"
"{\n"
"    gl_Position = mvp * vec4(vpos, 1.0);\n"
"}\n";

static const char* fragment_shader_text =
"uniform vec4 col;\n"
"void main()\n"
"{\n"
"    gl_FragColor = col;\n"
"}\n";

struct BBoxCtx {
	GLuint vbo;
	GLuint vao;
	GLuint shader_program;
	GLint mvp_loc;
	GLint col_loc;
	GLint vpos_loc;
};


struct Camera camera;


struct BBoxCtx BBoxCtxCreate(GLuint shader_program)
{
	struct BBoxCtx ctx;

	float verts[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f
	};
//	float verts[3 * 8];
//	int l = 0;
//	for (int i = 0; i < 2; ++i) {
//		for (int j = 0; j < 2; ++j) {
//			for (int k = 0; k < 2; ++k) {
//				verts[l * 3 + 0] = -0.5f + (float)i;
//				verts[l * 3 + 1] = -0.5f + (float)j;
//				verts[l * 3 + 2] = -0.5f + (float)k;
//				++l;
//			}
//		}
//	}
//
	GL_SAFE_CALL(glGenVertexArrays(1, &ctx.vao));
	GL_SAFE_CALL(glBindVertexArray(ctx.vao));

	GL_SAFE_CALL(glGenBuffers(1, &ctx.vbo));
	GL_SAFE_CALL(glBindBuffer(GL_ARRAY_BUFFER, ctx.vbo));
	GL_SAFE_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts,
		GL_STATIC_DRAW));

	ctx.shader_program = shader_program;
	GL_SAFE_CALL(glUseProgram(shader_program));
	ctx.mvp_loc = glGetUniformLocation(ctx.shader_program, "mvp");
	GL_CHECK
	ctx.col_loc = glGetUniformLocation(ctx.shader_program, "col");
	GL_CHECK
	ctx.vpos_loc = glGetAttribLocation(ctx.shader_program, "vpos");
	GL_CHECK

	GL_SAFE_CALL(glEnableVertexAttribArray(ctx.vpos_loc));
	GL_SAFE_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(float), 0));

	return ctx;
}

void BBoxCtxDestroy(struct BBoxCtx *ctx)
{
	GL_SAFE_CALL(glDeleteVertexArrays(1, &ctx->vao));
	GL_SAFE_CALL(glDeleteBuffers(1, &ctx->vbo));
}

void BBoxDraw(
	const struct GeoBoundingBox *bbox,
	struct BBoxCtx *ctx,
	mat4x4 vp)
{
	(void)bbox;

	// Model matrix
	mat4x4 m;
        mat4x4_identity(m);
	mat4x4_scale_aniso(m, m,
			   bbox->max.x - bbox->min.x,
			   bbox->max.y - bbox->min.y,
			   bbox->max.z - bbox->min.z);
	mat4x4_translate_in_place(m,
			   0.5 * (bbox->max.x + bbox->min.x),
			   0.5 * (bbox->max.y + bbox->min.y),
			   0.5 * (bbox->max.z + bbox->min.z));

	mat4x4 mvp;
        mat4x4_mul(mvp, vp, m);

	GL_SAFE_CALL(glUseProgram(ctx->shader_program));
	GL_SAFE_CALL(glUniformMatrix4fv(ctx->mvp_loc, 1, GL_FALSE,
		(const GLfloat*)mvp));
	GL_SAFE_CALL(glUniform4f(ctx->col_loc, 0.0f, 1.0f, 0.0f, 1.0f));

	GL_SAFE_CALL(glBindVertexArray(ctx->vao));

	GL_SAFE_CALL(glDrawArrays(GL_LINES, 0, 24));
}

GLuint create_program()
{
	GLuint vertex_shader, fragment_shader, program;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GL_SAFE_CALL(glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL));
	GL_SAFE_CALL(glCompileShader(vertex_shader));

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	GL_SAFE_CALL(glShaderSource(fragment_shader, 1, &fragment_shader_text,
		NULL));
	GL_SAFE_CALL(glCompileShader(fragment_shader));

	program = glCreateProgram();
	GL_SAFE_CALL(glAttachShader(program, vertex_shader));
	GL_SAFE_CALL(glAttachShader(program, fragment_shader));
	GL_SAFE_CALL(glLinkProgram(program));

	return program;
}

static void error_callback(int error, const char* description)
{
	(void)error;
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode,
	int action, int mods)
{
	(void)scancode;
	(void)mods;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	CameraHandleKeyPress(&camera, key, action);
}

static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	CameraHandleMouseMove(&camera, window, x, y);
}

int main(void)
{
	CameraInitialize(&camera);
	camera.position[0] = 0.3;
	camera.position[1] = 0.0;
	camera.position[2] = 4.0;
	camera.rotation[0] = 0.0;
	camera.rotation[1] = 85.0;

	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	window = glfwCreateWindow(640, 480, "Viewer", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval(1);

	GL_SAFE_CALL(glEnable(GL_LINE_SMOOTH));
	GL_SAFE_CALL(glHint(GL_LINE_SMOOTH_HINT, GL_NICEST));
	GL_SAFE_CALL(glEnable(GL_BLEND));
	GL_SAFE_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	GLuint program = create_program();
	GL_CHECK

	struct BBoxCtx bbox_ctx = BBoxCtxCreate(program);
	GL_CHECK

	double t = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		float ratio;
		int width, height;

		GL_CHECK
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;

		GL_SAFE_CALL(glViewport(0, 0, width, height));
		GL_SAFE_CALL(glClear(GL_COLOR_BUFFER_BIT));

		struct GeoBoundingBox bbox = {{-0.9, -0.9, -0.9}, {0.9, 0.9, 0.9}};

		double t1 = glfwGetTime();
		CameraMove(&camera, t1 - t);
		t = t1;

		mat4x4 v;
		mat4x4_identity(v);
		// View
		mat4x4_rotate_X(v, v, camera.rotation[0]);
		mat4x4_rotate_Y(v, v, camera.rotation[1]);
		mat4x4_translate_in_place(v,
			camera.position[0],
			camera.position[1],
			camera.position[2]);

		// Projection
		mat4x4 p;
		mat4x4_perspective(p, 45.0f * (float)M_PI / 180.0f, ratio,
		1.0e-2f, 10.0f);

		mat4x4 vp;
		mat4x4_mul(vp, p, v);
		BBoxDraw(&bbox, &bbox_ctx, vp);

		glfwSwapBuffers(window);
		glfwPollEvents();
		GL_CHECK
	}

	BBoxCtxDestroy(&bbox_ctx);

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

