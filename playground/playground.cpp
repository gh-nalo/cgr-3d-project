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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
	// unsigned int diffuseMap = loadTexture("brick_texture.png");
	// unsigned int specularMap = loadTexture("brick_specular.png");
	unsigned int diffuseMap = loadTexture("brick_texture.png");
	unsigned int specularMap = loadTexture("brick_specular.png");

	// Game Loop
	while (!glfwWindowShouldClose(window))
	{
		// Acquire deltaTime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		totalTimePassed += deltaTime;

		// Process user input
		processInput(window);

		// Reset - Background color
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Model (World), View (Camera), Projection matrices
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		
		updateLightingShaderInformation(lightingShader, model, view, projection);

		// Textures
		// Diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// Specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		// Render game objects
		glBindVertexArray(cubeVAO);

		// Calculate model matrix for each object
		glm::mat4 model_obj = glm::mat4(1.0f);
		model_obj = glm::translate(model_obj, cubePositions[0]);
		model_obj = glm::scale(model_obj, glm::vec3(40.0, 1.0, 40.0));
		model_obj = glm::rotate(model_obj, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));
		lightingShader.setMat4("model", model_obj);

		// Draw faces
		glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices (6 faces * 2 triangles * 3 vertices)

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
	// Activate shader before setting uniforms/drawing objects
	lightingShader.use();
	lightingShader.setVec3("light.position", camera.Position);
	lightingShader.setVec3("light.direction", camera.Front);
	lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
	lightingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
	lightingShader.setVec3("viewPos", camera.Position);

	// Light properties
	lightingShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
	lightingShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
	lightingShader.setFloat("light.constant", 1.0f);
	lightingShader.setFloat("light.linear", 0.09f);
	lightingShader.setFloat("light.quadratic", 0.032f);

	// Material properties
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	lightingShader.setFloat("material.shininess", 32.0f);

	// Timing
	lightingShader.setFloat("iTime", totalTimePassed);
	std::cout << "Time passed: " << totalTimePassed << std::endl;

	// MVP
	lightingShader.setMat4("model", model);
	lightingShader.setMat4("view", view);
	lightingShader.setMat4("projection", projection);
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
	// Space - Move up
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
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