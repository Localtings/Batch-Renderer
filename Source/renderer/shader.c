#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <memory.h>

const char *vertshader_src =
"#version 330 core\n"
"layout (location = 0) in vec3 vpos;\n"
"layout (location = 1) in vec2 vtexcoord;\n"
"layout (location = 2) in float vtexid;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"

"out vec2 ftexcoord;\n"
"out float ftexid;\n"

"void main()\n"
"{\n"
"   gl_Position = proj * view * model * vec4(vpos, 1.0);\n"
"   ftexcoord = vtexcoord;\n"
"   ftexid = vtexid;\n"
"}\0";

const char *fragshader_src =
"#version 330 core\n"
"in vec2 ftexcoord;\n"
"in float ftexid;"

"uniform sampler2D texs[32];\n"

"out vec4 fo;\n"

"void main()\n"
"{\n"
"   int index = int(ftexid);\n"
"   fo = texture(texs[index], ftexcoord);\n"
"}\n\0";

int bind_shader(unsigned int id)
{
  glUseProgram(id);
  return 1;
}

int destroy_shader(unsigned int id)
{
  glDeleteProgram(id);
  return 1;
}

int init_shader(unsigned int *id)
{
  int res;
  char link_buff[512];
  unsigned int vertshader, fragshader;
  res = 1;
  vertshader = glCreateShader(GL_VERTEX_SHADER);
  fragshader = glCreateShader(GL_FRAGMENT_SHADER);
  if (!create_shader(vertshader, &vertshader_src) ||
    !create_shader(fragshader, &fragshader_src))
  {
    fprintf(stderr, "create shader error\n");
    return !res;
  }

  *id = glCreateProgram();
  glAttachShader(*id, vertshader);
  glAttachShader(*id, fragshader);
  if (!link_shader(*id, link_buff))
  {
    fprintf(stderr, "Link error: %s\n", link_buff);
    return !res;
  }
  glDeleteShader(vertshader);
  glDeleteShader(fragshader);
  bind_shader(0);
  return res;
}

int create_shader(unsigned int id, const char **src)
{
  int res;
  glShaderSource(id, 1, src, NULL);
  glCompileShader(id);
  glGetShaderiv(id, GL_COMPILE_STATUS, &res);
  return res;
}

int link_shader(unsigned int id, char *err_buffer)
{
  int res;
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &res);
  if (!res)
    glGetProgramInfoLog(id, 512, NULL, err_buffer);
  return res;
}

void set_uniform_mat4(unsigned int shader_id, const char *name, mat4 mat)
{
  glUniformMatrix4fv(glGetUniformLocation(shader_id, name), 1, GL_FALSE, mat);
}

void set_uniform_4f(unsigned int shader_id, const char *name,
  float f1, float f2, float f3, float f4)
{
  glUniform4f(glGetUniformLocation(shader_id, name), f1, f2, f3, f4);
}