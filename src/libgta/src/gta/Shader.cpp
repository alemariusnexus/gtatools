/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.
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
