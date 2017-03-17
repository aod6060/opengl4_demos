#include "sys.h"


// Texture2D
void Texture2D::init(std::string fn) {
	SDL_Surface* surface = IMG_Load(fn.c_str());

	if (surface == 0) {
		std::cout << fn << " doesn't exist..." << std::endl;
		return;
	}

	this->width = surface->w;
	this->height = surface->h;

	GLenum format = GL_RGB;

	if (surface->format->BytesPerPixel == 4) {
		format = GL_RGBA;
	}

	glGenTextures(1, &id);

	bind();

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		format,
		this->width,
		this->height,
		0,
		format,
		GL_UNSIGNED_BYTE,
		surface->pixels
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unbind();

	SDL_FreeSurface(surface);
}

void Texture2D::bind(GLenum e) {
	glActiveTexture(e);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::unbind(GLenum e) {
	glActiveTexture(e);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::release() {
	glDeleteTextures(1, &id);
}

GLuint Texture2D::getWidth() {
	return this->id;
}

GLuint Texture2D::getHeight() {
	return this->id;
}

GLuint Texture2D::getID() {
	return this->id;
}
