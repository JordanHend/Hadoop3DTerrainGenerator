#ifndef COMMON_H
#define COMMON_H

#include "Shader.h"
#include <iostream>
#include <vector>
#include <string>
#include <glad\glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "GLCamera.h"


namespace shader
{
	extern Shader textShader;

}
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern glm::mat4 projection;
extern GLCamera mainCamera;
#endif;