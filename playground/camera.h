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
const float YAW = -90.0f, PITCH = 0.0f, SPEED = 2.5f, SENSITIVITY = 0.1f, ZOOM = 60.0f;

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
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

	/// <summary>
	///		Calculate lookAt matrix
	/// </summary>
	/// <returns>Updated lookAt matrix</returns>
	glm::mat4 GetViewMatrix();

	/// <summary>
	///		Process keyboard input
	/// </summary>
	/// <param name="direction">Type of movement</param>
	/// <param name="deltaTime">Passed time since last frame</param>
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);

	/// <summary>
	///		Process mouse input
	/// </summary>
	/// <param name="xoffset">x offset</param>
	/// <param name="yoffset">y offset</param>
	/// <param name="constrainPitch">predefined</param>
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

	/// <summary>
	///		Process scrolling input
	/// </summary>
	/// <param name="yoffset">Scrolling offset</param>
	void ProcessMouseScroll(float yoffset);

private:

	/// <summary>
	///		Helper function to update euler angles of the model
	/// </summary>
	void updateCameraVectors();
};
#endif