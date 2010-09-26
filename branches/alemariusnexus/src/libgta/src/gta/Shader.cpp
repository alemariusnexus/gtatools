/*
 * Shader.cpp
 *
 *  Created on: 23.09.2010
 *      Author: alemariusnexus
 */

#include "Shader.h"
#include "GLException.h"
#include <cstring>
#include <cstdio>
#include <gtaformats/util/stream/InputStream.h>



Shader::Shader(GLenum type)
		: shader(glCreateShader(type)), type(type)
{
}


void Shader::loadSourceCode(const char* code, int length)
{
	if (length == -1) {
		length = strlen(code);
	}

	glShaderSource(shader, 1, &code, &length);
}


void Shader::loadSourceCode(const File& file)
{
	InputStream* stream = file.openStream();
	stream->seek(0, InputStream::STREAM_SEEK_END);
	InputStream::streampos len = stream->tell();
	stream->seek(0, InputStream::STREAM_SEEK_BEGIN);
	char* buffer = new char[len];
	stream->read(buffer, len);
	delete stream;
	loadSourceCode(buffer, len);
	delete[] buffer;
}


void Shader::compile()
{
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		GLint maxLength;
		GLint actualLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &actualLength, log);

		char* errmsg = new char[actualLength + 64];
		sprintf(errmsg, "Error compiling shader %d. Info log:\n\n%s", shader, log);
		delete[] log;
		GLException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
}
