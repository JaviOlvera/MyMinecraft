#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Audio.h"
#include "Physics.h"

using namespace glm;

extern vector<Audio> Audios;
extern int currentAudioId;

extern void PlayAudio(std::string filePath, bool wholePath, int vol);



class Camera
{
public:

	vec3 Orientation = vec3(0.0f, 0.0f, -1.0f);
	vec3 Up = vec3(0.0f, 1.0f, 0.0f);


	bool fly = false;

	float playerHeight = 1.7f;
	float eyeHeight = 1.62f;
	float playerRadius = 0.3f;


	vector<vec3> Collider =
	{
		//Positive scale from origin
		vec3(playerRadius, playerHeight - eyeHeight, playerRadius),

		//Negative scale from origin
		vec3(playerRadius, eyeHeight, playerRadius)
	};

	Entity entity = Entity(vec3(0,0,0), Collider);
	

	int width;
	int height;

	float Speed = 0.05f;
	float RunSpeed = 0.1f;
	float sensitivity = 100.0f;


	Camera(int width, int height, vec3 position);

	void Matrix(float FOVdeg, float nearPlane, float farPlane, unsigned int shader, const char* uniform);
	void Inputs(GLFWwindow* window, int lastMouseX, int lastMouseY, int camOffsetX, int camOffsetY);

};
