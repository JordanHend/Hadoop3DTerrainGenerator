#ifndef SHADOW_HANDLER_H
#define SHADOW_HANDLER_H
#include <algorithm>    // std::min
#include "Shader.h"
#include <glad\glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Terrain.h"


struct PointLight
{

	glm::vec3 position;
	glm::vec3 ambient = { 1.0f, 1.0f, 1.0f };
	glm::vec3 diffuse = { 0.0f, 0.0f, 0.0f };
	glm::vec3 specular = { 0.0f, 0.0f, 0.0f };

	float constant = 1;
	float linear = 0;
	float quadratic = 0;

};

class ShadowHandler
{
public:
	ShadowHandler();

	
	Shader terraihDepthShader;

	void confShader(Terrain * terrain, PointLight light);
	unsigned int depthCubemap;
	float near_plane = 1.0f;
	float far_plane = 5000.0f;
	~ShadowHandler();
private :
	glm::mat4 shadowProj;



	
	unsigned int depthMapFBO;


};


float distance2D(glm::vec2 point, glm::vec2 point2);

float distance2D(glm::vec3 point, glm::vec3 point2);

float distance3D(glm::vec3 point, glm::vec3 point2);


#endif