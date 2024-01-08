#ifndef RENDERER_H_
#define RENDERER_H_

#define QUAD_VERT 6
#define MAX_TEX 32
#define MAX_MODEL 262144

#include "shader.h"

typedef struct
{
	float x, y;
} vec2_t;

typedef struct
{
	float x, y, z;
} vec3_t;

typedef struct
{
	vec3_t ndc;
	vec2_t tex_coord;
	float tex_index;
	float model_index;
} vertex_t;

typedef struct
{
	unsigned int ssbo;
	int model_arr_size;
	mat4 *model_arr;
	int model_index;
	mat4 view, proj;
} mvp_data;

typedef struct
{
	vertex_t *vert;
	int max_vert;
	int quad_count;
	int vert_size;
} vert_data;

typedef struct
{
	int width, height;
	unsigned int shader_id;
	unsigned int vao, vbo;
	vert_data vert_d;
	mvp_data mvp_d;
	unsigned int *texs;
	int tex_count;
} renderer_t;

int renderer_init(renderer_t *, int, int);
int renderer_draw_quad(renderer_t *, unsigned int, unsigned int);
int renderer_load_tex(renderer_t *, unsigned int *, const char *);
int renderer_push_model(renderer_t *, mat4);
int renderer_draw(renderer_t *);
int renderer_flush(renderer_t *);
int renderer_end(renderer_t *);

#endif