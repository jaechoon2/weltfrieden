#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <pthread.h>
#include <math.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text.h"

typedef struct t_character {
  GLuint texid;
  GLuint res[2];
  GLuint bearing[2];
  GLuint advance;
} character;

#define MAXCHARS 512

character characters[MAXCHARS];

character t_char;

extern float res[2];
extern GLuint* texfbo;
extern GLuint sampler;
extern double now;
extern GLuint vao;

typedef struct {
  float x, y, z;    // position
  float s, t;       // texture
  float r, g, b, a; // color
} vertex_t;

GLuint text_vshader = 0;
GLuint text_shader = 0;


void textlayer_init(layer* l) {
  FT_Library ft;

  if (FT_Init_FreeType(&ft)) {
    log_err("[text:init] freetype could not be initialized\n");
    exit(1);
  }

  FT_Face face;
  if (FT_New_Face(ft, "fonts/VeraMono.ttf", 0, &face)) {
    log_err("[text:init] could not be loaded\n");
    exit(1);
  }

  float fontsize = fmin(l->scale, 400);

  FT_Set_Pixel_Sizes(face, 0, fontsize);

  if (FT_Load_Char(face, l->text->text[0], FT_LOAD_RENDER)) {
    log_err("[text:init] failed loading char\n");
    exit(1);
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 8-bit gray scale bitmap coming up!

  GLuint texture;

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RED,
    face->glyph->bitmap.width,
    face->glyph->bitmap.rows,
    0,
    GL_RED,
    GL_UNSIGNED_BYTE,
    face->glyph->bitmap.buffer
    );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  l->shaderid = texture;
  l->text->res[0] = face->glyph->bitmap.width;
  l->text->res[1] = face->glyph->bitmap.rows;
  l->text->bearing[0] = face->glyph->bitmap_left;
  l->text->bearing[1] = face->glyph->bitmap_top;
  l->text->advance = face->glyph->advance.x;


  GLfloat w = l->text->res[0] / res[0];
  GLfloat h = l->text->res[1] / res[0];

  GLfloat xpos = ( (l->text->bearing[0]) / fontsize ) + l->pos[0] - (w/2);
  GLfloat ypos = ((l->pos[1] - (l->text->res[1] - l->text->bearing[1])) / fontsize) + (l->pos[1]) - (h/2);

  GLfloat vertices[6][4] = {
    { xpos, ypos + h, 0.0, 0.0 },
    { xpos, ypos, 0.0, 1.0 },
    { xpos + w, ypos, 1.0, 1.0 },
    { xpos, ypos + h, 0.0, 0.0 },
    { xpos + w, ypos, 1.0, 1.0 },
    { xpos + w, ypos + h, 1.0, 0.0 }
  };

  log_info("[text:res] %f,%f %f,%f", xpos, ypos, w, h);

  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glGenVertexArrays(1, &l->vao);
  glGenBuffers(1, &l->vbo);
  glBindVertexArray(l->vao);
  glBindBuffer(GL_ARRAY_BUFFER, l->vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  l->progid = glCreateProgram();
  glAttachShader(l->progid, text_vshader);
  glAttachShader(l->progid, text_shader);
  glLinkProgram(l->progid);
}

void textlayer_add(t_showargs args) {
  layer* l = textlayer_new();

  if (l == NULL) {
    log_err("[text:new] hit max (%d)\n", MAXSHADERLAYERS);
    return;
  }

  l->text->text = strdup(args.words);

  layer_init(l, &args);
  layer_add(l);
}


void textlayer_apply(layer* l, int even) {
  glUseProgram(l->progid);
  glUniform3f( glGetUniformLocation(l->progid, "text_color"), l->color[0], l->color[1], l->color[2] );
  glUniform2fv( glGetUniformLocation(l->progid, "res"), 1, res);
  uarg(l, "elapsed", now - l->when);
  uarg(l, "cps", l->cps);
  uarg(l, "dur", l->duration);

  /* glBindSampler(1, sampler); */
  /* glUniform1i( glGetUniformLocation(l->progid, "fbotex"), 1); */
  /* glActiveTexture(GL_TEXTURE1); */
  /* glBindTexture(GL_TEXTURE_2D, texfbo[even]); */

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(l->vao);
  glBindTexture(GL_TEXTURE_2D, l->shaderid);

  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void textlayer_finish(layer* l) {
}

layer *textlayer_new() {
  layer *l = layer_new();
  if (l == NULL) {
    return(NULL);
  }

  l->text = malloc(sizeof(textlayer));
  l->is_text = 1;

  return(l);
}