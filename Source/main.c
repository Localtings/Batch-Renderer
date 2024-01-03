#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "obj.h"
#include <windows.h>

void cleanexit(int code)
{
	glfwTerminate();
	exit(code);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	GLFWwindow *window;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	window = glfwCreateWindow(800, 800, "Batch Renderer", NULL, NULL);
	if (window == NULL)
	{
		printf("Failed to create GLFW window");
		cleanexit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD");
		cleanexit(-1);
	}

	renderer_t R;

	if (!renderer_init(&R))
	{
		fprintf(stderr, "Could not initialize renderer\n");
		cleanexit(-1);
	}

	unsigned int cillian_murphy_goddess;

	if (!renderer_load_tex(&R, &cillian_murphy_goddess, "\\Batch Renderer\\Data\\Textures\\cillian_murphy.png"))
		cleanexit(-1);

	obj_t *cowcowmanmanthingit;
	int i;
	int size;

	cowcowmanmanthingit = malloc(sizeof(obj_t) * 25);
	i = 0;
	size = 1.f;
	for (float j = -2.f * size; j <= 2.f * size; j += size)
	{
		for (float k = -2.f * size; k <= 2.f * size; k += size)
		{
			obj_create(&(cowcowmanmanthingit[i]), &R, (vec2_t) { j, k }, (vec2_t) { size, size }, cillian_murphy_goddess);
			i++;
		}
	}

	while (!glfwWindowShouldClose(window))
	{
		mat4 thing;
		float speed;
		float m_x, m_y;
		speed = 0.01f;
		m_x = 0;
		m_y = 0;

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			m_x = -speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			m_x = speed;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			m_y = speed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			m_y = -speed;

		glClearColor(0, 1, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < 25; i++)
			obj_draw(&(cowcowmanmanthingit[i]), &R);

		obj_get_model(&(cowcowmanmanthingit[0]), &R, &thing);
		glm_translate(thing, (vec3) { m_x, m_y, 0.f });
		obj_set_model(&(cowcowmanmanthingit[0]), &R, thing);

		renderer_draw(&R);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	free(cowcowmanmanthingit);
	renderer_end(&R);
	cleanexit(0);
}