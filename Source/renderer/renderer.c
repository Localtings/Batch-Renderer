#include "renderer.h"
#include "../utils/bool.h"
#include <memory.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int renderer_init_vao(renderer_t *R_p)
{
	glGenVertexArrays(1, &(R_p->vao));
	glGenBuffers(1, &(R_p->vbo));
	glBindVertexArray(R_p->vao);

	glBindBuffer(GL_ARRAY_BUFFER, R_p->vbo);
	glBufferData(GL_ARRAY_BUFFER, R_p->vert_d.vert_size, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, ndc));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, tex_coord));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, tex_index));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, model_index));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return TRUE;
}

int renderer_init_mvp(renderer_t *R_p)
{
	R_p->mvp_d.model_index = 0;
	R_p->mvp_d.model_arr_size = sizeof(mat4) * MAX_MODEL;
	R_p->mvp_d.model_arr = malloc(R_p->mvp_d.model_arr_size);
	glGenBuffers(1, &(R_p->mvp_d.ssbo));
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, R_p->mvp_d.ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, R_p->mvp_d.model_arr_size, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, R_p->mvp_d.ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glm_mat4_identity(R_p->mvp_d.view);
	glm_mat4_identity(R_p->mvp_d.proj);
	glm_translate(R_p->mvp_d.view, (vec3) { 0.f, 0.f, -10.f });
	glm_perspective(45.f, 800.f / 600.f, 0.1f, 100.f, R_p->mvp_d.proj);
	return TRUE;
}

int renderer_init_vertices(renderer_t *R_p)
{
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &(R_p->vert_d.max_vert));
	R_p->vert_d.vert_size = sizeof(vertex_t) * R_p->vert_d.max_vert;
	R_p->vert_d.vert = malloc(R_p->vert_d.vert_size);
	memset(R_p->vert_d.vert, 0, R_p->vert_d.vert_size);
	R_p->vert_d.quad_count = 0;
	return TRUE;
}

int renderer_init_textures(renderer_t *R_p)
{
	int tex_size;
	tex_size = sizeof(unsigned int) * MAX_TEX;
	R_p->texs = malloc(tex_size);
	memset(R_p->texs, 0, tex_size);
	R_p->tex_count = 0;
	stbi_set_flip_vertically_on_load(1);
	return TRUE;
}


int renderer_init(renderer_t *R_p)
{
	if (!init_shader(&(R_p->shader_id)))
		return FALSE;
	renderer_init_vertices(R_p);
	renderer_init_vao(R_p);
	renderer_init_mvp(R_p);
	renderer_init_textures(R_p);
	return TRUE;
}

int renderer_draw_call(renderer_t *R_p)
{
	glBindVertexArray(R_p->vao);
	glDrawArrays(GL_TRIANGLES, 0, QUAD_VERT * R_p->vert_d.quad_count);
	return TRUE;
}

int renderer_set_samplers(renderer_t *R_p)
{
	unsigned int texs_loc;
	int samplers[32];
	int i;

	for (i = 0; i < 32; i++)
		samplers[i] = i;
	texs_loc = glGetUniformLocation(R_p->shader_id, "texs");
	glUniform1iv(texs_loc, 32, samplers);
	return TRUE;
}

int renderer_submit_mvp(renderer_t *R_p)
{
	set_uniform_mat4(R_p->shader_id, "view", R_p->mvp_d.view);
	set_uniform_mat4(R_p->shader_id, "proj", R_p->mvp_d.proj);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, R_p->mvp_d.ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, R_p->mvp_d.model_arr_size, R_p->mvp_d.model_arr);
	return TRUE;
}

int renderer_push_model(renderer_t *R_p, mat4 model)
{
	if (R_p->mvp_d.model_index + 1 > MAX_MODEL)
		return FALSE;
	memcpy(R_p->mvp_d.model_arr + R_p->mvp_d.model_index, model, sizeof(mat4));
	R_p->mvp_d.model_index++;
	return TRUE;
}

int renderer_push_quad2vert(renderer_t *R_p, vertex_t *quad)
{
	int curr_vert;
	curr_vert = QUAD_VERT * R_p->vert_d.quad_count;
	if (curr_vert + QUAD_VERT > R_p->vert_d.max_vert)
		return FALSE;
	memcpy(R_p->vert_d.vert + curr_vert, quad, sizeof(vertex_t) * QUAD_VERT);
	R_p->vert_d.quad_count++;
	return TRUE;
}

int renderer_submit_textures(renderer_t *R_p)
{
	int i;
	for (i = 0; i < R_p->tex_count; i++)
		glBindTextureUnit(i, *(R_p->texs + i));
	return TRUE;
}

int renderer_flush(renderer_t *R_p)
{
	R_p->vert_d.quad_count = 0;
	memset(R_p->vert_d.vert, 0, R_p->vert_d.vert_size);
	memset(R_p->texs, 0, MAX_TEX * sizeof(unsigned int));
	R_p->tex_count = 0;
	return TRUE;
}

int renderer_draw_quad(renderer_t *R_p, unsigned int tex_id, unsigned int model_index)
{
	vertex_t res[QUAD_VERT];
	float tex_index;
	int tex_find;
	int i;
	tex_find = 0;

	for (i = 0; i < R_p->tex_count; i++)
	{
		if (*(R_p->texs + i) == tex_id)
		{
			tex_index = i;
			tex_find = 1;
			break;
		}
	}

	if (!tex_find)
	{
		if (R_p->tex_count > MAX_TEX - 1)
			renderer_draw(R_p);
		*(R_p->texs + R_p->tex_count) = tex_id;
		tex_index = R_p->tex_count;
		R_p->tex_count++;
	}

	res[0].ndc = (vec3_t){ 0.f, 0.f, 0.f };
	res[0].tex_coord = (vec2_t){ 0.0f, 0.0f };
	res[0].tex_index = tex_index;
	res[0].model_index = model_index;

	res[1].ndc = (vec3_t){ 1.f, 0.f, 0.f };
	res[1].tex_coord = (vec2_t){ 1.0f, 0.0f };
	res[1].tex_index = tex_index;
	res[1].model_index = model_index;

	res[2].ndc = (vec3_t){ 1.f, 1.f, 0.f };
	res[2].tex_coord = (vec2_t){ 1.0f, 1.0f };
	res[2].tex_index = tex_index;
	res[2].model_index = model_index;

	res[3].ndc = (vec3_t){ 1.f, 1.f, 0.f };
	res[3].tex_coord = (vec2_t){ 1.0f, 1.0f };
	res[3].tex_index = tex_index;
	res[3].model_index = model_index;

	res[4].ndc = (vec3_t){ 0.f, 1.f, 0.f };
	res[4].tex_coord = (vec2_t){ 0.0f, 1.0f };
	res[4].tex_index = tex_index;
	res[4].model_index = model_index;

	res[5].ndc = (vec3_t){ 0.f, 0.f, 0.f };
	res[5].tex_coord = (vec2_t){ 0.0f, 0.0f };
	res[5].tex_index = tex_index;
	res[5].model_index = model_index;

	return renderer_push_quad2vert(R_p, res);
}

int renderer_submit_vert(renderer_t *R_p)
{
	glBindBuffer(GL_ARRAY_BUFFER, R_p->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, R_p->vert_d.vert_size, R_p->vert_d.vert);
	return TRUE;
}

int renderer_load_tex(renderer_t *R_p, unsigned int *tex_p, const char *tex_dir)
{
	int width, height, color_channel;
	unsigned char *data;
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load(tex_dir, &width, &height, &color_channel, 0);
	if (!data)
	{
		fprintf(stderr, "Failed to load texture");
		return FALSE;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	memcpy(tex_p, &tex, sizeof(unsigned int));
	return TRUE;
}

int renderer_draw(renderer_t *R_p)
{
	renderer_submit_vert(R_p);
	bind_shader(R_p->shader_id);
	renderer_set_samplers(R_p);
	renderer_submit_mvp(R_p);
	renderer_submit_textures(R_p);
	renderer_draw_call(R_p);
	renderer_flush(R_p);
	return TRUE;
}

int renderer_end(renderer_t *R_p)
{
	free(R_p->vert_d.vert);
	free(R_p->mvp_d.model_arr);
	glDeleteVertexArrays(1, &(R_p->vao));
	glDeleteBuffers(1, &(R_p->vbo));
	destroy_shader(R_p->shader_id);
	return TRUE;
}