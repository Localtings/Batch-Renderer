#ifndef OBJ_H_
#define OBJ_H_

#include "renderer/renderer.h"

typedef struct
{
	int	destroyed, solid;
	unsigned int tex_id;
	unsigned int model_index;
} obj_t;

int obj_create(obj_t *, renderer_t *, vec2_t, vec2_t, unsigned int);
int obj_draw(obj_t *, renderer_t *);
int obj_get_model(obj_t *, renderer_t *, mat4 *);
int obj_set_model(obj_t *, renderer_t *, mat4);

#endif