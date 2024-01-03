#include "obj.h"
#include "utils/bool.h"
#include "renderer/renderer.h"
#include <memory.h>

int obj_create(obj_t *obj_p, renderer_t *R_p, vec2_t pos, vec2_t size, unsigned int tex_id)
{
	int curr_vert;
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3) { { pos.x }, { pos.y }, 0.f });
	glm_scale(model, (vec3) { size.x, size.y, 0.f });
	obj_p->destroyed = 0;
	obj_p->solid = 0;
	obj_p->tex_id = tex_id;
	obj_p->model_index = R_p->mvp_d.model_index;
	return renderer_push_model(R_p, model);
}

int obj_draw(obj_t *obj_p, renderer_t *R_p)
{
	return renderer_draw_quad(R_p, obj_p->tex_id, obj_p->model_index);
}

int obj_get_model(obj_t *obj_p, renderer_t *R_p, mat4 *model_p)
{
	memcpy(model_p, R_p->mvp_d.model_arr + obj_p->model_index, sizeof(mat4));
	return TRUE;
}

int obj_set_model(obj_t *obj_p, renderer_t *R_p, mat4 model)
{
	memcpy(R_p->mvp_d.model_arr + obj_p->model_index, model, sizeof(mat4));
	return TRUE;
}