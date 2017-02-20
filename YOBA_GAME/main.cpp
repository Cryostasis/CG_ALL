#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")
//#pragma comment(lib, "glew32s.lib")

//#define GLEW_STATIC
#define _USE_MATH_DEFINES



#include <GL/glew.h>
#include <GL/freeglut.h>

#include "disable_warnings.h"

#include "math/math3d.h"
#include "math/mathgl.h"
#include "error_log.h"
#include "glerrors.h"
#include "files.h"
#include "program.h"
#include "textures.h"
#include "mesh.h"
#include "light.h"
#include "camera.h"
#include "physics.h"
#include "winfuncs.h"
#include "fonts.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <queue>
#include <set>
#include <iostream>
#include <fstream>
#include <cmath>
#include <Windows.h>

using namespace std;

#define MAX_BULLETS 100
#define USE_MASSIVE_MODELS 0
#define VSYNC true

int window[2] = { 1280, 720 };
int wind_pos[2] = { 70, 0 };
int shad_size = 6;

long long last_ticks;
queue<long long> frames;

bool use_massive = USE_MASSIVE_MODELS;
int timer_ticks = 1;
GLfloat diff_angle = 0.005;
GLfloat diff_cam_angle = 0.001;
GLfloat diff_cam_pos = 0.05;
GLfloat diff_trans = 0.3;
GLfloat aspect = (float)window[0] / (float)window[1];
GLfloat fov = 45.0, znear = 0.1, zfar = 100.0;

mat4 matrixViewProjection;

bool flag_render_objects = true;
bool flag_render_normals = false;
bool flag_render_pol_mesh = false;

int glut_window;

GLuint textures[10];
int light, maters[10] = { -1 };

mesh_t		*meshes;
mesh_t		*targets;
bullet_t	bullets[MAX_BULLETS];
set<int>	light_meshes;


camera_t mainCamera;

int mesh_num;
int targ_num;
int bullet_num;

GLuint sych_tex;
GLuint cock_tex;
GLuint cat_tex;

GLuint brick_tex;
GLuint brick_n_tex;
GLuint brick_s_tex;

GLuint blank_tex;
GLuint Yyoba_tex;
GLuint Ryoba_tex;
GLuint Gyoba_tex;
GLuint cone_tex;
GLuint flat_tex;
GLuint ship_tex;
GLuint fw190_tex;
GLuint is3_tex;
GLuint is4_tex;
GLuint is7_tex;
GLuint f22_tex;
GLuint su35_tex;
GLuint t50_tex;

void reg_texture()
{
	norm_tex =	get_texture_from_tga("textures/normals.tga");

	sych_tex =	get_texture_from_tga("textures/sych.tga");
	cock_tex =	get_texture_from_tga("textures/cock.tga");

	brick_tex = get_texture_from_tga("textures/brick.tga");
	brick_n_tex = get_texture_from_tga("textures/brick_n.tga");
	brick_s_tex = get_texture_from_tga("textures/brick_s.tga");

	cat_tex =	get_texture_from_tga("textures/cat.tga");
	blank_tex = get_texture_from_tga("textures/blank.tga");
	Yyoba_tex = get_texture_from_tga("textures/Yyoba.tga");
	Ryoba_tex = get_texture_from_tga("textures/Ryoba.tga");
	Gyoba_tex = get_texture_from_tga("textures/Gyoba.tga");
	cone_tex =	get_texture_from_tga("textures/cone.tga");
	flat_tex =	get_texture_from_tga("textures/flat.tga");
	ship_tex =	get_texture_from_tga("textures/ship.tga");
	fw190_tex = get_texture_from_tga("textures/fw190.tga");
	is3_tex =	get_texture_from_tga("textures/is3.tga");
	is4_tex =	get_texture_from_tga("textures/is4.tga");
	is7_tex =	get_texture_from_tga("textures/is7.tga");
	f22_tex =	get_texture_from_tga("textures/f22.tga");
	su35_tex =	get_texture_from_tga("textures/su35.tga");
	t50_tex =	get_texture_from_tga("textures/t50.tga");
}

void scene_tact()
{
	meshes[0].rotate(0, diff_angle, 0);
	//meshes[2].rotate(diff_angle, diff_angle, diff_angle);
	
	for (int i = 0; i < MAX_BULLETS; i++)
		if (bullets[i].visible)
			bullets[i].physics_analize();
		
	for (int i = 0; i < targ_num; i++)
		for (int j = 0; j < MAX_BULLETS; j++)
			if (bullets[j].visible && bullets[j].intercept(targets[i], OT_SPHERE))
			{
				targets[i].texture = Ryoba_tex;
				targets[i].set_size(targets[i].size * 1.1);
				bullets[j].visible = false;
			}
	
	for (int i = 3; i < 6; i++)
	{
		meshes[i].rotate(diff_angle, diff_angle, diff_angle);
		meshes[i].rotate_around_y(p_data.position[0], -diff_angle);
	}

	for (int i = 0; i < targ_num; i++)
		targets[i].rotate_around_y_central(p_data.position[0], diff_angle, diff_angle); 

	if (!USE_MASSIVE_MODELS)
		return;

	for (int i = 8; i < 13; i++)
		meshes[i].rotate_around_y_central(p_data.position[0], -diff_angle, -diff_angle);

	for (int i = 13; i < 16; i++)
		meshes[i].rotate_around_y_central(p_data.position[0], diff_angle, diff_angle);
}

void activate_tex(GLuint unit, GLuint tex, char *unif_name, GLenum type)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, tex);
	GLint texloc = glGetUniformLocation(Program, unif_name);
	glUniform1i(texloc, unit);
}

void render(GLuint program, camera_t &camera)
{
	glUseProgram(program);

	if (Program == program)
	{
		activate_tex(3, p_data.depth_tex[0], "pLight_depth_tex", GL_TEXTURE_CUBE_MAP);
		activate_tex(4, d_data.depth_tex[0], "dLight_depth_tex", GL_TEXTURE_2D);
		activate_tex(5, s_data.depth_tex[0], "sLight_depth_tex", GL_TEXTURE_2D);

		setup_lights(program);
	}

	if (flag_render_objects)
	{
		for (int i = 0; i < mesh_num; i++)
		{
			if (!use_massive && USE_MASSIVE_MODELS && i == mesh_num - 8)
				break;
			if (meshes[i].visible)
			{
				if (Program != program && light_meshes.count(i) != 0)
					continue;
				meshes[i].render(program, camera);
			}
		}
		
		for (int i = 0; i < targ_num; i++)
			if (targets[i].visible)
				targets[i].render(program, camera);

		for (int i = 0; i < MAX_BULLETS; i++)
			if (bullets[i].visible)
				bullets[i].render(program, camera);
	}

	glUseProgram(line_program);

	if (flag_render_normals && program == Program)
	{
		for (int i = 0; i < mesh_num; i++)
		{
			if (!use_massive && USE_MASSIVE_MODELS && i == mesh_num - 8)
				break;
			if (meshes[i].visible)
				meshes[i].render_normals(line_program, camera);
		}

		for (int i = 0; i < targ_num; i++)
			if (targets[i].visible)
				targets[i].render_normals(line_program, camera);

		for (int i = 0; i < MAX_BULLETS; i++)
			if (bullets[i].visible)
				bullets[i].render_normals(line_program, camera);
	}

	if (flag_render_pol_mesh && program == Program)
	{
		for (int i = 0; i < mesh_num; i++)
		{
			if (!use_massive && USE_MASSIVE_MODELS && i == mesh_num - 8)
				break;
			if (meshes[i].visible)
				meshes[i].render_pol_mesh(line_program, camera);
		}

		for (int i = 0; i < targ_num; i++)
			if (targets[i].visible)
				targets[i].render_pol_mesh(line_program, camera);

		for (int i = 0; i < MAX_BULLETS; i++)
			if (bullets[i].visible)
				bullets[i].render_pol_mesh(line_program, camera);
	}
	
	if (Program == program)
	{
		glUseProgram(text_program);
		render_text();
	}
	check_GL_error();
	
	if (Program == program) 
		glutSwapBuffers();
}

void render_scene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	glViewport(0, 0, window[0], window[1]);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	render(Program, mainCamera);
}

void render_shadows(GLuint program, GLuint FBO, camera_t & camera)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glViewport(0, 0, window[0] * shad_size, window[1] * shad_size);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	render(program, camera);
}

void render_all()
{
	for (int i = 0; i < p_data.cnt; i++)
		for (int j = 0; j < 6; j++)
		{
			glUseProgram(cube_depth_program);
			glUniform3fv(8, 1, vec3(p_data.position[i]).v);
			p_data.bind_to_face(i, j);
			render_shadows(cube_depth_program, p_data.depthFBO[i], p_data.camera[i]);
		}
	for (int i = 0; i < d_data.cnt; i++)
		render_shadows(depth_program, d_data.depthFBO[i], d_data.camera[i]);
	for (int i = 0; i < s_data.cnt; i++)
		render_shadows(depth_program, s_data.depthFBO[i], s_data.camera[i]);
	
	render_scene();
}

void init_scene()
{
	mainCamera = camera_t(0.0, 1.5, 7.0);
	mainCamera.calc_perspective(fov, aspect, znear, zfar);

	mesh_num = 16;
	if (!USE_MASSIVE_MODELS)
		mesh_num -= 8;
	meshes = new mesh_t[mesh_num];

	mesh_create_from_file("objects/fig1.obj", meshes[0], vec3(0.0f, 1.0f, 1.0f), 0.5, maters[0], cock_tex);
	mesh_create_sphere(meshes[1], p_data.position[0], 0.2, maters[1], blank_tex);
	mesh_create_cube(meshes[2], vec3(-1.0f, 0.7f, 2.0f), 0.3, maters[0], brick_tex, brick_n_tex, brick_s_tex);

	for (int i = 3; i < 6; i++)
	{
		mesh_create_sphere(meshes[i], vec3(1.0f, 1.5f, 2.0f), 0.2, maters[0], sych_tex);
		meshes[i].rotate_around_y_central(p_data.position[0], M_PI * 2 * i / 3, M_PI * 2 * i / 3);
		meshes[i].visible = false;
	}
	mesh_create_from_file("objects/cone.obj", meshes[6], s_data.position[0], 0.2, maters[1], cone_tex);
	meshes[6].rotate(0.0, 0.0, -M_PI / 4);

	mesh_create_from_file("objects/flat.obj", meshes[7], vec3(0, -3, 0), 10, maters[0], blank_tex);
	//mesh_create_from_file("objects/flat.obj", meshes[7], vec3(0, -3, 0), 10, maters[2], 2);
	
	light_meshes.insert(1);
	light_meshes.insert(6);

	if (USE_MASSIVE_MODELS)
	{
		mesh_create_from_file("objects/fw190.obj", meshes[8], vec3(7, 2, 2), 0.3, maters[0], fw190_tex);
		mesh_create_from_file("objects/ship.obj", meshes[9], vec3(7, 2, 2), 0.5, maters[0], ship_tex);
		mesh_create_from_file("objects/f22.obj", meshes[10], vec3(7, 2, 2), 0.5, maters[0], f22_tex);
		mesh_create_from_file("objects/su35.obj", meshes[11], vec3(7, 2, 2), 0.2, maters[0], su35_tex);
		mesh_create_from_file("objects/t50.obj", meshes[12], vec3(7, 2, 2), 0.2, maters[0], t50_tex);
		meshes[8].rotate_around_y_central(p_data.position[0], 0, M_PI);
		meshes[9].rotate_around_y_central(p_data.position[0], M_PI * 4 / 5, M_PI * 4 / 5);
		meshes[10].rotate_around_y_central(p_data.position[0], M_PI * 8 / 5, M_PI * 8 / 5 + M_PI);
		meshes[11].rotate_around_y_central(p_data.position[0], M_PI * 12 / 5, M_PI * 12 / 5 + M_PI);
		meshes[12].rotate_around_y_central(p_data.position[0], M_PI * 16 / 5, M_PI * 16 / 5 + M_PI);

		mesh_create_from_file("objects/is4.obj", meshes[13], vec3(-6, -3, 2), 0.3, maters[0], is4_tex);
		mesh_create_from_file("objects/is7.obj", meshes[14], vec3(-6, -3, 2), 0.3, maters[0], is7_tex);
		mesh_create_from_file("objects/is3.obj", meshes[15], vec3(-6, -3, 2), 0.3, maters[0], is3_tex);
		meshes[14].rotate_around_y_central(p_data.position[0], M_PI * 2 / 3, M_PI * 2 / 3);
		meshes[15].rotate_around_y_central(p_data.position[0], M_PI * 4 / 3, M_PI * 4 / 3);
	}
	targ_num = 6;
	targets = new physical_mesh_t[targ_num];

	for (int i = 0; i < targ_num; i++)
	{
		mesh_create_from_file("objects/yoba.obj", targets[i], vec3(0.0, 2.5, 1.0), 0.2, maters[0], Yyoba_tex);
		targets[i].rotate_around_y_central(p_data.position[0], M_PI * 2 * i / targ_num, M_PI * 2 * i / targ_num + 1.12 * M_PI);
		targets[i].visible = false;
	}

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		bullet_create(bullets[i], vec3(), vec3(), maters[0], Gyoba_tex, 0.05, 300);
		bullets[i].visible = false;
	}
}

void bullet_run()
{
	for (int i = 0; i < MAX_BULLETS; i++)
		if (!bullets[i].visible)
		{
			bullets[i].look_at(
				(GLTranslation(mainCamera.position) * transpose(GLrotate(mainCamera.rotation))) * vec4(0.0, 0.0, -0.2, 1.0), mainCamera.position, vec3_y);
			bullets[i].speed = vec3(transpose(GLrotate(mainCamera.rotation)) * vec4(0.0, 0.0, -0.1, 1.0));
			bullets[i].physics_reset();
			return;
		}
}

void init_lighting()
{
	p_data.add(
		vec4(0.0f, 2.0f, 2.0f, 1.0f),
		vec4(0.1f, 0.1f, 0.1f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec4(0.03f, 0.03f, 0.03f, 0.03f),
		vec3(0.5f, 0.0f, 0.02f), 
		window, shad_size);

	d_data.add(
		vec4(-1.0f, 1.0f, 0.0f, 0.0f),
		vec4(0.1f, 0.1f, 0.1f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec4(0.03f, 0.03f, 0.03f, 0.03f),
		window, shad_size);
	
	s_data.add(
		vec4(3.0f, 4.0f, 2.0f, 1.0f),
		vec4(-1.0f, -1.0f, 0.0f, 1.0f),
		vec4(0.1f, 0.1f, 0.1f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec4(0.03f, 0.03f, 0.03f, 0.03f),
		vec3(0.09f, 0.0f, 0.015f), cosf(45.0 / 180.0 * M_PI * 2), 25.0f,
		window, shad_size);

	/*s_data.add(
		vec4(3.0f, 2.5f, 2.0f, 1.0f),
		vec4(-1.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.1f, 0.1f, 0.1f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec3(0.5f, 0.0f, 0.02f), cosf(22.5 / 180.0 * M_PI), 25.0f,
		window, shad_size);*/

	maters[0] = material_create(
		vec4(0.2f, 0.2f, 0.2f, 1.0f),
		vec4(0.3f, 0.3f, 0.3f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f),
		20.0f);
	maters[1] = material_create(
		vec4(0.2f, 0.2f, 0.2f, 1.0f),
		vec4(0.3f, 0.3f, 0.3f, 1.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f),
		vec4(199.0f, 199.0f, 199.0f, 199.0f),
		20.0f);
	maters[2] = material_create(
		vec4(0.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		0.0f);
	maters[3] = material_create(
		vec4(0.2f, 0.2f, 0.2f, 1.0f),
		vec4(0.3f, 0.3f, 0.3f, 1.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		20.0f);
}

char flag_forward, flag_back, flag_left, flag_right, flag_up, flag_down, flag_roll_l, flag_roll_r;
int flag_use_tex;

void update_pos()
{
	mainCamera.move(
		(flag_right - flag_left) * diff_cam_pos, 
		(flag_up - flag_down) * diff_cam_pos, 
		(flag_back - flag_forward) * diff_cam_pos);
}

int lx, ly;
bool mouse_flag;
bool cur_set_flag;

void on_mouse_move(int x, int y)
{
	if (mouse_flag)
	{
		mouse_flag = false;
		return;
	}
	int dx = x - lx, dy = y - ly;
	mainCamera.rotate(0.001 * dy, 0.001 * dx, 0.0);
	mouse_flag = true;
	lx = window[0] / 2; ly = window[1] / 2;
	glutWarpPointer(window[0] / 2, window[1] / 2);
	if (!cur_set_flag)
	{
		cur_set_flag = true;
		ShowCursor(0);
	}
}

void timer(int i)
{
	static bool init;
	if (!init)
		last_ticks = GetTickCount();
	init = true;

	long long new_ticks = GetTickCount();
	while (frames.size() > 0 && new_ticks - frames.front() > 1000)
		frames.pop();

	char *buf = new char[5];
	sprintf(buf, "%d", frames.size());
	prepare_text(window[0], window[1], 200, -200, buf, vec4(1.0, 1.0, 1.0, 1.0), aspect, 12);

	frames.push(new_ticks);
	last_ticks = new_ticks;

	scene_tact();
	render_all();
	update_pos();
	glutTimerFunc(timer_ticks, timer, 0);
}

void on_mouse_click(int button, int state, int x, int y)
{
	if (state)
		return;
	switch (button)
	{
	case GLUT_LEFT_BUTTON: {bullet_run(); break; }
	}
}

void on_press_key(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 32:				{ flag_up = 1; break; }
	case 'w': case 'W':		{ flag_forward = 1; break; }
	case 's': case 'S':		{ flag_back = 1; break; }
	case 'a': case 'A':		{ flag_left = 1; break; }
	case 'd': case 'D':		{ flag_right = 1; break; }
	case 'q': case 'Q':		{ flag_roll_l = 1; break; }
	case 'e': case 'E':		{ flag_roll_r = 1; break; }
	case 'g': case 'G':		{ snapshot(window[0], window[1]); break; }
	case 45:				{ diff_angle /= 2; break; }
	case 43:				{ diff_angle *= 2; break; }
	case 42:				{ diff_angle *= -1; break; }
	case 27: {exit(0); }
	}
}

void on_press_spec_key(int key, int x, int y)
{
 switch (key)
	{
	case GLUT_KEY_SHIFT_L:	{flag_down = 1; break; }
	case GLUT_KEY_F1:		{meshes[1].visible = !meshes[1].visible; toggle_point_setup(); break; }
	case GLUT_KEY_F2:		{toggle_direct_setup(); break; }
	case GLUT_KEY_F3:		{meshes[6].visible = !meshes[6].visible; toggle_spot_setup(); break; }
	case GLUT_KEY_F4:		{
								flag_use_tex = abs(flag_use_tex - 1);  
								glUseProgram(Program); glUniform1i(use_tex_loc, flag_use_tex); 
								glUseProgram(line_program); glUniform1i(use_tex_loc, flag_use_tex); 
								break; }
	case GLUT_KEY_F5:		{flag_render_normals = !flag_render_normals; break; }
	case GLUT_KEY_F6:		{flag_render_pol_mesh = !flag_render_pol_mesh; break; }
	case GLUT_KEY_F7:		{flag_render_objects = !flag_render_objects; break; }
	case GLUT_KEY_F8:		{use_massive = !use_massive; break; }
	}
}

void on_up_key(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 32:				{ flag_up = 0; break; }
	case 'w': case 'W':		{ flag_forward = 0; break; }
	case 's': case 'S':		{ flag_back = 0; break; }
	case 'a': case 'A':		{ flag_left = 0; break; }
	case 'd': case 'D':		{ flag_right = 0; break; }
	case 'q': case 'Q':		{ flag_roll_l = 0; break; }
	case 'e': case 'E':		{ flag_roll_r = 0; break; }
	}
}

void on_up_spec_key(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_SHIFT_L: {flag_down = 0; break; }
	}
}

int init_glew()
{
	GLenum glew_status = glewInit();
	if(GLEW_OK != glew_status) 
	{
		std::cout << "Error: " << glewGetErrorString(glew_status) << endl;
		return ST_GLEW_INIT_ERROR;
	}
	
	if(!GLEW_VERSION_3_3) 
	{
		std::cout << "No support for OpenGL 3.3 found\n";
		return ST_GLEW_INIT_ERROR;
	}
	return ST_OK;
}

int init()
{
	setlocale(LC_ALL, "Russian");

	int k;
	glutInit(&k, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window[0], window[1]);
	glutInitWindowPosition(wind_pos[0], wind_pos[1]);
	glutCreateWindow("CG_GL");
	glutHideWindow();

	//hwnd = GetActiveWindow();
	
	int err = init_glew();
	if (err)
		return err;
	err = init_programs();
	if (err)
		return err;

	glutDisplayFunc(render_all);
	glutTimerFunc(timer_ticks, timer, 0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	lx = window[0] / 2 + wind_pos[0];
	ly = window[1] / 2 + wind_pos[1];
	SetCursorPos(lx, ly);

	glutKeyboardFunc(on_press_key);
	glutSpecialFunc(on_press_spec_key);
	glutKeyboardUpFunc(on_up_key);
	glutSpecialUpFunc(on_up_spec_key);
	glutMotionFunc(on_mouse_move);
	glutPassiveMotionFunc(on_mouse_move);
	glutMouseFunc(on_mouse_click);

	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	return ST_OK;
}

void init_framebuffer()
{
	GLuint fbo, tex, depth_rb;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1280, 720, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	//-------------------------
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//Attach 2D texture to this FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	//-------------------------
	glGenRenderbuffers(1, &depth_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);
	//-------------------------
	//Attach depth buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
	//-------------------------
	//Does the GPU support current FBO configuration?
	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		cout << "good"; break;
	default:
		cout << "pizdets"; break;
	}
}

typedef BOOL(APIENTRY * wglSwapIntervalEXT_Func)(int);

void set_vsync(bool vs)
{
	wglSwapIntervalEXT_Func wglSwapIntervalEXT = wglSwapIntervalEXT_Func(wglGetProcAddress("wglSwapIntervalEXT"));
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(vs);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{	
	CreateConsole();
	load_screen_on(hInstance, nCmdShow);

	int err = init();
	cout << (unsigned char*)glGetString(GL_VENDOR) << endl << (unsigned char*)glGetString(GL_RENDERER) << endl;

	if (err)
	{
		load_screen_off();
		cout << error_to_string(err) << endl;
		system("pause");
		return err;
	}

	init_lighting();
	reg_texture();
	init_scene();
	init_physics(0.0, -0.001, 0.0);
	
	//cout << (unsigned char*)glGetString(GL_VENDOR) << endl << (unsigned char*)glGetString(GL_RENDERER) << endl;

	set_vsync(VSYNC);
		
	active_font("font");

	load_screen_off();
	glut_window = glutGetWindow();
	glutShowWindow();

	ShowCursor(0);
	glutMainLoop();

	return 0;
}