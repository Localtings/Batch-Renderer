#include "renderer.h"
#include <glad/glad.h>
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, pos));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, tex_coord));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(vertex_t), offsetof(vertex_t, tex_index));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return 1;
}

int renderer_init_coords(renderer_t *R_p)
{
  glm_mat4_identity(R_p->coords_d.model);
  glm_mat4_identity(R_p->coords_d.view);
  glm_mat4_identity(R_p->coords_d.proj);
  glm_translate(R_p->coords_d.view, (vec3) { 0.f, 0.f, -10.f });
  glm_perspective(45.f, 800.f / 600.f, 0.1f, 100.f, R_p->coords_d.proj);
  return 1;
}

int renderer_init_vertices(renderer_t *R_p)
{
  glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &(R_p->vert_d.max_vert));
  R_p->vert_d.vert_size = sizeof(vertex_t) * R_p->vert_d.max_vert;
  R_p->vert_d.vert = malloc(R_p->vert_d.vert_size);
  memset(R_p->vert_d.vert, 0, R_p->vert_d.vert_size);
  R_p->vert_d.quad_count = 0;
  return 1;
}

int renderer_init_textures(renderer_t *R_p)
{
  int tex_size;
  tex_size = sizeof(unsigned int) * MAX_TEX;
  R_p->texs = malloc(tex_size);
  memset(R_p->texs, 0, tex_size);
  R_p->tex_count = 0;
  stbi_set_flip_vertically_on_load(1);
  return 1;
}


int renderer_init(renderer_t *R_p)
{
  if (!init_shader(&(R_p->shader_id)))
    return 0;
  renderer_init_vertices(R_p);
  renderer_init_vao(R_p);
  renderer_init_coords(R_p);
  renderer_init_textures(R_p);
  R_p->batch_count = 0;
  return 1;
}

int renderer_draw_call(renderer_t *R_p)
{
  glBindVertexArray(R_p->vao);
  glDrawArrays(GL_TRIANGLES, 0, QUAD_VERT * R_p->vert_d.quad_count);
  return 1;
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
  return 1;
}

int renderer_submit_mvp(renderer_t *R_p)
{
  set_uniform_mat4(R_p->shader_id, "model", R_p->coords_d.model);
  set_uniform_mat4(R_p->shader_id, "view", R_p->coords_d.view);
  set_uniform_mat4(R_p->shader_id, "proj", R_p->coords_d.proj);
  return 1;
}

int renderer_push_quad2vert(renderer_t *R_p, vertex_t *quad)
{
  int curr_vert;
  curr_vert = QUAD_VERT * R_p->vert_d.quad_count;
  if (curr_vert + QUAD_VERT > R_p->vert_d.max_vert)
    return 0;
  memcpy(R_p->vert_d.vert + curr_vert, quad, sizeof(vertex_t) * QUAD_VERT);
  R_p->vert_d.quad_count++;
  return 1;
}

int renderer_submit_textures(renderer_t *R_p)
{
  int i;
  for (i = 0; i < R_p->tex_count; i++)
    glBindTextureUnit(i, *(R_p->texs + i));
  return 1;
}

int renderer_flush(renderer_t *R_p)
{
  R_p->vert_d.quad_count = 0;
  memset(R_p->vert_d.vert, 0, R_p->vert_d.vert_size);
  memset(R_p->texs, 0, MAX_TEX * sizeof(unsigned int));
  R_p->tex_count = 0;
  return 1;
}

int renderer_draw_quad(renderer_t *R_p, vertex_t *quad, float x, float y, unsigned int tex_id)
{
  vertex_t res[QUAD_VERT];
  float size;
  float tex_index;
  int tex_find;
  int i;
  size = 1.f;
  tex_find = 0;

  if (tex_id > (R_p->batch_count + 1) * MAX_TEX)
  {
    R_p->batch_count++;
    renderer_draw(R_p);
  }

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
    *(R_p->texs + R_p->tex_count) = tex_id;
    tex_index = R_p->tex_count;
    R_p->tex_count++;
  }

  res[0].pos = (vec3_t){ x, y, 0.f };
  res[0].tex_coord = (vec2_t){ 0.0f, 0.0f };
  res[0].tex_index = tex_index;

  res[1].pos = (vec3_t){ x + size, y, 0.f };
  res[1].tex_coord = (vec2_t){ 1.0f, 0.0f };
  res[1].tex_index = tex_index;

  res[2].pos = (vec3_t){ x + size, y + size, 0.f };
  res[2].tex_coord = (vec2_t){ 1.0f, 1.0f };
  res[2].tex_index = tex_index;

  res[3].pos = (vec3_t){ x + size, y + size, 0.f };
  res[3].tex_coord = (vec2_t){ 1.0f, 1.0f };
  res[3].tex_index = tex_index;

  res[4].pos = (vec3_t){ x, y + size, 0.f };
  res[4].tex_coord = (vec2_t){ 0.0f, 1.0f };
  res[4].tex_index = tex_index;

  res[5].pos = (vec3_t){ x, y, 0.f };
  res[5].tex_coord = (vec2_t){ 0.0f, 0.0f };
  res[5].tex_index = tex_index;

  memcpy(quad, res, sizeof(res));
  return renderer_push_quad2vert(R_p, quad);
}

int renderer_submit_vert(renderer_t *R_p)
{
  glBindBuffer(GL_ARRAY_BUFFER, R_p->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, R_p->vert_d.vert_size, R_p->vert_d.vert);
  return 1;
}

int renderer_load_tex(renderer_t * R_p, unsigned int *tex_p, const char *tex_dir)
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
    return 0;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);
  *tex_p = tex;
  return 1;
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
  return 1;
}

int renderer_loop_clean(renderer_t *R_p)
{
  R_p->batch_count = 0;
  return 1;
}

int renderer_end(renderer_t *R_p)
{
  free(R_p->vert_d.vert);
  glDeleteVertexArrays(1, &(R_p->vao));
  glDeleteBuffers(1, &(R_p->vbo));
  destroy_shader(R_p->shader_id);
  return 1;
}