#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "objloader.hpp"
#include "CameraControls.h"
#include "TextureController.h"
#include "AnimationController.h"
#include "Animation.h"

#include "glsl.h"

using namespace std;


//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------
const int amountOfObjects = 7;
const int WIDTH = 800, HEIGHT = 600;
const char * fragshader_name = "fragmentshader.frag";
const char * vertexshader_name = "vertexshader.vert";
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
GLuint vao[amountOfObjects];
GLuint uniform_mv, uniform_material_ambient, uniform_material_diffuse, uniform_specular, uniform_material_power;
GLuint textures[amountOfObjects];

CameraControls* camera;
TextureController* textureController;
AnimationController* animationController;

glm::vec3 specular;

glm::mat4 model[amountOfObjects], view, projection;
glm::mat4 mv[amountOfObjects];

LightSource light;
Material material[amountOfObjects];
Animation animation[amountOfObjects];




//--------------------------------------------------------------------------------
// Mesh variables
//--------------------------------------------------------------------------------

vector<glm::vec3> vertices[amountOfObjects];
vector<glm::vec3> normals[amountOfObjects];
vector<glm::vec2> uvs[amountOfObjects];


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

	for (int i = 0; i < amountOfObjects; i++) {

		// Send mvp
		glUseProgram(program_id);

		// Do transformation
		if (i == 0) {
			model[i] = animationController->AnimateCar(model[i], &animation[i]);
		}
		if (i == 5) {
			model[i] = animationController->AnimateTrashBin(model[i], &animation[i]);
		}
		mv[i] = view * model[i];


		glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv[i]));
		glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(material[i].ambient_color));
		glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(material[i].diffuse_color));
		glUniform3fv(uniform_specular, 1, glm::value_ptr(specular));
		glUniform1f(uniform_material_power, material[i].power);

		glBindTexture(GL_TEXTURE_2D, textures[i]);
		// Send vao
		glBindVertexArray(vao[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glDrawArrays(GL_TRIANGLES, 0, vertices[i].size());
		glBindVertexArray(0);
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
	glutCreateWindow("Project Graphics");
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
	for (int i = 0; i < amountOfObjects; i++) {
		model[i] = glm::mat4();
		view = glm::mat4();
		projection = glm::perspective(
			glm::radians(45.0f),
			1.0f * WIDTH / HEIGHT, 0.1f,
			200.0f);
	}
	model[0] = glm::translate(model[0], glm::vec3(0.0, 0.0, -11.0));
	model[1] = glm::translate(model[1], glm::vec3(0.0, -1.0, -5.0));
	model[2] = glm::translate(model[2], glm::vec3(0.0, -1.0, 0.0));
	model[3] = glm::translate(model[3], glm::vec3(15.0, -1.0, 0.0));
	model[4] = glm::translate(model[4], glm::vec3(50.0, -1.0, -4.0));
	model[5] = glm::translate(model[5], glm::vec3(0.0, -1.0, 20.0));
	model[6] = glm::translate(model[5], glm::vec3(0.0, 0.0, -10.0));


}

//------------------------------------------------------------
// void InitObjects()
//------------------------------------------------------------
void InitObjects()
{
	loadOBJ("Objects/Porsche_911_GT2.obj", vertices[0], uvs[0], normals[0]);
	loadOBJ("Objects/teapot.obj", vertices[1], uvs[1], normals[1]);
	loadOBJ("Objects/Terrain.obj", vertices[2], uvs[2], normals[2]);
	loadOBJ("Objects/Basic_tree.obj", vertices[3], uvs[3], normals[3]);
	loadOBJ("Objects/Basic_house_joined.obj", vertices[4], uvs[4], normals[4]);
	loadOBJ("Objects/trash_can.obj", vertices[5], uvs[5], normals[5]);
	loadOBJ("Objects/postbus.obj", vertices[6], uvs[6], normals[6]);
}

//------------------------------------------------------------
// InitMaterialsLight()
//------------------------------------------------------------
void InitMaterialsLight()
{
	light.position = glm::vec3(4.0, 20.0, 4.0);
	material[0].ambient_color = glm::vec3(0.0, 0.0, 0.0);
	material[0].diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material[0].power = 128;
	material[1].ambient_color = glm::vec3(0.3, 0.1, 0.1);
	material[1].diffuse_color = glm::vec3(0.5, 0.5, 0.4);
	material[1].power = 128;
	material[2].ambient_color = glm::vec3(0.0, 0.0, 0.0);
	material[2].diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material[2].power = 68;
	material[3].ambient_color = glm::vec3(0.0, 0.0, 0.0);
	material[3].diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material[3].power = 68;
	material[4].ambient_color = glm::vec3(0.0, 0.0, 0.0);
	material[4].diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material[4].power = 68;	
	material[5].ambient_color = glm::vec3(0.0, 0.0, 0.0);
	material[5].diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material[5].power = 68;
	material[6].ambient_color = glm::vec3(0.0, 0.0, 0.0);
	material[6].diffuse_color = glm::vec3(0.0, 0.0, 0.0);
	material[6].power = 128;
	specular = glm::vec3(0.8);
}

//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------

void InitBuffers()
{
	GLuint position_id;
	GLuint normal_id;
	GLuint vbo_vertices[amountOfObjects];
	GLuint vbo_normals[amountOfObjects];
	GLuint vbo_uvs[amountOfObjects];

	for (int i = 0; i < amountOfObjects; i++) {
		// vbo for vertices
		glGenBuffers(1, &vbo_vertices[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices[i]);
		glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(glm::vec3), &(vertices[i][0]), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo for normals
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

		// Make uniform vars
		uniform_mv = glGetUniformLocation(program_id, "mv");
		GLuint uniform_proj = glGetUniformLocation(program_id, "projection");
		GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
		uniform_material_ambient = glGetUniformLocation(program_id,
			"mat_ambient");
		uniform_material_diffuse = glGetUniformLocation(program_id,
			"mat_diffuse");

		uniform_specular = glGetUniformLocation(program_id, "mat_specular");
		uniform_material_power = glGetUniformLocation(program_id, "mat_power");

		glUseProgram(program_id);

		// Fill uniform vars
		glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv[i]));
		glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(uniform_light_pos, 1, glm::value_ptr(light.position));
		glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(material[i].ambient_color));
		glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(material[i].diffuse_color));
		glUniform3fv(uniform_specular, 1, glm::value_ptr(specular));
		glUniform1f(uniform_material_power, material[i].power);
	}


}

void InitControllers() {
	camera = new CameraControls();
	textureController = new TextureController();
	animationController = new AnimationController();
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
	InitControllers();
	InitMaterialsLight();
	InitShaders();
	InitMatrices();
	InitObjects();
	InitBuffers();
	textureController->LoadTextures(textures);

	glEnable(GL_TEXTURE_2D);


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
