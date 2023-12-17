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
  window = glfwCreateWindow(800, 800, "SummerOpenGL", NULL, NULL);
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
  cillian_murphy_goddess = malloc(sizeof(unsigned int) * 40);
  memset(cillian_murphy_goddess, 0, sizeof(unsigned int) * 40);
  for (int i = 0; i < 40; i++)
  {
    /* slow as fuck just doing this for the sake of testing */
    if (!renderer_load_tex(&R, &*(cillian_murphy_goddess + i), "\\Batch Renderer\\Data\\Textures\\cillian_murphy.png"))
      cleanexit(-1);
  }

  while (!glfwWindowShouldClose(window))
  {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, 1);
    glClearColor(0, 1, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vertex_t q0[QUAD_VERT], q1[QUAD_VERT], q2[QUAD_VERT], q3[QUAD_VERT], q4[QUAD_VERT];
    renderer_draw_quad(&R, q0, -1.f, -0.5f, cillian_murphy_goddess[0]);
    renderer_draw_quad(&R, q1, 0.f, -0.5f, cillian_murphy_goddess[1]);
    renderer_draw_quad(&R, q2, 1.f, -0.5f, cillian_murphy_goddess[35]);
    renderer_draw_quad(&R, q3, 2.f, -0.5f, cillian_murphy_goddess[5]);
    renderer_draw_quad(&R, q4, 3.f, -0.5f, cillian_murphy_goddess[1]);

    renderer_draw(&R);
    renderer_loop_clean(&R);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  renderer_end(&R);
  free(cillian_murphy_goddess);
  cleanexit(0);
}