#include "math/math3d.h"
#include "math/mathgl.h"
#include "tinyxml.h"
#include "glerrors.h"
#include "error_log.h"
#include "textures.h"
#include "files.h"
#include "loadObject.h"
#include "mesh.h"
#include "program.h"
#include "camera.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <map>
#include <vector>

#define FONT_MULTIPLYER 200

struct chr
{
	char C;
	int x, y;
	int width, height;
	g_object obj;
};

GLuint font_tex = -1;
std::map<char, chr> font_map;
std::vector<mesh_t> font_mesh;
camera_t font_camera;
vec4 font_color;

const int tex_size = 512;

void active_font(char *file)
{
	char *name = new char[100];
	name[0] = 0;
	sprintf(name, "textures/%s.tga", file);

	font_tex = get_texture_from_tga(name);
	
	name[0] = 0;
	sprintf(name, "%s.xml", file);

	TiXmlDocument *doc = new TiXmlDocument(name);
	if (!doc->LoadFile())
		return;

	TiXmlElement *font = doc->FirstChildElement("font");
	TiXmlElement *elem = font->FirstChildElement("char");

	while (elem != NULL)
	{
		const char* id = elem->Attribute("id");
		const char* x = elem->Attribute("x");
		const char* y = elem->Attribute("y");
		const char* w = elem->Attribute("width");
		const char* h = elem->Attribute("height");
		chr buf;

		sscanf(id, "%d", &buf.C);
		sscanf(x, "%d", &buf.x);
		sscanf(y, "%d", &buf.y);
		sscanf(w, "%d", &buf.width);
		sscanf(h, "%d", &buf.height);

		buf.obj = clone_obj("objects/quad.obj");
		
		GLfloat x1 = GLfloat(buf.x) / tex_size, x2 = GLfloat(buf.x + buf.width) / tex_size;
		GLfloat y2 = GLfloat(buf.y) / tex_size, y1 = GLfloat(buf.y + buf.height) / tex_size;

		buf.obj.texcoords[0] = x1; buf.obj.texcoords[1] = y1;
		buf.obj.texcoords[2] = x2; buf.obj.texcoords[3] = y1;
		buf.obj.texcoords[4] = x2; buf.obj.texcoords[5] = y2;
		buf.obj.texcoords[6] = x1; buf.obj.texcoords[7] = y2;
		buf.obj.texcoords[8] = x1; buf.obj.texcoords[9] = y1;
		buf.obj.texcoords[10] = x2; buf.obj.texcoords[11] = y2;

		font_map[buf.C] = buf;

		elem = elem->NextSiblingElement("char");
	}
	
	font_camera = camera_t();

	delete[] name;
}

void prepare_text(int wnd_w, int wnd_h, int X, int Y, char *text, vec4 color, float aspect, float scale)
{
	font_color = color;
	font_mesh.clear();
	font_mesh.resize(strlen(text));
	int i = 0;
	GLfloat x = X - wnd_w, y = Y + wnd_h;
	y -= (GLfloat)font_map[text[0]].height / 2;
	while (text[i] != 0)
	{
		x += (GLfloat)font_map[text[i]].width / 2;
		float chr_aspect = (GLfloat)font_map[text[i]].width / font_map[text[i]].height;
		font_mesh[i] = mesh_t(
			vec3((GLfloat)x / wnd_w, (GLfloat)y / wnd_h, 0),
			vec3(scale * chr_aspect / aspect, scale, scale) / FONT_MULTIPLYER,
			0, font_tex, &font_map[text[i]].obj);
		font_mesh[i].rotate(M_PI / 2, 0, 0);
		i++;
		x += (GLfloat)font_map[text[i]].width / 2;
	}
}

void render_text()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glUniform4fv(LINE_COLOR_LOC, 1, font_color.v);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font_tex);
	glUniform1i(FONT_TEX, 0);
	for (int i = 0; i < font_mesh.size(); i++)
		font_mesh[i].render(text_program, font_camera);
	glEnable(GL_DEPTH_TEST);
}