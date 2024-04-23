#include "Camera.h"
#include "wtypes.h"

Camera::Camera(int width, int height, vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Orientation = vec3(0.0f, 0.0f, -1.0f);

	if (fly)
	{
		playerHeight = 0.6f;
		eyeHeight = 0.3f;
		playerRadius = 0.3f;
	}

	Collider =
	{
		//Positive scale from origin
		vec3(playerRadius, playerHeight - eyeHeight, playerRadius),

		//Negative scale from origin
		vec3(playerRadius, eyeHeight, playerRadius)
	};

	entity = Entity(position, Collider);
}

extern float deltaTime;

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, unsigned int shader, const char* uniform)
{
	mat4 view = mat4(1.0f);
	mat4 projection = mat4(1.0f);

	view = lookAt(entity.position, entity.position + Orientation, Up);
	projection = perspective(radians(FOVdeg), (float)(width / height), nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shader, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));

}

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void Camera::Inputs(GLFWwindow* window, int lastMouseX, int lastMouseY, int camOffsetX, int camOffsetY)
{
	float speed = Speed;
	float oldY = entity.position.y;

	entity.walking = false;

	if (glfwGetKey(window, GLFW_KEY_X) != GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			speed = RunSpeed;
			entity.running = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			speed = Speed;
			entity.running = false;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			if (!fly)
			{
				entity.position += speed * normalize(vec3(Orientation.x, 0, Orientation.z));
				entity.walking = true;
			}
			else
			{
				entity.position += speed * Orientation;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			entity.position += speed * -normalize(cross(Orientation, Up));
			entity.walking = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			if (!fly)
			{
				entity.position += speed * -normalize(vec3(Orientation.x, 0, Orientation.z));
				entity.walking = true;
			}
			else
			{
				entity.position += speed * -Orientation;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			entity.position += speed * normalize(cross(Orientation, Up));
			entity.walking = true;
		}

		if (!fly)
		{
			entity.position.y = oldY;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			if (fly)
			{
				entity.position += speed * Up;
			}
			else if (!entity.falling)
			{
				entity.ySpeed = entity.jumpImpulse;
				entity.falling = true;
			}			
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			if (fly)
			{
				entity.position += speed * -Up;
			}
			else
			{
				//Crounch
			}
		}

		//CAMERA ROTATION https://learnopengl.com/Getting-started/Camera

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		double lastX = lastMouseX;
		double lastY = lastMouseY;

		double yaw = Orientation.x;
		double pitch = Orientation.y;

		//float xoffset = xpos - lastX;
		//float yoffset = lastY - ypos;
		double xoffset = camOffsetX;
		double yoffset = camOffsetY;


		//Detectar si el ratón ha llegado a un extremo
		int screenWidth, screenHeight;
		GetDesktopResolution(screenWidth, screenHeight);

		POINT cursorPos;
		GetCursorPos(&cursorPos);

		// Verificar si el cursor llega a los bordes de la pantalla
		if (cursorPos.x <= 0)
		{
			SetCursorPos(screenWidth - 2, cursorPos.y);
		}
		else if (cursorPos.x >= screenWidth - 1) {

			SetCursorPos(1, cursorPos.y);
		}

		if (cursorPos.y <= 0)
		{
			SetCursorPos(cursorPos.x, screenHeight - 2);
		}
		else if (cursorPos.y >= screenHeight - 1)
		{
			SetCursorPos(cursorPos.x, 1);
		}

		//std::cout << xoffset << std::endl;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw -= xoffset;
		pitch -= yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		Orientation = -glm::normalize(direction);

		/*static double lastMouseX = 0.0;
		static double lastMouseY = 0.0;

		// Capturar la posición actual del cursor
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			// Calcular la diferencia entre la posición actual y la anterior del cursor
			float deltaX = static_cast<float>(mouseX - lastMouseX);
			float deltaY = static_cast<float>(mouseY - lastMouseY);

			// Ajustar la sensibilidad del movimiento del ratón
			float sensitivity = 0.1f;

			// Calcular la nueva orientación de la cámara sin usar glm::rotate
			float yaw = deltaX * sensitivity;
			float pitch = deltaY * sensitivity;

			// Aplicar la rotación horizontal (yaw) alrededor del eje Up
			glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), Up);
			Orientation = glm::vec3(yawRotation * glm::vec4(Orientation, 0.0f));

			// Calcular el eje lateral de la cámara
			glm::vec3 lateralAxis = glm::cross(Orientation, Up);

			// Aplicar la rotación vertical (pitch) alrededor del eje lateral
			glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), lateralAxis);
			Orientation = glm::vec3(pitchRotation * glm::vec4(Orientation, 0.0f));

			// Actualizar las últimas posiciones del cursor
			lastMouseX = mouseX;
			lastMouseY = mouseY;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			lastMouseX = mouseX;
			lastMouseY = mouseY;
		}*/
	}
}
