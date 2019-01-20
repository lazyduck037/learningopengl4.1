#include <GL/glew.h>	/* include GLEW and new version of GL on Windows */
#include <GLFW/glfw3.h> /* GLFW helper library */
#include <stdio.h>

const char *vertex_shader = "#version 410\n"
								"in vec3 vp;"
								"void main () {"
								"  gl_Position = vec4(vp, 1.0);"
								"}";


const char *fragment_shader = "#version 410\n"
							"out vec4 frag_colour;"
							"void main () {"
							"  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
							"}";


GLuint vao;
GLuint vbo;
GLfloat points[] = { 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f };
GLFWwindow *window = NULL;
GLuint shader_programme;



void printVersion()
{
	const GLubyte *renderer;
			const GLubyte *version;
	renderer = glGetString( GL_RENDERER ); /* get renderer string */
		version = glGetString( GL_VERSION );	 /* version as a string */
		printf( "Renderer: %s\n", renderer );
		printf( "OpenGL version supported %s\n", version );
}
void initWindow()
{

	if ( !glfwInit() ) {
		fprintf( stderr, "ERROR: could not start GLFW3\n" );
		return ;
	}

		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
		glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

		window = glfwCreateWindow( 640, 480, "Hello Triangle", NULL, NULL );
		if ( !window ) {
			fprintf( stderr, "ERROR: could not open window with GLFW3\n" );
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent( window );

		glewExperimental = GL_TRUE;
		glewInit();
}

void initBuffer()
{
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( GLfloat ), points, GL_STATIC_DRAW );
}

void initArrayBuffer()
{
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	glEnableVertexAttribArray( 0 );

	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
}

void processPipline()
{
	GLuint vert_shader, frag_shader;
	vert_shader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vert_shader, 1, &vertex_shader, NULL );
	glCompileShader( vert_shader );
	frag_shader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( frag_shader, 1, &fragment_shader, NULL );
	glCompileShader( frag_shader );
	shader_programme = glCreateProgram();
	glAttachShader( shader_programme, frag_shader );
	glAttachShader( shader_programme, vert_shader );
	glLinkProgram( shader_programme );
}

void draw()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glUseProgram( shader_programme );
	glBindVertexArray( vao );
	glDrawArrays( GL_TRIANGLES, 0, 3 );
	glfwPollEvents();
	glfwSwapBuffers( window );
}
int main() {


		initWindow();

		printVersion();


		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LESS );


		initBuffer();
		initArrayBuffer();

		processPipline();

		while ( !glfwWindowShouldClose( window ) ) {
			draw();
		}


		glfwTerminate();
		return 0;
}
