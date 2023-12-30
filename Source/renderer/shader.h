#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>
#include <cglm/cglm.h>

int bind_shader(unsigned int);
int init_shader(unsigned int *);
int create_shader(unsigned int, const char**);
int link_shader(unsigned int, char*);
int destroy_shader(unsigned int);

void set_uniform_mat4(unsigned int, const char *, mat4);
void set_uniform_4f(unsigned int, const char*, float, float, float, float);

#endif