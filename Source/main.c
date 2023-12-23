#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "renderer/renderer.h"
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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

	unsigned int *cillian_murphy_goddess;
	int i;

	cillian_murphy_goddess = malloc(sizeof(unsigned int) * 36);
	memset(cillian_murphy_goddess, 0, sizeof(unsigned int) * 36);
	/* slow as fuck just doing this for the sake of testing */
	for (i = 0; i < 36; i++)
		if (!renderer_load_tex(&R, &cillian_murphy_goddess[i], "\\Batch Renderer\\Data\\Textures\\cillian_murphy.png"))
			cleanexit(-1);


	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);
		glClearColor(0, 1, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float j, k;
		int i;

		i = 0;
		for (k = -3.5f; k <= 1.5f; k++)
		{
			for (j = -3.f; j <= 2.f; j++)
			{

				renderer_draw_quad(&R, j, k, cillian_murphy_goddess[i]);
				i++;
			}
		}

		renderer_draw(&R);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	renderer_end(&R);
	free(cillian_murphy_goddess);
	cleanexit(0);
}