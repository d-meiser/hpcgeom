#include <basic_types.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "linmath.h"


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


#define GL_SAFE_CALL(f) do {\
	f;\
	assert(GL_NO_ERROR == glGetError());\
} while(0);

#define GL_CHECK do {\
	assert(GL_NO_ERROR == glGetError());\
} while(0);


static const char* vertex_shader_text =
"uniform mat4 mvp;\n"
"attribute vec3 vpos;\n"
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
	GLuint shader_program;
	GLint mvp_loc;
	GLint col_loc;
	GLint vpos_loc;
};

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

	return ctx;
}

void BBoxCtxDestroy(struct BBoxCtx *ctx)
{
	GL_SAFE_CALL(glDeleteBuffers(1, &ctx->vbo));
}

void BBoxDraw(
	const struct GeoBoundingBox *bbox,
	struct BBoxCtx *ctx,
	mat4x4 vp)
{
	(void)bbox;

	mat4x4 m;
	// Model matrix
        mat4x4_identity(m);

	mat4x4 mvp;
        mat4x4_mul(mvp, vp, m);

	GL_SAFE_CALL(glUseProgram(ctx->shader_program));
	GL_SAFE_CALL(glUniformMatrix4fv(ctx->mvp_loc, 1, GL_FALSE,
		(const GLfloat*)mvp));
	GL_SAFE_CALL(glUniform4f(ctx->col_loc, 0.0f, 1.0f, 0.0f, 1.0f));

	GL_SAFE_CALL(glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo));
	GL_SAFE_CALL(glEnableVertexAttribArray(ctx->vpos_loc));
	GL_SAFE_CALL(glVertexAttribPointer(ctx->vpos_loc, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 3, (void*)0));

	GL_SAFE_CALL(glDrawArrays(GL_LINES, 0, 24));
	//GL_SAFE_CALL(glDrawArrays(GL_LINE_STRIP, 0, 8));
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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void)scancode;
	(void)mods;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "Viewer", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval(1);

	GLuint program = create_program();
	GL_CHECK

	struct BBoxCtx bbox_ctx = BBoxCtxCreate(program);
	GL_CHECK

	while (!glfwWindowShouldClose(window)) {
		float ratio;
		int width, height;

		GL_CHECK
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;

		GL_SAFE_CALL(glViewport(0, 0, width, height));
		GL_SAFE_CALL(glClear(GL_COLOR_BUFFER_BIT));

		struct GeoBoundingBox bbox = {{-0.2, -0.4, 0.1}, {0.4, 0.1, 0.3}};

		mat4x4 v;
		// View
		vec3 eye = {0.2f, 0.1f, 5.0f};
		vec3 center = {0.0f, 0.0f, 0.0f};
		vec3 up = {0.0f, 1.0f, 0.0f};
		mat4x4_look_at(v, eye, center, up);

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

