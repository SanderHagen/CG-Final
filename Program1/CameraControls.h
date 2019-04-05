#pragma once
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
class CameraControls {
public:
	const float mouseX_Sensitivity = 0.005f;
	const float mouseY_Sensitivity = 0.005f;
	
	bool firstpersonmode = true;
	bool isMousePressed = false;
	float pitch = 0;
	float yaw = 0;	
	//define overview camera lookat
	float pitchOV = glm::radians(12.0);
	float yawOV = glm::radians(65.0);
	float roll = 0;
	glm::vec3 fpsPos;
	glm::vec3 overviewPos;
	glm::vec3 eyeVector;
	glm::vec3 eyeVectorOV;
	glm::vec2 mousePosition;
	glm::mat4 viewMatrix;
	glm::mat4 overviewMat;

	CameraControls(glm::mat4 fpsView) {
		viewMatrix = fpsView;
		eyeVector = glm::vec3();
		eyeVectorOV = glm::vec3();
		fpsPos = glm::vec3(0.0, 0.0, 0.0);
		overviewPos = glm::vec3(30.0, -20.0, -15.0);
		overviewMat = glm::mat4();
	}

	void CaptureKeys(unsigned char key, int x, int y);
	void CaptureMouse(int button, int state, int x, int y);
	void CaptureMouseMove(int x, int y);
	void UpdateView();


};