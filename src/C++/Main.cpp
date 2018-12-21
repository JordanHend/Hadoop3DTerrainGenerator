#include <glad\glad.h>
#include <gl\GL.h>
#include <GLFW\glfw3.h>
#include <glad\glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "ShadowHandler.h"
#include "Terrain.h"
#include "Common.h"
#include "GLCamera.h"
#include "Timer.h"
#include "ConfigReader.h"
#include "Font.h"
//Initializes openGL
int initOpenGL();

GLFWwindow * window;


//Light movement info
float angle = PI/2;
float radius = 400;

//Deltatime
double TimeBetweenFrames = 0;
float lastFrame = 0;
Timer timer;
float FPS;
float deltaTime;

//For Mouse handling.
float lastX, lastY;
bool firstMouse = true;


//Function to set a point light into a shader. Pulled from one of my other projects.
void setPointLight(PointLight* obj, Shader shader, int index);
//Process Keyboard input.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
//Process mouse input.
void mouseCallback(GLFWwindow * window, double xposition, double yposition);
//Handle window size change event. 
void framebuffer_size_callback(GLFWwindow * window, int width, int height);
//Handle scroll callback.
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//To Update Camera with movement
void updateCamera(GLFWwindow * window);

int initOpenGL()
{
	//Init glfw
	glfwInit();



	//Set version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWmonitor * monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);



	//Creating window
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Average Terrain Renderer", NULL, NULL);


	/* For Borderless Fullscreen windowed mode. 


	window = glfwCreateWindow(mode->width, mode->height, "OpenGL", NULL, NULL);
	SCREEN_WIDTH = mode->width;
	SCREEN_HEIGHT = mode->height;
	*/


	if (window == NULL)
	{
		std::cout << "Failed to create window";
		return -1;
	}

	//Set context to current window
	glfwMakeContextCurrent(window);
	//Set event callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scroll_callback);
	//initialize GLAD function pointers.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	return 1;

}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}

void mouseCallback(GLFWwindow * window, double xposition, double yposition)
{
	if (firstMouse)
	{
		lastX = xposition;
		lastY = yposition;
		firstMouse = false;
	}

	float xoffset = xposition - lastX;
	float yoffset = lastY - yposition; // reversed since y-coordinates go from bottom to top

	lastX = xposition;
	lastY = yposition;


		mainCamera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void updateCamera(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		float velocity = mainCamera.MovementSpeed * deltaTime;
		mainCamera.Position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		float velocity = mainCamera.MovementSpeed * deltaTime;
		mainCamera.Position += glm::vec3(0.0f, -1.0f, 0.0f) * velocity;
	}
}



void setPointLight(PointLight* obj, Shader shader, int index)
{
	shader.setVec3("viewPos", mainCamera.Position);
	std::string light = "pointLights[" + std::to_string(index) + "]";
	shader.setVec3(light + ".position", obj->position);
	shader.setVec3(light + ".ambient", obj->ambient);
	shader.setVec3(light + ".diffuse", obj->diffuse);
	shader.setVec3(light + ".specular", obj->specular);
	shader.setFloat(light + ".constant", obj->constant);
	shader.setFloat(light + ".linear", obj->linear);
	shader.setFloat(light + ".quadratic", obj->quadratic);

}



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	mainCamera.MovementSpeed += yoffset * 10;
}


//Just the vertices for the rube to draw.
const float cubeVertices[] = {
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
};



int main()
{
	//Set up OpenGL
	initOpenGL();


	//Initialize the things to draw the light source.
	Shader shapeShader;
	shapeShader.init("PrimitiveVertex.shdr", "PrimitiveFragment.shdr");

	//VBO and VAO for the light source
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	//init text shader
	shader::textShader.init("textVertex.shdr", "textFragment.shdr");


	//OpenGL Settings
	glfwSwapInterval(0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_BLEND);

	glEnable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	//Setting up terrain
	Terrain terrain;

	float hscale, bscale, r = 1, g = 1, b = 1;

	//Read config file for params
	ConfigReader read;
	read.read(hscale, bscale, radius, r, g, b);
	
	//Load terrain textures
	terrain.loadHeightMap("image.jpeg", 0, hscale, bscale);
	terrain.setBlendTexture("Assets/Blend.png");
	terrain.loadTexture("Assets/Base.jpg");
	std::vector<std::string> textures = { "Assets/r.jpg", "Assets/g.jpg", "Assets/b.jpg" };
	terrain.setRGBTextures(textures);


	//set up light
	PointLight light;
	light.position = glm::vec3(0, 400, 0);
	light.ambient = glm::vec3(r, g, b);
	ShadowHandler shadow;

	//Font stuff for instructions
	Font font("Amiko-Regular.ttf");
	
	std::string fps;
	std::string cameraSpeed;
	std::string controls1 = "WASD to move around. Space and Control to go up/down. Scroll wheel to go faster/slower.";
	std::string controls2 = "Q and E moves the light around.";
	std::string controls3 = "ESC to exit.";


	//Setting up camera positions at the start
	mainCamera.Position.y = terrain.heightScale + 200;
	mainCamera.Position.x = -200;

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


	//Main loop
	while (!glfwWindowShouldClose(window))
	{

		//Update camera
		updateCamera(window);
		float camypos = terrain.getHeightAt(mainCamera.Position);
		if (mainCamera.Position.y <= camypos)
			mainCamera.Position.y = camypos + 1;


		//Get FPS
		timer.start();
		float currentFrame = glfwGetTime();
		TimeBetweenFrames = (currentFrame - lastFrame);
		deltaTime = TimeBetweenFrames;
		lastFrame = currentFrame;
		FPS = (1 / TimeBetweenFrames);



		//Update light pos (and read user input for rotating it)
		light.position.x = cos(angle)*radius;
		light.position.y = sin(angle)*radius;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			angle-= .4 * deltaTime;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			angle += .4 * deltaTime;



		//Do Shadows
		shadow.confShader(&terrain, light);


		//Render scene
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//Change viewport back after doing shadows
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		//Set up and Render text
		fps = "FPS: " + std::to_string(FPS);
		cameraSpeed = "Speed: " + std::to_string(mainCamera.MovementSpeed);
	
		font.RenderText(fps, 20, SCREEN_HEIGHT - 20, 0.4, glm::vec3(0.7, 0.7, 0.2));
		font.RenderText(cameraSpeed, 20, SCREEN_HEIGHT - 60, 0.4, glm::vec3(0.7, 0.7, 0.2));
		font.RenderText(controls1, 0, 10, 0.4, glm::vec3(0.7, 0.7, 0.2));
		font.RenderText(controls2, 0, 40, 0.4, glm::vec3(0.7, 0.7, 0.2));
		font.RenderText(controls3, 0, 70, 0.4, glm::vec3(0.7, 0.7, 0.2));

		//Render the light cube.
		glm::mat4 model;

		model = glm::translate(light.position);
		model = glm::scale(model, glm::vec3(50));
		shapeShader.use();
		shapeShader.setMat4("model", model);
		shapeShader.setMat4("projection", projection);
		shapeShader.setMat4("view", mainCamera.GetViewMatrix());
		shapeShader.setBool("selected", true);


		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


		//Use Terrain shader and set light info on there.
		terrain.heightMapShader.use();
		terrain.heightMapShader.setInt("numPointLights", 1);
		setPointLight(&light, terrain.heightMapShader, 0);
		terrain.heightMapShader.setMat4("projection", projection);
		terrain.heightMapShader.setVec3("viewPos", mainCamera.Position);
		terrain.heightMapShader.setBool("shadows", true);
		terrain.heightMapShader.setFloat("far_plane", shadow.far_plane);
		glActiveTexture(GL_TEXTURE5);
		terrain.heightMapShader.setInt("depthMap", 5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadow.depthCubemap);
		glActiveTexture(GL_TEXTURE0);

		terrain.Render();


		//Check for errors in case there are any.
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "GL ERROR! " << err << std::endl;
		}


		//And finish!
		glfwSwapBuffers(window);
		glfwPollEvents();

	}





	glfwTerminate();

	return 0;


}