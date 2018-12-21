#include "ShadowHandler.h"



ShadowHandler::ShadowHandler()
{
	//depthShader.init("depthVertex.shdr", "depthGeometry.shdr", "depthFragment.shdr");
	terraihDepthShader.init("DepthTerrainVertex.shdr", "depthGeometry.shdr", "depthFragment.shdr");
	//instancedDepthShader.init("ShadowInstancedShader.vsh", "depthGeometry.shdr", "depthFragment.shdr");



	const unsigned int SHADOW_WIDTH = 4024, SHADOW_HEIGHT = 4024;

	float aspect = ((float)SHADOW_WIDTH / (float)SHADOW_HEIGHT);

	shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plane , far_plane);

	terraihDepthShader.use();
	terraihDepthShader.setMat4("projection", shadowProj);
	terraihDepthShader.setFloat("far_plane", far_plane);



	glGenFramebuffers(1, &depthMapFBO);
	// create depth cubemap texture
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Frame buffer OFFLINE" << std::endl;
	}

	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


float distance2D(glm::vec2 point, glm::vec2 point2)
{
	return pow(point2.x - point.x, 2) + pow(point2.y - point.y, 2);
}
float distance2D(glm::vec3 point, glm::vec3 point2)
{
	return pow(point2.x - point.x, 2) + pow(point2.z - point.z, 2);
}
float distance3D(glm::vec3 point, glm::vec3 point2)
{
	return pow(point2.x - point.x, 2) + pow(point2.y - point.y, 2) + pow(point2.z - point.z, 2);
}


void ShadowHandler::confShader(Terrain * terrain, PointLight light)
{
	terraihDepthShader.use();
	const unsigned int SHADOW_WIDTH = 4024, SHADOW_HEIGHT = 4024;


	GLenum err;
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glClear(GL_DEPTH_BUFFER_BIT);


	
	
	   glm::vec3 lightPos = light.position;
	
	
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));


	
			
		
			for (unsigned int i = 0; i < 6; ++i)
				terraihDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

			terraihDepthShader.setFloat("far_plane", far_plane);
			terraihDepthShader.setVec3("lightPos", lightPos);
			terrain->RenderShadow(terraihDepthShader);
		
			GLenum x = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (x == GL_FRAMEBUFFER_COMPLETE)
			{
			//	std::cout << "Frame buffer ONLINE" << std::endl;
			}
			else
			//	std::cout << "WHAT >> " << x << " ERROR "  << std::endl;

			while ((err = glGetError()) != GL_NO_ERROR)
			{
				//std::cout << "GL ERROR! " << err << std::endl;
			}

	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowHandler::~ShadowHandler()
{
}
