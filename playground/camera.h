#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

/// <summary>
///     Options for camera movement
/// </summary>
enum class Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

/// <summary>
///     Initial camera values
/// </summary>
const float YAW = -90.0f, PITCH = 0.0f, SPEED = 2.5f, SENSITIVITY = 0.1f, ZOOM = 45.0f;

/// <summary>
///		Processes user input
/// </summary>
class Camera
{
public:

	/// <summary>
	///		Camera movement attributes
	/// </summary>
	glm::vec3 Position, Front, Up, Right, WorldUp;

	/// <summary>
	///		Euler angles
	/// </summary>
	float Yaw, Pitch;

	/// <summary>
	///  Camera settings
	/// </summary>
	float MovementSpeed, MouseSensitivity, Zoom;

	/// <summary>
	///		Constructor
	/// </summary>
	/// <param name="position">Initial camera position</param>
	/// <param name="up">Up-vector</param>
	/// <param name="yaw">Initial yaw</param>
	/// <param name="pitch">Initial pitch</param>
	/// <returns>Obj</returns>
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	/// <summary>
	///		Calculate lookAt matrix
	/// </summary>
	/// <returns>Updated lookAt matrix</returns>
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	/// <summary>
	///		Process keyboard input
	/// </summary>
	/// <param name="direction">Type of movement</param>
	/// <param name="deltaTime">Passed time since last frame</param>
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == Camera_Movement::FORWARD)
			Position += glm::vec3(Front.x, 0.0, Front.z) * velocity;
		if (direction == Camera_Movement::BACKWARD)
			Position -= glm::vec3(Front.x, 0.0, Front.z) * velocity;
		if (direction == Camera_Movement::LEFT)
			Position -= glm::vec3(Right.x, 0.0, Right.z) * velocity;
		if (direction == Camera_Movement::RIGHT)
			Position += glm::vec3(Right.x, 0.0, Right.z) * velocity;
		if (direction == Camera_Movement::UP)
			Position += glm::vec3(0.0, 1.0, 0.0) * velocity;
		if (direction == Camera_Movement::DOWN)
			Position -= glm::vec3(0.0, 1.0, 0.0) * velocity;
	}

	/// <summary>
	///		Process mouse input
	/// </summary>
	/// <param name="xoffset">x offset</param>
	/// <param name="yoffset">y offset</param>
	/// <param name="constrainPitch">predefined</param>
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	/// <summary>
	///		Process scrolling input
	/// </summary>
	/// <param name="yoffset">Scrolling offset</param>
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:

	/// <summary>
	///		Helper function to update euler angles of the model
	/// </summary>
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		// Re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif