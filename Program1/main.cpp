#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "objloader.hpp"
#include "texture.hpp"
#include "CameraControls.h"

#include "glsl.h"

using namespace std;


//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 800, HEIGHT = 600;
const char * fragshader_name = "fragmentshader.fsh";
const char * vertexshader_name = "vertexshader.vsh";
unsigned const int DELTA = 10;

//--------------------------------------------------------------------------------
// typedef
//--------------------------------------------------------------------------------

struct LightSource
{
	glm::vec3 position;
};

struct Material
{
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
	float power;
};

//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

GLuint program_id;
GLuint vao[2];
GLuint uniform_mv;
GLuint texture_id[2];

CameraControls* camera;

glm::vec3 specular;

glm::mat4 model[2], view, projection;
glm::mat4 mv[2];

LightSource light;
Material material[2];




//--------------------------------------------------------------------------------
// Mesh variables
//--------------------------------------------------------------------------------

vector<glm::vec3> vertices[2];
vector<glm::vec3> normals[2];
vector<glm::vec2> uvs[2];


//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void keyboardHandler(unsigned char key, int a, int b)
{
	if (key == 27)
		glutExit();
}


//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------

void Render()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	view = camera->viewMatrix;

	for (int i = 0; i < 2; i++) {
		// Send vao
		glBindVertexArray(vao[i]);
		glDrawArrays(GL_TRIANGLES, 0, vertices[i].size());
		glBindVertexArray(0);

		// Do transformation
		model[i] = glm::rotate(model[i], 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
		mv[i] = view * model[i];

		// Send mvp
		glUseProgram(program_id);
		glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv[i]));
	}
	glutSwapBuffers();
}



//------------------------------------------------------------
// void Render(int n)
// Render method that is called by the timer function
//------------------------------------------------------------

void Render(int n)
{
	Render();

	glutTimerFunc(DELTA, Render, 0);
}


//------------------------------------------------------------
// void InitGlutGlew(int argc, char **argv)
// Initializes Glut and Glew
//------------------------------------------------------------

void InitGlutGlew(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Hello OpenGL");
	glutDisplayFunc(Render);
	glutKeyboardFunc(keyboardHandler);
	glutTimerFunc(DELTA, Render, 0);

	glewInit();
}


//------------------------------------------------------------
// void InitShaders()
// Initializes the fragmentshader and vertexshader
//------------------------------------------------------------

void InitShaders()
{
	char * vertexshader = glsl::readFile(vertexshader_name);
	GLuint vsh_id = glsl::makeVertexShader(vertexshader);

	char * fragshader = glsl::readFile(fragshader_name);
	GLuint fsh_id = glsl::makeFragmentShader(fragshader);

	program_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}


//------------------------------------------------------------
// void InitMatrices()
//------------------------------------------------------------

void InitMatrices()
{
	for (int i = 0; i < 2; i++) {
		model[i] = glm::mat4();
		view = glm::mat4();
		projection = glm::perspective(
			glm::radians(45.0f),
			1.0f * WIDTH / HEIGHT, 0.1f,
			20.0f);
	}
	model[1] = glm::translate(model[1], glm::vec3(0.0, 0.0, -10.0));
	model[0] = glm::translate(model[0], glm::vec3(0.0, 0.0, -11.0));

}

//------------------------------------------------------------
// void InitObjects()
//------------------------------------------------------------
void InitObjects()
{
	bool res = loadOBJ("Objects/teapot.obj", vertices[1], uvs[1], normals[1]);
	bool res2 = loadOBJ("Objects/Porsche_911_GT2.obj", vertices[0], uvs[0], normals[0]);

	//bool res = loadOBJ("Objects/BWM X5 4.obj", vertices, uvs, normals);
	//texture_id = loadDDS("Textures/uvmap.DDS");
	//texture_id = loadBMP("Textures/uvtemplate.bmp");
	texture_id[0] = loadBMP("Textures/Yellobrk.bmp");
	texture_id[1] = loadBMP("Textures/uvmap.DDS");
}

//------------------------------------------------------------
// InitMaterialsLight()
//------------------------------------------------------------
void InitMaterialsLight()
{
	light.position = glm::vec3(4.0, 4.0, 4.0);
	material[0].ambient_color = glm::vec3(0.2, 0.2, 0.0);
	material[0].diffuse_color = glm::vec3(0.5, 0.5, 0.0);
	material[0].power = 128;
	material[1].ambient_color = glm::vec3(0.3, 0.1, 0.1);
	material[1].diffuse_color = glm::vec3(0.5, 0.5, 0.4);
	material[1].power = 128;
	specular = glm::vec3(1.0);
}

//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------

void InitBuffers()
{
	GLuint position_id;
	GLuint normal_id;
	GLuint vbo_vertices[2];
	GLuint vbo_normals[2];
	GLuint vbo_uvs[2];

	for (int i = 0; i < 2; i++) {
		// vbo for vertices
		glGenBuffers(1, &vbo_vertices[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices[i]);
		glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(glm::vec3), &(vertices[i][0]), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo for vertices
		glGenBuffers(1, &vbo_normals[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals[i]);
		glBufferData(GL_ARRAY_BUFFER, normals[i].size() * sizeof(glm::vec3), &(normals[i][0]), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo for uvs
		glGenBuffers(1, &vbo_uvs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs[i]);
		glBufferData(GL_ARRAY_BUFFER, uvs[i].size() * sizeof(glm::vec2),
			&uvs[i][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint uv_id = glGetAttribLocation(program_id, "uv");

		// Get vertex attributes
		position_id = glGetAttribLocation(program_id, "position");
		normal_id = glGetAttribLocation(program_id, "normal");

		// Allocate memory for vao
		glGenVertexArrays(1, &vao[i]);

		// Bind to vao
		glBindVertexArray(vao[i]);

		// Bind vertices to vao
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices[i]);
		glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(position_id);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Bind vertices to vao
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals[i]);
		glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(normal_id);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Bind uvs to vao
		glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs[i]);
		glVertexAttribPointer(uv_id, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(uv_id);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Stop bind to vao
		glBindVertexArray(0);

		glGenBuffers(1, &vbo_normals[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals[i]);
		glBufferData(GL_ARRAY_BUFFER,
			normals[i].size() * sizeof(glm::vec3),
			&normals[i][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint normalID = glGetAttribLocation(program_id, "normal");

		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals[i]);
		glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(normalID);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Make uniform vars
		uniform_mv = glGetUniformLocation(program_id, "mv");
		GLuint uniform_proj = glGetUniformLocation(program_id, "projection");
		GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
		GLuint uniform_material_ambient = glGetUniformLocation(program_id,
			"mat_ambient");
		GLuint uniform_material_diffuse = glGetUniformLocation(program_id,
			"mat_diffuse");

		GLuint uniform_specular = glGetUniformLocation(program_id, "mat_specular");
		GLuint uniform_material_power = glGetUniformLocation(program_id, "mat_power");

		glUseProgram(program_id);

		// Fill uniform vars
		glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv[i]));
		glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(uniform_light_pos, 1, glm::value_ptr(light.position));
		glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(material[i].ambient_color));
		glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(material[i].diffuse_color));
		glUniform3fv(uniform_specular, 1, glm::value_ptr(specular));
		glUniform1f(uniform_material_power, material[i].power);

		// Fill uniform var
		glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv[i]));
	}


}

void InitCamera() {
	camera = new CameraControls();
}

void handleKeyboardInput(unsigned char key, int x, int y) {
	camera->CaptureKeys(key, x, y);
}

void handleMouseInput(int button, int state, int x, int y) {
	camera->CaptureMouse(button, state, x, y);
}

void handleMouseMove(int x, int y) {
	camera->CaptureMouseMove(x, y);
}


int main(int argc, char ** argv)
{
	InitGlutGlew(argc, argv);
	InitMaterialsLight();
	InitShaders();
	InitMatrices();
	InitObjects();
	InitBuffers();
	InitCamera();

	glutKeyboardFunc(handleKeyboardInput);
	glutMouseFunc(handleMouseInput);
	glutMotionFunc(handleMouseMove);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);

	glutMainLoop();

	return 0;
}
