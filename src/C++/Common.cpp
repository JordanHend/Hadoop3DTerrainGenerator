#include "Common.h"



namespace shader
{
	Shader textShader;

}
int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 720;
glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 10000.0f);
GLCamera mainCamera(glm::vec3(0.0f, 200.0f, 3.0f));