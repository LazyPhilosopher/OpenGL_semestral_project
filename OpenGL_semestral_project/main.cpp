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
#include "Light.h"
#include "Material.h"

#include "Model.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;

Material shinyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Model xwing, mountains;

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

	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTexture();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTexture();

	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);

	xwing = Model();
	xwing.LoadModel("Models/x-wing.obj");

	mountains = Model();
	mountains.LoadModel("Models/mountains.obj");

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.6f,
		0.0f, 0.0f, -1.0f);

	unsigned int pointLightCount = 0;
	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
		0.0f, 0.1f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
		0.0f, 0.1f,
		-4.0f, 2.0f, 0.0f,
		0.3f, 0.1f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, -1.5f, 0.0f,
		-100.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformAmbientIntensity = 0, uniformAmbientColour = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
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

		 
		 // Clear window
		 glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		 
		 // Use shader program
		 shaderList[0].UseShader();
		 uniformModel = shaderList[0].GetModelLocation();
		 uniformProjection = shaderList[0].GetProjectionLocation();
		 uniformView = shaderList[0].GetViewLocation();
		 uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
		 uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();

		 uniformEyePosition = shaderList[0].GetEyePositionLocation();
		 uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		 uniformShininess = shaderList[0].GetShininessLocation();

		 glm::vec3 lowerLight = camera.getCameraPosition();
		 lowerLight.y -= 0.3f;

		 shaderList[0].SetDirectionalLight(&mainLight);
		 shaderList[0].SetPointLights(pointLights, pointLightCount);
		 shaderList[0].SetSpotLights(spotLights, spotLightCount);
		 
		 // Diagonal + rotational translations
		 glm::mat4 model = glm::mat4(1.0f);
		 
		 
		 // Set uniform variable value for xMove in vertex shader
		 glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		 glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		 glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		 model = glm::mat4(1.0f);
		 model = glm::translate(model, glm::vec3(-5.0f, 2.0f, 0.0f));
		 model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
		 glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		 shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		 xwing.RenderModel();

		 model = glm::mat4(1.0f);
		 model = glm::translate(model, glm::vec3(-7.0f, -50.0f, 10.0f));
		 model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		 glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		 shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
		 mountains.RenderModel();

		 // Unuse shader program
		 glUseProgram(0);
		 
		 mainWindow.swapBuffers();
	}

	// Terminate GLFW
	glfwTerminate();

	return 0;
}
