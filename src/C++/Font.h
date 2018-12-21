#pragma once

#include "Common.h"
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H  
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

class Font
{
public:
	Font(std::string fontDirectory);
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	
	~Font();

private:
	GLuint VAO;
	GLuint VBO;
	

	std::map<GLchar, Character> Characters;
};

