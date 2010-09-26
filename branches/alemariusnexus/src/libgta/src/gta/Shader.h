/*
 * Shader.h
 *
 *  Created on: 23.09.2010
 *      Author: alemariusnexus
 */

#ifndef SHADER_H_
#define SHADER_H_

#include <GL/glew.h>
#include <gtaformats/util/File.h>


class Shader {
	friend class ShaderProgram;

public:
	Shader(GLenum type);
	void loadSourceCode(const char* code, int length = -1);
	void loadSourceCode(const File& file);
	void compile();

private:
	GLuint getGLIdentifier() { return shader; }

private:
	GLuint shader;
	GLenum type;
};

#endif /* SHADER_H_ */
