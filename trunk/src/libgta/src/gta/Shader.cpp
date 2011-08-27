/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "Shader.h"
#include "GLException.h"
#include <cstring>
#include <cstdio>

using std::streamoff;



Shader::Shader(GLenum type)
		: type(type),
#ifdef GTA_USE_OPENGL_ES
		  code(NULL), shader(0)
#else
		  shader(glCreateShader(type))
#endif
{
}


void Shader::loadSourceCode(const char* code, int length)
{
	if (length == -1) {
		length = strlen(code);
	}

#ifdef GTA_USE_OPENGL_ES
	this->code = new char[length+1];
	strncpy(this->code, code, length);
	this->code[length] = '\0';
#else
	glShaderSource(shader, 1, &code, &length);
#endif
}


void Shader::loadSourceCode(const File& file)
{
	istream* stream = file.openInputStream();
	stream->seekg(0, istream::end);
	unsigned int len = (unsigned int) stream->tellg();
	stream->seekg(0, istream::beg);
	char* buffer = new char[len];
	stream->read(buffer, len);
	delete stream;
	loadSourceCode(buffer, len);
	delete[] buffer;
}


#ifdef GTA_USE_OPENGL_ES
void Shader::compile() {}
#endif


#ifdef GTA_USE_OPENGL_ES
void Shader::glesForceCompile()
#else
void Shader::compile()
#endif
{
	GLException::checkError("Error 0");
#ifdef GTA_USE_OPENGL_ES
	if (shader == 0) {
		shader = glCreateShader(type);
	}
	GLException::checkError("Error 0.1");

	int len = strlen(code);
	const char* ccode = code;
	glShaderSource(shader, 1, &ccode, &len);
	GLException::checkError("Error 0.2");
#endif

	glCompileShader(shader);
	GLint status;
	GLException::checkError("Error 1");
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	GLException::checkError("Error 2");

	if (status == GL_FALSE) {
		GLint maxLength;
		GLint actualLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char* log = new char[maxLength];
		glGetShaderInfoLog(shader, maxLength, &actualLength, log);

		char* errmsg = new char[actualLength + 64];
		sprintf(errmsg, "Error compiling shader %d. Info log:\n\n%s", shader, log);
		delete[] log;
		GLException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	} else {
		GLint maxLength;
		GLint actualLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		
		if (maxLength != 0) {
			char* log = new char[maxLength];
			glGetShaderInfoLog(shader, maxLength, &actualLength, log);

			printf("Shader compilation successful. Build log:\n==========\n%s\n==========\n", log);

			delete[] log;
		} else {
			printf("Shader compilation successful. Build log is empty\n");
		}
	}
}
