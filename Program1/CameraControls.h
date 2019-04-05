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
	float roll = 0;
	glm::vec3 fpsPos;
	glm::vec3 overviewPos;
	glm::vec3 eyeVector;
	glm::vec2 mousePosition;
	glm::mat4 viewMatrix;
	glm::mat4 overviewMat;

	CameraControls(glm::mat4 fpsView) {
		viewMatrix = fpsView;
		eyeVector = glm::vec3();
		fpsPos = glm::vec3(0.0, 0.0, 0.0);
		overviewPos = glm::vec3(30.0, -20.0, -15.0);
		overviewMat = glm::lookAt(overviewPos,glm::vec3(),glm::vec3(0.0,1.0,0.0));
	}

	void CaptureKeys(unsigned char key, int x, int y);
	void CaptureMouse(int button, int state, int x, int y);
	void CaptureMouseMove(int x, int y);
	void UpdateView();


};