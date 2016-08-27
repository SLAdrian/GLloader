#include "Cube.h"

GLuint Cube::vao, Cube::vbo;


Cube::Cube()
{
	if (vao == 0 || vbo == 0)
	{
		glGenVertexArrays(1, &this->vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &this->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof cubevertices, cubevertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}



void Cube::render() {

	glBindVertexArray(this->vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

glm::mat4 * Cube::getModel()
{
	return &model;
}

TexturedCube::TexturedCube() : Cube()
{
	return;
}

void TexturedCube::loadTextures(std::string tex, std::string spec = "")
{
	this->texture = loadTexture(tex);

	if (spec != "")
		this->specular = loadTexture(spec);
}

GLuint loadTexture(std::string path)
{
	GLuint load;
	glGenTextures(1, &load);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, load);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };

	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


	SDL_Surface* img = IMG_Load(path.c_str());
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->w, img->h,0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);

	glGenerateMipmap(GL_TEXTURE_2D);
	SDL_FreeSurface(img);

	glBindTexture(GL_TEXTURE_2D, 0);

	int x = glGetError();
	if (x != 0)
		std::cout << x << '\n';

	else
		std::cout << "Loaded IMG: " << path << std::endl;

	return load;
}