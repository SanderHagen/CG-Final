#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CameraControls.h"
#include "TextureController.h"
#include "AnimationController.h"
#include "Animation.h"
#include "Material.h"
#include "ObjectWrapper.h"

#include "glsl.h"

using namespace std;


//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------
const int amountOfObjects = 9;
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
ObjectWrapper* objectWrapper;

glm::vec3 specular;

glm::mat4 model[amountOfObjects], view,overview, projection;
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

	if (camera->firstpersonmode) {
		view = camera->viewMatrix;
	}
	else {
		view = camera->overviewMat;
	}

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
	view = glm::mat4();
	for (int i = 0; i < amountOfObjects; i++) {
		model[i] = glm::mat4();
		projection = glm::perspective(
			glm::radians(45.0f),
			1.0f * WIDTH / HEIGHT, 0.1f,
			200.0f);
	}
}

//------------------------------------------------------------
// void InitObjects()
// method to add a new object
// 2 steps to add object.
// 1: copy a line below, fill in the parameters accordingly.
// 2: increase the constant "amountOfObjects by 1"
//------------------------------------------------------------
void InitObjects()
{
	//AddObject Signature (const char* path, glm::mat4* model,int objectNumber, glm::vec3 pos, vector<glm::vec3>* verts, vector<glm::vec3>* normals, vector<glm::vec2>* uvs, Material* material, glm::vec3 ambient_color, glm::vec3 diffuse_color, float power)
	objectWrapper->AddObject("Objects/Porsche_911_GT2.obj", model, 0, glm::vec3(2.0, -0.2, 0.0), vertices, normals, uvs, material, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 128);
	objectWrapper->AddObject("Objects/teapot.obj", model, 1, glm::vec3(-8.0, -1.0, -10.0), vertices, normals, uvs, material, glm::vec3(0.8, 0.5, 0.5), glm::vec3(0.8, 0.5, 0.4), 128);
	objectWrapper->AddObject("Objects/Terrain.obj", model, 2, glm::vec3(0.0, -1.0, 0.0), vertices, normals, uvs, material, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 68);
	objectWrapper->AddObject("Objects/Basic_tree.obj", model, 3, glm::vec3(15.0, -1.0, 0.0), vertices, normals, uvs, material, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 68);
	objectWrapper->AddObject("Objects/Basic_house_joined.obj", model, 4, glm::vec3(17.0, -1.0, -15.0), vertices, normals, uvs, material, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 68);
	objectWrapper->AddObject("Objects/trash_can.obj", model, 5, glm::vec3(10.0, -1.0, -30.0), vertices, normals, uvs, material, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 68);
	objectWrapper->AddObject("Objects/postbus.obj", model, 6, glm::vec3(7.0, -1.0, 14.0), vertices, normals, uvs, material, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 128);
	objectWrapper->AddObject("Objects/street.obj", model, 7, glm::vec3(0.0, -0.9, -30.0), vertices, normals, uvs, material, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 128);
	objectWrapper->AddObject("Objects/Basic_house_joined.obj", model, 8, glm::vec3(17.0, -1.0, 15.0), vertices, normals, uvs, material, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 68);
}

//------------------------------------------------------------
// MoveObjects()
// Set objects to desired rotation on initialization.
//------------------------------------------------------------
void MoveObjects() {
	model[6] = glm::rotate(model[6], glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
}

//------------------------------------------------------------
// InitMaterialsLight()
//------------------------------------------------------------
void InitMaterialsLight()
{
	light.position = glm::vec3(4.0, 20.0, 4.0);
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
	camera = new CameraControls(view);
	textureController = new TextureController();
	animationController = new AnimationController();
	objectWrapper = new ObjectWrapper();
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
	InitMatrices();
	InitControllers();
	InitMaterialsLight();
	InitShaders();
	InitObjects();
	MoveObjects();
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
