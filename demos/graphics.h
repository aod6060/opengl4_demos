#pragma once

struct Texture2D {
public:

	GLuint id;
	GLuint width;
	GLuint height;

	void init(std::string fn);

	void bind(GLenum e = GL_TEXTURE0);

	void unbind(GLenum e = GL_TEXTURE0);

	void release();

	GLuint getWidth();

	GLuint getHeight();

	GLuint getID();

};