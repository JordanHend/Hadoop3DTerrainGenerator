#ifndef TERRAIN_H
#define TERRAIN_H
#include <glad\glad.h>
#include <gl\GL.h>
#include <stb_image.h>
#include <string>
#include <iostream>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/std_based_type.hpp>
#include <vector>
#include "Shader.h"
#include "Common.h"
struct HeightMap
{
	unsigned char * data;
	std::string name;
	unsigned int width, height;
	unsigned int channels;
};

struct TerrainVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;


};

struct Light
{
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 color;

	float constant;
	float linear;
	float quadratic;
};

class Terrain
{
public:

	std::string heightMapFilename;
	std::string textureFileName;
	std::string blendmap;
	// Max height of terrain in world unis.
	float heightScale;
	// Space between terrain vertices in world units for both the X and Z axes.
	float blockScale;
	Terrain()
	{
		heightMapShader.init("TerrainVertex.shdr", "TerrainFrag.shdr");
	};


	/*
	Loads a height map using an image given by filename. 
	@param bitsPerPixel is not neededed, stb_image automatically gives the bits per pixel when it loads the image and it uses that automatically.
	@param heightscale gives the maximum height of the terrain.
	@param blockscale gives the size per pixel. low values = more sharp terrain, higher values = bigger size but soother.
	*/
	bool loadHeightMap(const std::string & filename, unsigned char bitsPerPixel, float heightScale, float blockScale);

	//Used to get height at a location. Used to make sure camera doesn't go through the terrain.
	float getHeightAt(const glm::vec3 & position);

	//Loads the base texture
	bool loadTexture(const std::string & filename);

	//Renders terrain.
	void Render();

	//Render's terrain using the depth shader for rendering shadows.
	void RenderShadow(Shader shader);

	//The shader for the terrain.
	Shader heightMapShader;

	~Terrain();

	//Vertex storage
	std::vector<TerrainVertex> vertices;

	//Sets the blend map texture. 
	void setBlendTexture(std::string filename);

	//Sets the RGB textures to be used by blendmapping.
	void setRGBTextures(std::vector<std::string> filenames);


private:
	unsigned int VBO = 1000;
	unsigned int VAO;
	unsigned int EBO;



	std::vector<int> indices;
	std::vector<unsigned int> blendTextures;
	unsigned int blendtexture = 1000;
	std::vector<unsigned int> textures;
	glm::mat4 localToWorldMatrix;
	glm::mat4 inv_localToWorldMatrix;

	glm::ivec2 HeightMapDimensions;


	HeightMap HeightmapFromFile(const char *path, bool gamma);
	void GenerateIndexBuffers();
	void GenerateNormals();
	void GenerateVertexBuffers();
	inline float GetHeightValue(const unsigned char* data, unsigned char numBytes);
	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma);
	

};

#endif