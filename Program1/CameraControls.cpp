#pragma once
#include <cmath>
#include "CameraControls.h"
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>

void CameraControls::CaptureKeys(unsigned char key, int x, int y) {
	float speed = 0.1;
	float dz = 0;
	float dx = 0;
	float yaw = 0;
	switch (key) {
	case 'w':
		dz = 2;
		break;
	case 's':
		dz = -2;
		break;
	case 'a':
		dx = -2;
		break;
	case 'd':
		dx = 2;
		break;
	case 'c':
		if (firstpersonmode) {
			firstpersonmode = false;
		}
		else {
			firstpersonmode = true;
		}
		break;
	}
	glm::mat4 mat = viewMatrix;

	glm::vec3 forward(mat[0][2], mat[1][2], mat[2][2]);
	glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

	eyeVector += (-dz * forward + dx * strafe)*speed;

	UpdateView();
}

void CameraControls::UpdateView() {
	glm::mat4 matPitch = glm::mat4(1.0f);
	glm::mat4 matYaw = glm::mat4(1.0f);
	glm::mat4 matRoll = glm::mat4(1.0f);

	matPitch = glm::rotate(matPitch, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	matYaw = glm::rotate(matYaw, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	matRoll = glm::rotate(matRoll, roll, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 rotate = matRoll * matPitch * matYaw;

	glm::mat4 translate = glm::mat4(1.0f);
	//if firstpersonmode, look up and go forward on same y axis
	if (firstpersonmode) {
		eyeVector.y = 1;
	}
	translate = glm::translate(translate, -eyeVector);
	viewMatrix = rotate * translate;
};

void CameraControls::CaptureMouse(int button, int state, int x, int y) {
	if (state == GLUT_UP) {
		isMousePressed = false;
	}
	if (state == GLUT_DOWN) {
		isMousePressed = true;
		mousePosition.x = x;
		mousePosition.y = y;
	}
}

void CameraControls::CaptureMouseMove(int x, int y) {
	if (isMousePressed == false) {
		return;
	}
	glm::vec2 mouse_delta = glm::vec2(x, y) - mousePosition;

	yaw += mouseX_Sensitivity * mouse_delta.x;
	pitch += mouseY_Sensitivity * mouse_delta.y;

	mousePosition = glm::vec2(x, y);
	UpdateView();
}