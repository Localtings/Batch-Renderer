#ifndef RENDERER_H_
#define RENDERER_H_

#define QUAD_VERT 6
#define MAX_TEX 32

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
	vec3_t pos;
	vec2_t tex_coord;
	float tex_index;
} vertex_t;

typedef struct
{
	mat4 model, view, proj;
} coords_data;

typedef struct
{
	vertex_t *vert;
	int max_vert;
	int quad_count;
	int vert_size;
} vert_data;

typedef struct
{
	unsigned int shader_id;
	unsigned int vao, vbo;
	vert_data vert_d;
	coords_data coords_d;
	unsigned int *texs;
	int tex_count;
} renderer_t;

int renderer_init(renderer_t *);
int renderer_draw_quad(renderer_t *, float, float, unsigned int);
int renderer_load_tex(renderer_t *, unsigned int *, const char *);
int renderer_draw(renderer_t *);
int renderer_flush(renderer_t *);
int renderer_end(renderer_t *);

#endif