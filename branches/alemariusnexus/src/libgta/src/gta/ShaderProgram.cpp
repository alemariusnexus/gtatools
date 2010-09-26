/*
 * ShaderProgram.cpp
 *
 *  Created on: 23.09.2010
 *      Author: alemariusnexus
 */

#include "ShaderProgram.h"
#include "GLException.h"
#include <cstring>
#include <cstdio>



ShaderProgram* currentProgram = NULL;


ShaderProgram::ShaderProgram()
		: program(glCreateProgram())
{
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
	currentProgram = this;
	glUseProgram(program);
}


ShaderProgram* ShaderProgram::getCurrent()
{
	return currentProgram;
}


GLint ShaderProgram::getAttributeLocation(const char* name)
{
	GLint attrib = glGetAttribLocation(program, name);
	return attrib;
}


GLint ShaderProgram::getUniformLocation(const char* name)
{
	GLint uniform = glGetUniformLocation(program, name);
	return uniform;
}
