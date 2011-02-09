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
 */

#include "ShaderProgram.h"
#include "GLException.h"
#include <cstring>
#include <cstdio>




ShaderProgram::ShaderProgram()
		: program(glCreateProgram())
{
}


ShaderProgram::~ShaderProgram()
{
	glUseProgram(0);
	glDeleteProgram(program);
}


void ShaderProgram::link()
{
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {
		GLint maxLength;
		GLint actualLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = new GLchar[maxLength];
		glGetProgramInfoLog(program, maxLength, &actualLength, log);

		char* errmsg = new char[actualLength + 64];
		sprintf(errmsg, "Error linking shader program %d. Info log:\n\n%s", program, log);
		GLException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
}


void ShaderProgram::attachShader(Shader* shader)
{
	glAttachShader(program, shader->getGLIdentifier());
}


void ShaderProgram::detachShader(Shader* shader)
{
	glDetachShader(program, shader->getGLIdentifier());
}


void ShaderProgram::makeCurrent()
{
	glUseProgram(program);
}


GLint ShaderProgram::getAttributeLocation(const char* name) const
{
	GLint attrib = glGetAttribLocation(program, name);
	return attrib;
}


GLint ShaderProgram::getUniformLocation(const char* name) const
{
	GLint uniform = glGetUniformLocation(program, name);
	return uniform;
}


void ShaderProgram::disableShaders()
{
	glUseProgram(0);
}
