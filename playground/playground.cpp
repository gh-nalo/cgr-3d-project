#include "playground.h"

/// <summary>
///		Setup and Initializations
/// </summary>
/// <returns></returns>
int main()
{
	// Initialize and configure glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the glfw window
	window = initializeWindow();

	// Init. OpenGL function pointers
	initializeFunctionPointers();

	// Enable Depth Testing
	glEnable(GL_DEPTH_TEST);

	// Configure cubeVBO
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// Configure cubeVAO
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	// Params :: index, nComponents, Type, Normalize?, Offset to next, Offset to first
	// Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	generateCubes();

	// Start game loop
	update();

	// De-allocate resources
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	// Terminate
	glfwTerminate();
	return 0;
}

/// <summary>
///		Animation Loop
/// </summary>
void update() {

	// Build and compile shaders
	Shader lightingShader = Shader("light.vs", "light.fs");

	// Load Textures
	unsigned int ground_texture = loadTexture("wood_texture.jpg");
	unsigned int ground_specular = loadTexture("wood_specular.png");

	unsigned int brick_texture = loadTexture("brick_texture.png");
	unsigned int brick_specular = loadTexture("brick_specular.png");

	int second = -1;

	float soundTimer = 0.0f;
	bool soundPlayed = false;

	// Game Loop
	while (!glfwWindowShouldClose(window))
	{
		// Acquire deltaTime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		totalTimePassed += deltaTime;
		soundTimer += deltaTime;
		flashLightTimer -= deltaTime;

		// Print time passed in console every second
		if ((int)totalTimePassed != second) {
			std::cout << "Time passed: " << floor(totalTimePassed) << std::endl;
			second++;
		}
		int second = (int)totalTimePassed;

		// Prevent sound when flashlight is turned off
		if (!flashLightOn || cheatMode) {
			soundPlayed = false;
			soundTimer = 0.0f;
		}
		// Start "suspense" music
		else if (!soundPlayed && soundTimer > 30.0f) {
			try {
				PlaySound(TEXT("8seconds.wav"), NULL, SND_FILENAME | SND_ASYNC);
				soundPlayed = true;
			}
			catch (const std::exception&) {
				std::cout << "Could not play sound file." << std::endl;
			}
		}
		// End game
		else if (soundPlayed && soundTimer > 39.0f) {
			cheatMode = true;
		}

		// Process user input
		processInput(window);

		// Reset - Background color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Model (World), View (Camera), Projection matrices
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);

		updateLightingShaderInformation(lightingShader, model, view, projection);

		// Render game objects
		glBindVertexArray(cubeVAO);

		// Calculate model matrix for each object
		for (int i = 0; i < cubePositions.size(); i++) {
			if (cubePositions[i].y == 0.0) {
				// Textures
				// Diffuse map
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, ground_texture);
				// Specular map
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, ground_specular);
			}
			else {
				// Textures
				// Diffuse map
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, brick_texture);
				// Specular map
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, brick_specular);
			}

			glm::mat4 model_obj = glm::mat4(1.0f);
			model_obj = glm::translate(model_obj, cubePositions[i]);
			model_obj = glm::scale(model_obj, glm::vec3(1.0, 1.0, 1.0));
			model_obj = glm::rotate(model_obj, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model_obj);

			// Draw faces
			glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices (6 faces * 2 triangles * 3 vertices)
		}

		// Swap buffers, poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

}

/// <summary>
///		Update attributes of lighting shader
/// </summary>
/// <param name="lightingShader">Lighting Shader to be updated</param>
void updateLightingShaderInformation(Shader& lightingShader, glm::mat4& model, glm::mat4& view, glm::mat4& projection) {
	
	// Activate shader
	lightingShader.use();

	// Light properties
	lightingShader.setVec3("light.position", camera.Position);
	lightingShader.setVec3("light.direction", camera.Front);

	if (flashLightOn) {
		lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(15.0f)));
		lightingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
	}
	else {
		lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(0.0f)));
		lightingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(0.0f)));
	}

	lightingShader.setVec3("viewPos", camera.Position);

	// Light and material properties
	// Ambient Light
	//lightingShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f); // in case stream is too dark
	lightingShader.setVec3("light.ambient", 0.15f, 0.15f, 0.15f);

	// Diffuse Light
	lightingShader.setVec3("light.diffuse", 0.01f, 0.01f, 0.01f);
	lightingShader.setInt("material.diffuse", 0);

	// Specular Light
	lightingShader.setVec3("light.specular", 0.05f, 0.05f, 0.05f);
	lightingShader.setInt("material.specular", 1);

	// Cheat mode
	lightingShader.setBool("cheatMode", cheatMode);

	// Timing
	lightingShader.setFloat("iTime", totalTimePassed);

	// MVP
	lightingShader.setMat4("model", model);
	lightingShader.setMat4("view", view);
	lightingShader.setMat4("projection", projection);

	// Settings
	lightingShader.setFloat("light.constant", 1.0f);
	lightingShader.setFloat("light.linear", 0.09f);
	lightingShader.setFloat("light.quadratic", 0.032f);

	// Material properties
	lightingShader.setFloat("material.shininess", 16.0f);
}

/// <summary>
///		Process user input
/// </summary>
/// <param name="window">Window</param>
void processInput(GLFWwindow* window)
{
	// ESC - Closing the window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Movement
	// W - Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	// S - Move backwards
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	// A - Move left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	// D - Move right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

	// C - Toggle flashlight
	if (flashLightTimer < 0.0 && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		flashLightOn = !flashLightOn;
		flashLightTimer = 0.5f;

		PlaySound(NULL, NULL, SND_ASYNC);
	}

	// Cheat/Debugging mode
	// X - Activate cheat mode
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		cheatMode = !cheatMode;
	// Space - Move up
	if (cheatMode && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
	// Ctrl - Move down
	if (cheatMode && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
}


/// <summary>
///		Executed when window size is changed
/// </summary>
/// <param name="window">Window</param>
/// <param name="width">Viewport width</param>
/// <param name="height">Viewport height</param>
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Adjust window scaling
	glViewport(0, 0, width, height);
}


/// <summary>
///		Executed when mouse is moved
/// </summary>
/// <param name="window">Window</param>
/// <param name="xpos">Mouse x position</param>
/// <param name="ypos">Mouse y position</param>
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// Initial mouse movement
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// Adjust offset
	float xoffset = xpos - lastX;
	// Reversed since y-coordinates go from bottom to top
	float yoffset = lastY - ypos;

	// Save last location
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


/// <summary>
///		Executed when mouse wheel is used
/// </summary>
/// <param name="window">Window</param>
/// <param name="xoffset">x offset</param>
/// <param name="yoffset">y offset</param>
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


/// <summary>
///		Utility function, load 2D textures from a file
/// </summary>
/// <param name="path">Path to the texture</param>
/// <returns>Texture ID</returns>
unsigned int loadTexture(char const* path)
{
	// Get a texture ID
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

	// Load textures
	if (data)
	{
		GLenum format = GL_RGBA;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

/// <summary>
///		Initialize GLFW window
/// </summary>
/// <returns></returns>
GLFWwindow* initializeWindow() {
	GLFWwindow* w = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

	if (w == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		std::exit(-1);
	}

	glfwMakeContextCurrent(w);
	glfwSetFramebufferSizeCallback(w, framebuffer_size_callback);
	glfwSetCursorPosCallback(w, mouse_callback);
	glfwSetScrollCallback(w, scroll_callback);

	// Capture mouse events
	glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return w;
}

/// <summary>
///		Load OpenGL function pointers (GLAD)
/// </summary>
void initializeFunctionPointers() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		std::exit(-1);
	}
}

/// <summary>
///		Generate map cubes
/// </summary>
void generateCubes() {
	size_t width = 50;
	float xCoord = (-1.0) * (float)width / 2.0;
	float zCoord = xCoord;

	for (size_t y = 0; y < width; y++) {
		for (size_t x = 0; x < width; x++) {
			cubePositions.push_back(glm::vec3(xCoord, 0.0, zCoord));

			if (world_1[x + y * width]) {
				cubePositions.push_back(glm::vec3(xCoord, 1.0, zCoord));
				cubePositions.push_back(glm::vec3(xCoord, 2.0, zCoord));
			}
			xCoord += 1.0f;
		}
		zCoord += 1.0f;
		xCoord = (-1.0) * (float)width / 2.0;
	}
}