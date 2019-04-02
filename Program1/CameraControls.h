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
	glm::vec2 mousePosition;
	glm::vec3 eyeVector;
	glm::mat4 viewMatrix;

	CameraControls() {
		viewMatrix = glm::mat4();
		eyeVector = glm::vec3();
	}

	void CaptureKeys(unsigned char key, int x, int y);
	void CaptureMouse(int button, int state, int x, int y);
	void CaptureMouseMove(int x, int y);
	void UpdateView();


};