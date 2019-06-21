#include "gl_utils.h"
#include "maths_funcs.h"
#include <GL/glew.h>		// include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define GL_LOG_FILE "gl.log"
int g_gl_width = 640;
int g_gl_height = 480;
GLFWwindow *g_window = NULL;
#define ONE_DEG_IN_RAD ( 2.0 * M_PI ) / 360.0 // 0.017444444
// g++ -Wall -pedantic -o phong.exe main.cpp gl_utils.cpp maths_funcs.cpp -I ../common/include ../common/win32/libglew32.dll.a  ../common/win32/glfw3dll.a -lOpenGL32 -L ./ -lglew32 -lglfw3 -lm

mat4 initViewMax()
{
	/* create VIEW MATRIX */
	float cam_pos[] = { 0.0f, 0.0f,
											2.0f }; // don't start at zero, or we will be too close
	float cam_yaw = 0.0f;				// y-rotation in degrees
	mat4 T =
		translate( identity_mat4(), vec3( -cam_pos[0], -cam_pos[1], -cam_pos[2] ) );
	mat4 R = rotate_y_deg( identity_mat4(), -cam_yaw );
	mat4 view_mat = R * T;
    return view_mat;
}
int main() 
{
    printf("hello");
    restart_gl_log();
    start_gl();
    glEnable( GL_DEPTH_TEST ); 
	glDepthFunc( GL_LESS );	

    GLfloat points[] = { 0.0f, 0.5f, 0.0f, 
                            0.5f, -0.5f, 0.0f, 
                            -0.5f, -0.5f, 0.0f };

	float normals[] = {
		0.0f, 0.0f, 1.0f, 
        0.0f, 0.0f, 1.0f, 
        0.0f, 0.0f, 1.0f,
	};

    GLuint points_vbo;
    glGenBuffers( 1, &points_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
	glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), points, GL_STATIC_DRAW );

    GLuint normals_vbo;
	glGenBuffers( 1, &normals_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, normals_vbo );
	glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), normals, GL_STATIC_DRAW );


    GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	
    glBindBuffer( GL_ARRAY_BUFFER, points_vbo );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, normals_vbo );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	
    glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );


    
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
	GLfloat proj_mat[] =    { 
                                Sx, 0.0f, 0.0f, 0.0f,	
                                0.0f, Sy  , 0.0f, 0.0f, 
                                0.0f, 0.0f, Sz, -1.0f, 
                                0.0f, 0.0f, Pz,	0.0f 
                            };





    mat4 view_mat = initViewMax();
    mat4 model_mat = identity_mat4();

    GLuint shader_programme = create_programme_from_files( "test_vs.glsl", "test_fs.glsl" );
    glUseProgram( shader_programme );


	int view_mat_location = glGetUniformLocation( shader_programme, "view_mat" );
	glUniformMatrix4fv( view_mat_location, 1, GL_FALSE, view_mat.m );

	int proj_mat_location = glGetUniformLocation( shader_programme, "projection_mat" );
	glUniformMatrix4fv( proj_mat_location, 1, GL_FALSE, proj_mat );

	int model_mat_location = glGetUniformLocation( shader_programme, "model_mat" );
	glUniformMatrix4fv( model_mat_location, 1, GL_FALSE, model_mat.m );

	glEnable( GL_CULL_FACE ); // cull face
	glCullFace( GL_BACK );		// cull back face
	glFrontFace( GL_CW );			// GL_CCW for counter clock-wise



    while ( !glfwWindowShouldClose( g_window ) ) 
    {
		_update_fps_counter( g_window );
		double current_seconds = glfwGetTime();

		// wipe the drawing surface clear
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glViewport( 0, 0, g_gl_width, g_gl_height );

		glUseProgram( shader_programme );

		model_mat.m[12] = sinf( current_seconds );
		glUniformMatrix4fv( model_mat_location, 1, GL_FALSE, model_mat.m );

		glBindVertexArray( vao );
		// draw points 0-3 from the currently bound VAO with current in-use shader
		glDrawArrays( GL_TRIANGLES, 0, 3 );
		// update other events like input handling
		glfwPollEvents();
		if ( GLFW_PRESS == glfwGetKey( g_window, GLFW_KEY_ESCAPE ) ) {
			glfwSetWindowShouldClose( g_window, 1 );
		}
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers( g_window );
	}
    // close GL context and any other GLFW resources
	glfwTerminate();
    return 1;
}