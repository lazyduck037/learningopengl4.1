#include "utils/utils.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "math/maths_funcs.h"
#include <math.h>
#define _USE_MATH_DEFINES

int g_gl_width = 640;
int g_gl_height = 480;
GLFWwindow *g_window = NULL;
GLuint vao;
float cam_speed = 1.0f;			 // 1 unit per second
float cam_yaw_speed = 10.0f; // 10 degrees per second
float cam_pos[] = { 0.0f, 0.0f,2.0f };
float cam_yaw = 0.0f;
GLint view_mat_location;

GLuint processFragment()
{
	char vertex_shader[1024 * 256];
	char fragment_shader[1024 * 256];
	parse_file_into_str("/resource/test_vs.glsl", vertex_shader, 1024 * 256 );
	parse_file_into_str("/resource/test_fs.glsl", fragment_shader, 1024 * 256 );


	GLuint vs = glCreateShader( GL_VERTEX_SHADER );
	const GLchar *p = (const GLchar *)vertex_shader;
	glShaderSource( vs, 1, &p, NULL );
	glCompileShader( vs );

	// check for compile errors
	int params = -1;
	glGetShaderiv( vs, GL_COMPILE_STATUS, &params );
	if ( GL_TRUE != params ) {
		fprintf( stderr, "ERROR: GL shader index %i did not compile\n", vs );
		print_shader_info_log( vs );
		return -1; // or exit or something
	}

	GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
	p = (const GLchar *)fragment_shader;
	glShaderSource( fs, 1, &p, NULL );
	glCompileShader( fs );

	// check for compile errors
	glGetShaderiv( fs, GL_COMPILE_STATUS, &params );
	if ( GL_TRUE != params ) {
		fprintf( stderr, "ERROR: GL shader index %i did not compile\n", fs );
		print_shader_info_log( fs );
		return -1; // or exit or something
	}

	GLuint shader_programme = glCreateProgram();
	glAttachShader( shader_programme, fs );
	glAttachShader( shader_programme, vs );
	glLinkProgram( shader_programme );

	glGetProgramiv( shader_programme, GL_LINK_STATUS, &params );
	if ( GL_TRUE != params )
	{
		fprintf( stderr, "ERROR: could not link shader programme GL index %i\n",
						 shader_programme );
		print_programme_info_log( shader_programme );
		return -1;
	}

	return shader_programme;
}

void initMVP(GLuint shader_programme)
{
	/*--------------------------create camera
	 * matrices----------------------------*/
	/* create PROJECTION MATRIX */
	// input variables
	float near = 0.1f;									// clipping plane
	float far = 100.0f;									// clipping plane
	float fov = 67.0f * ONE_DEG_IN_RAD; // convert 67 degrees to radians
	float aspect = (float)g_gl_width / (float)g_gl_height; // aspect ratio
	// matrix components
	float range = tan( fov * 0.5f ) * near;
	float Sx = ( 2.0f * near ) / ( range * aspect + range * aspect );
	float Sy = near / range;
	float Sz = -( far + near ) / ( far - near );
	float Pz = -( 2.0f * far * near ) / ( far - near );
	GLfloat proj_mat[] = { Sx,	 0.0f, 0.0f, 0.0f,	0.0f, Sy,		0.0f, 0.0f,
												 0.0f, 0.0f, Sz,	 -1.0f, 0.0f, 0.0f, Pz,		0.0f };

	/* create VIEW MATRIX */

	 // don't start at zero, or we will be too close
	float cam_yaw = 0.0f;				// y-rotation in degrees
	mat4 T = translate( identity_mat4(), vec3( -cam_pos[0], -cam_pos[1], -cam_pos[2] ) );
	mat4 R = rotate_y_deg( identity_mat4(), -cam_yaw );
	mat4 view_mat = R * T;

	/* get location numbers of matrices in shader programme */
	view_mat_location = glGetUniformLocation( shader_programme, "view" );
	GLint proj_mat_location = glGetUniformLocation( shader_programme, "proj" );

	glUseProgram( shader_programme );
	glUniformMatrix4fv( view_mat_location, 1, GL_FALSE, view_mat.m );
	glUniformMatrix4fv( proj_mat_location, 1, GL_FALSE, proj_mat );
}

void initBuffer()
{

	GLfloat points[] = { 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f };
	GLfloat colours[] = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };


	GLuint points_vbo;
	glGenBuffers( 1, &points_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
	glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), points, GL_STATIC_DRAW );


	GLuint colours_vbo;
	glGenBuffers( 1, &colours_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, colours_vbo );
	glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), colours, GL_STATIC_DRAW );


	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glBindBuffer( GL_ARRAY_BUFFER, colours_vbo );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );

}

void draw(GLuint shader_programme)
{
	static double previous_seconds = glfwGetTime();
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	previous_seconds = current_seconds;
	_update_fps_counter( g_window );
			// wipe the drawing surface clear
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glViewport( 0, 0, g_gl_width, g_gl_height );

	glUseProgram( shader_programme );
	glBindVertexArray( vao );
	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawArrays( GL_TRIANGLES, 0, 3 );
	// update other events like input handling
	glfwPollEvents();
	//key event

	bool cam_moved = false;
	if ( glfwGetKey( g_window, GLFW_KEY_A ) ) {
		cam_pos[0] -= cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if ( glfwGetKey( g_window, GLFW_KEY_D ) ) {
		cam_pos[0] += cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if ( glfwGetKey( g_window, GLFW_KEY_PAGE_UP ) ) {
		cam_pos[1] += cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if ( glfwGetKey( g_window, GLFW_KEY_PAGE_DOWN ) ) {
		cam_pos[1] -= cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if ( glfwGetKey( g_window, GLFW_KEY_W ) ) {
		cam_pos[2] -= cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if ( glfwGetKey( g_window, GLFW_KEY_S ) ) {
		cam_pos[2] += cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if ( glfwGetKey( g_window, GLFW_KEY_LEFT ) ) {
		cam_yaw += cam_yaw_speed * elapsed_seconds;
		cam_moved = true;
	}
	if ( glfwGetKey( g_window, GLFW_KEY_RIGHT ) ) {
		cam_yaw -= cam_yaw_speed * elapsed_seconds;
		cam_moved = true;
	}
	/* update view matrix */
	if ( cam_moved )
	{
		mat4 T = translate( identity_mat4(), vec3( -cam_pos[0], -cam_pos[1], -cam_pos[2] ) ); // cam translation
		mat4 R = rotate_y_deg( identity_mat4(), -cam_yaw );					//
		mat4 view_mat = R * T;
		glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m );
	}


	if ( GLFW_PRESS == glfwGetKey( g_window, GLFW_KEY_ESCAPE ) ) {
		glfwSetWindowShouldClose( g_window, 1 );
	}

	// put the stuff we've been drawing onto the display
	glfwSwapBuffers( g_window );
}


int main()
{
	//init windown contex and create logfil to debug
	restart_gl_log();
	start_gl();


	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );

	initBuffer();
	GLuint shader_programme = processFragment();
	initMVP(shader_programme);

	glEnable( GL_CULL_FACE ); // cull face
	glCullFace( GL_BACK );		// cull back face
	glFrontFace( GL_CW );			// GL_CCW for counter clock-wise


	while ( !glfwWindowShouldClose( g_window ) ) {

		draw(shader_programme);
	}

	glfwTerminate();
	return 0;
}
