#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <shader.h>
#include <cube.h>
#include <arcball.h>
#include <cstdlib>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Function Prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double x, double y);
void render();
unsigned int loadTexture(const char *);

// Global Variables
GLFWwindow *window = NULL;
Shader *globalShader = NULL;
Shader *lampShader = NULL;
static unsigned int diffuseMap, specularMap;
float BACKGRAOUND_COLOR[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
unsigned int SCR_WIDTH = 1600;
unsigned int SCR_HEIGHT = 800;
Cube *cube, *lamp;

// glm variables
glm::mat4 projection, view, model;
glm::vec3 camPosition(0.0f, 0.0f, 9.0f);
glm::vec3 camTarget(0.0f, 0.0f, 0.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);

// for lighting
glm::vec3 lightSize(0.1f, 0.1f, 0.1f);
glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

// for arcball
float arcballSpeed = 0.2f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
bool arcballCamRot = true;

int main()
{
	window = glAllInit();

	// shader loading and compile (by calling the constructor)
	globalShader = new Shader("global_08.vs", "global_08.fs");
	lampShader = new Shader("lamp.vs", "lamp.fs");

	// projection and view matrix and lightening
	globalShader->use();
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = glm::lookAt(camPosition, camTarget, camUp);

	globalShader->setMat4("projection", projection);
	globalShader->setMat4("view", view);
	globalShader->setVec3("viewPos", camPosition);
	// material struct
	globalShader->setInt("material.diffuse", 0);
	globalShader->setInt("material.specular", 1);
	diffuseMap = loadTexture("container2.bmp");
	specularMap = loadTexture("container2_specular.bmp");
	globalShader->setFloat("material.shininess", 32.0f);
	// directional light struct
	globalShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	globalShader->setVec3("dirLight.ambient", lightColor * glm::vec3(0.05f));
	globalShader->setVec3("dirLight.diffuse", lightColor * glm::vec3(0.4f));
	globalShader->setVec3("dirLight.specular", lightColor * glm::vec3(0.5f));
	// spotlight struct
	glm::vec3 spotDir = glm::vec3(0.0f, 0.0f, 0.0f) - lightPos;
	globalShader->setVec3("spotLight.position", lightPos);
	globalShader->setVec3("spotLight.direction", spotDir);
	globalShader->setVec3("spotLight.ambient", lightColor * glm::vec3(0.1f));
	globalShader->setVec3("spotLight.diffuse", lightColor * glm::vec3(1.0f));
	globalShader->setVec3("spotLight.specular", lightColor * glm::vec3(1.0f));
	globalShader->setFloat("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
	globalShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
	globalShader->setFloat("spotLight.constant", 1.0f);
	globalShader->setFloat("spotLight.linear", 0.09f);
	globalShader->setFloat("spotLight.quadratic", 0.032f);

	// lamp shader
	lampShader->use();
	lampShader->setMat4("projection", projection);
	lampShader->setMat4("view", view);

	cube = new Cube();
	lamp = new Cube();

	while (!glfwWindowShouldClose(window)) {
		render();
		glfwPollEvents();
	}
}

GLFWwindow *glAllInit()
{
	GLFWwindow *window;

	// glfw: initialize and configure
	if (!glfwInit()) {
		printf("GLFW initialisation failed!");
		glfwTerminate();
		exit(-1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// glfw window creation
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Practice", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// Allow modern extension features
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW initialisation failed!" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(-1);
	}

	// OpenGL states
	glClearColor(BACKGRAOUND_COLOR[0], BACKGRAOUND_COLOR[1], BACKGRAOUND_COLOR[2], BACKGRAOUND_COLOR[3]);
	glEnable(GL_DEPTH_TEST);

	return window;
}


unsigned int loadTexture(const char* texture_file) {

	unsigned int texture;

	// Create texture ids.
	glGenTextures(1, &texture);

	// All upcomming GL_TEXTURE_2D operations now on "texture" object
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texture parameters for wrapping.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char *image = stbi_load(texture_file, &width, &height, &nrChannels, 0);
	if (!image) {
		printf("texture %s loading error ... \n", texture_file);
	}
	else printf("texture %s loaded\n", texture_file);

	GLenum format;
	if (nrChannels == 1) format = GL_RED;
	else if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// arcball view
	view = glm::lookAt(camPosition, camTarget, camUp);
	view = view * camArcBall.createRotationMatrix();

	// lamp
	lampShader->use();
	lampShader->setMat4("view", view);
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, lightSize);
	lampShader->setMat4("model", model);
	lamp->draw(lampShader);

	// cubes
	globalShader->use();
	globalShader->setMat4("view", view);
	// texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	// cube1
	model = glm::mat4(1.0f);
	model = model * modelArcBall.createRotationMatrix();
	globalShader->setMat4("model", model);
	cube->draw(globalShader);

	// cube2
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
	globalShader->setMat4("model", model);
	cube->draw(globalShader);

	// cube3
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 2.0f, 1.0f));
	globalShader->setMat4("model", model);
	cube->draw(globalShader);
	
	

	glfwSwapBuffers(window);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, true);
		}
		else if (key == GLFW_KEY_R) {
			camArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
			modelArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
		}
		else if (key == GLFW_KEY_A) {
			arcballCamRot = !arcballCamRot;
			if (arcballCamRot) {
				std::cout << "ARCBALL: Camera rotation mode" << std::endl;
			}
			else {
				std::cout << "ARCBALL: Model  rotation mode" << std::endl;
			}
		}
	}
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
	if (arcballCamRot)
		camArcBall.mouseButtonCallback(window, button, action, mods);
	else
		modelArcBall.mouseButtonCallback(window, button, action, mods);
}

void cursor_position_callback(GLFWwindow *window, double x, double y) {
	if (arcballCamRot)
		camArcBall.cursorCallback(window, x, y);
	else
		modelArcBall.cursorCallback(window, x, y);
}
