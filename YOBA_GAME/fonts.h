#ifndef _FONTS_H_INC
#define _FONTS_H_INC

#include "math/math3d.h"
#include "math/mathgl.h"

void active_font(char *file);
void prepare_text(int wnd_w, int wnd_h, int X, int Y, char *text, vec4 color, float aspect, float scale);
void render_text();

#endif