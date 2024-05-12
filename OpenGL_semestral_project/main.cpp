#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex Shader code
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

// Function to create a triangle
void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	// Vertex data
	GLfloat vertices[] = {
		//	x      y      z			u	  v
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		0.0f, -1.0f, 1.0f,		0.5f, 0.0f,
		1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.5f, 1.0f
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 20, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 20, 12);
	meshList.push_back(obj2);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// Main function
int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTexture();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTexture();

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);


	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = (GLfloat) glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		 // Get + Handle user input events
		 glfwPollEvents();
		 
		 camera.keyControl(mainWindow.getKeys(), deltaTime);
		 camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		 
		 // Update triangle offset for animation
		 if (direction)
		 {
		 	triOffset += triIncrement;
		 }
		 else {
		 	triOffset -= triIncrement;
		 }
		 
		 if (abs(triOffset) >= triMaxOffset)
		 {
		 	direction = !direction;
		 }
		 
		 curAngle += 0.05f;
		 if (curAngle >= 360)
		 {
		 	curAngle = 0.0f;
		 }
		 
		 if (curSize >= maxSize || curSize <= minSize)
		 {
		 	sizeDirection = !sizeDirection;
		 }
		 if (sizeDirection)
		 {
		 	curSize += 0.001f;
		 }
		 else
		 {
		 	curSize -= 0.001f;
		 }
		 
		 // Clear window
		 glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		 
		 // Use shader program
		 shaderList[0].UseShader();
		 uniformModel = shaderList[0].GetModelLocation();
		 uniformProjection = shaderList[0].GetProjectionLocation();
		 uniformView = shaderList[0].GetViewLocation();
		 
		 // Diagonal + rotational translations
		 glm::mat4 model = glm::mat4(1.0f);
		 //model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		 model = glm::translate(model, glm::vec3(0.0f, -0.0f, -2.5f));
		 model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		 model = glm::translate(model, glm::vec3(-triOffset, -triOffset, 0.0f));
		 model = glm::rotate(model, -curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		 
		 
		 // Set uniform variable value for xMove in vertex shader
		 glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		 glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		 glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		 brickTexture.UseTexture();
		 meshList[0]->RenderMesh();
		 
		 
		 model = glm::mat4(1.0f);
		 //model = glm::rotate(model, -curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		 model = glm::translate(model, glm::vec3(0.0f, 1.0f, -10.0f));
		 model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		 model = glm::translate(model, glm::vec3(triOffset, triOffset, 0.0f));
		 model = glm::rotate(model, -curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		 glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		 //glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		 dirtTexture.UseTexture();
		 meshList[1]->RenderMesh();
		 
		 // Unuse shader program
		 glUseProgram(0);
		 
		 mainWindow.swapBuffers();
	}

	// Terminate GLFW
	glfwTerminate();

	return 0;
}
