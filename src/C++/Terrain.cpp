#include "Terrain.h"








bool Terrain::loadHeightMap(const std::string & filename, unsigned char bitsPerPixel, float heightScale, float blockScale)
{
	this->heightMapFilename = filename;
	this->heightScale = heightScale;
	this->blockScale = blockScale;
	vertices.clear();;
	indices.clear();
	HeightMap map = HeightmapFromFile(filename.c_str(), true);


	unsigned int numVerts = map.width * map.height;
	vertices.resize(numVerts);
	//m_ColorBuffer.resize(numVerts);

	HeightMapDimensions = glm::uvec2(map.width, map.height);

	// Size of the terrain in world units
	float terrainWidth = (map.width - 1) * blockScale;
	float terrainHeight = (map.height - 1) * blockScale;

	float halfTerrainWidth = terrainWidth * 0.5f;
	float halfTerrainHeight = terrainHeight * 0.5f;

	for (unsigned int j = 0; j < map.height; ++j)
	{
		for (unsigned i = 0; i < map.width; ++i)
		{
			unsigned int index = (j * map.width) + i;
			//assert(index * bytesPerPixel < fileSize);
			float heightValue = GetHeightValue(&map.data[index * map.channels],map.channels);

			float S = (i / (float)(map.width - 1));
			float T = (j / (float)(map.height - 1));

			float X = (S * terrainWidth) - halfTerrainWidth;
			float Y = heightValue * heightScale;
			float Z = (T * terrainHeight) - halfTerrainHeight;

	
			vertices[index].normal = glm::vec3(0);
			vertices[index].position = glm::vec3(X, Y, Z);
			vertices[index].texCoords = glm::vec2(S, T );

	
		}
	}
	std::cout << "Terrain has been loaded!" << std::endl;
	delete[] map.data;

	GenerateIndexBuffers();
	GenerateNormals();
	GenerateVertexBuffers();

	return true;
}

float Terrain::getHeightAt(const glm::vec3 & position)
{
	float height = -FLT_MAX;
	// Check if the terrain dimensions are valid
	if (HeightMapDimensions.x < 2 || HeightMapDimensions.y < 2) return height;

	// Width and height of the terrain in world units
	float terrainWidth = (HeightMapDimensions.x - 1) * blockScale;
	float terrainHeight = (HeightMapDimensions.y - 1) * blockScale;
	float halfWidth = terrainWidth * 0.5f;
	float halfHeight = terrainHeight * 0.5f;

	// Multiple the position by the inverse of the terrain matrix 
	// to get the position in terrain local space
	glm::vec3 terrainPos = glm::vec3(inv_localToWorldMatrix * glm::vec4(position, 1.0f));
	glm::vec3 invBlockScale(1.0f / blockScale, 0.0f, 1.0f / blockScale);

	// Calculate an offset and scale to get the vertex indices
	glm::vec3 offset(halfWidth, 0.0f, halfHeight);

	// Get the 4 vertices that make up the triangle we're over
	glm::vec3 vertexIndices = (terrainPos + offset) * invBlockScale;

	int u0 = (int)floorf(vertexIndices.x);
	int u1 = u0 + 1;
	int v0 = (int)floorf(vertexIndices.z);
	int v1 = v0 + 1;

	if (u0 >= 0 && u1 < HeightMapDimensions.x && v0 >= 0 && v1 < HeightMapDimensions.y)
	{
		glm::vec3 p00 = vertices[(v0 * HeightMapDimensions.x) + u0].position;    // Top-left vertex
		glm::vec3 p10 = vertices[(v0 * HeightMapDimensions.x) + u1].position;    // Top-right vertex
		glm::vec3 p01 = vertices[(v1 * HeightMapDimensions.x) + u0].position;    // Bottom-left vertex
		glm::vec3 p11 = vertices[(v1 * HeightMapDimensions.x) + u1].position;    // Bottom-right vertex

																				  
		float percentU = vertexIndices.x - u0;
		float percentV = vertexIndices.z - v0;

		glm::vec3 dU, dV;
		if (percentU > percentV)
		{   // Top triangle
			dU = p10 - p00;
			dV = p11 - p10;
		}
		else
		{   // Bottom triangle
			dU = p11 - p01;
			dV = p01 - p00;
		}

		glm::vec3 heightPos = p00 + (dU * percentU) + (dV * percentV);
		// Convert back to world-space by multiplying by the terrain's world matrix
		heightPos = glm::vec3(localToWorldMatrix * glm::vec4(heightPos, 1));
		height = heightPos.y;
	}

	return height;
}

bool Terrain::loadTexture(const std::string & filename)
{
	this->textureFileName = filename;
	textures.push_back(TextureFromFile(filename.c_str(), "", true));
	return textures.size() != 0;
}

void Terrain::Render()
{

	if (indices.size() != 0)
	{
		heightMapShader.use();
		heightMapShader.setMat4("model", glm::mat4(1.0f));
		heightMapShader.setMat4("view", mainCamera.GetViewMatrix());
		heightMapShader.setMat4("projection", projection);
		//Loop through textures, and bind them where appropriate.
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0);
			std::string name = "texture_diffuse1";
			std::string number;


			// Set the sampler to the correct texture unit
			heightMapShader.setInt(name.c_str(), 0);
			// Then bind the texture.
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
		// bind rgb blend textures.
		if (blendTextures.size() != 0)
		{
			glActiveTexture(GL_TEXTURE1);
			heightMapShader.setInt("texture_r", 1);
			glBindTexture(GL_TEXTURE_2D, blendTextures[0]);

			glActiveTexture(GL_TEXTURE2);
			heightMapShader.setInt("texture_g", 2);
			glBindTexture(GL_TEXTURE_2D, blendTextures[1]);

			glActiveTexture(GL_TEXTURE3);
			heightMapShader.setInt("texture_b", 3);
			glBindTexture(GL_TEXTURE_2D, blendTextures[2]);
		}
		// and finally, bind blend texture.

		if (blendtexture != 1000)
		{
			glActiveTexture(GL_TEXTURE4);


			// Set the sampler to the correct texture unit
			heightMapShader.setInt("blendmap", 4);
			// Then bind the texture.
			glBindTexture(GL_TEXTURE_2D, blendtexture);
		}
		//Bind VAO and draw
	
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		
		// Set back to default
		glActiveTexture(GL_TEXTURE0);

	}
}

void Terrain::RenderShadow(Shader shader)
{
	shader.setMat4("model", glm::mat4(1.0f));
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}




Terrain::~Terrain()
{
}

void Terrain::setBlendTexture(std::string filename)
{
	this->blendmap = filename;
		if(blendtexture != 1000)
	glDeleteTextures(1, &blendtexture);
		
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
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
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}


	blendtexture = textureID;
}

void Terrain::setRGBTextures(std::vector<std::string> filenames)
{
	if (filenames.size() == 3)
	{
		if (blendTextures.size() != 0)
		{
			for (unsigned int i = 0; i < blendTextures.size(); i++)
			{
				glDeleteTextures(1, &blendTextures[i]);
			}
			blendTextures.clear();
		}

		for (unsigned int i = 0; i < filenames.size(); i++)
		{
			std::string filename = filenames[i];


			unsigned int textureID;
			glGenTextures(1, &textureID);

			int width, height, nrComponents;
			unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
			if (data)
			{
				GLenum format;
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
				std::cout << "Texture failed to load at path: " << filename << std::endl;
				stbi_image_free(data);
			}
			blendTextures.push_back(textureID);
		}
	}
}

HeightMap Terrain::HeightmapFromFile(const char * path, bool gamma)
{

	HeightMap map;

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		map.data = data;
		map.name = path;
		map.height = height;
		map.width = width;
		map.channels = nrComponents;

		
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return map;
}

void Terrain::GenerateIndexBuffers()
{
	if (HeightMapDimensions.x < 2 || HeightMapDimensions.y < 2)
	{
		// Terrain hasn't been loaded, or is of an incorrect size
		return;
	}

	const unsigned int terrainWidth = HeightMapDimensions.x;
	const unsigned int terrainHeight = HeightMapDimensions.y;

	// 2 triangles for every quad of the terrain mesh
	const unsigned int numTriangles = (terrainWidth - 1) * (terrainHeight - 1) * 2;

	// 3 indices for each triangle in the terrain mesh
	indices.resize(numTriangles * 3);

	unsigned int index = 0; // Index in the index buffer
	for (unsigned int j = 0; j < (terrainHeight - 1); ++j)
	{
		for (unsigned int i = 0; i < (terrainWidth - 1); ++i)
		{
			int vertexIndex = (j * terrainWidth) + i;
			// Top triangle (T0)
			indices[index++] = vertexIndex;                           // V0
			indices[index++] = vertexIndex + terrainWidth + 1;        // V3
			indices[index++] = vertexIndex + 1;                       // V1
																			// Bottom triangle (T1)
			indices[index++] = vertexIndex;                           // V0
			indices[index++] = vertexIndex + terrainWidth;            // V2
			indices[index++] = vertexIndex + terrainWidth + 1;        // V3
		}
	}
}

void Terrain::GenerateNormals()
{
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		glm::vec3 v0 = vertices[indices[i + 0]].position;
		glm::vec3 v1 = vertices[indices[i + 1]].position;
		glm::vec3 v2 = vertices[indices[i + 2]].position;

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		vertices[indices[i + 0]].normal += normal;
		vertices[indices[i + 1]].normal += normal;
		vertices[indices[i + 2]].normal += normal;
	}

	const glm::vec3 UP(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		vertices[i].normal = glm::normalize(vertices[i].normal);


	}
}

void Terrain::GenerateVertexBuffers()
{
	if (VBO != 1000)
	{
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &EBO);
	}
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);


	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Positions

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);
	glEnableVertexAttribArray(0);
	// Normals

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, normal));
	glEnableVertexAttribArray(1);
	// Texture coords

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, texCoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

inline float Terrain::GetHeightValue(const unsigned char * data, unsigned char numBytes)
{
	switch (numBytes)
	{
	case 1:
	{
		return (unsigned char)(data[0]) / (float)0xff;
	}
	break;
	case 2:
	{
		return (unsigned short)(data[1] << 8 | data[0]) / (float)0xffff;
	}
	break;
	case 3:
	{
		return (data[2] << 16 | data[1] << 8 | data[0]) / (float)0xffffff;
	}
	case 4:
	{
		return (float)(data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]) / (float)0xffffffff;
	}
	break;
	default:
	{
		assert(false);  // Height field with non standard pixel sizes	}
		break;
	}

	return 0.0f;
	}
}

unsigned int Terrain::TextureFromFile(const char * path, const std::string & directory, bool gamma)
{
	if (textures.size() != 0)
	{
		for (int i = 0; i < textures.size(); i++)
		{
			glDeleteTextures(1, &textures.at(i));
		}
		textures.clear();
	}

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
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

