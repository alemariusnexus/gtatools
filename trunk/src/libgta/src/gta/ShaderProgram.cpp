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

#include "ShaderProgram.h"
#include "GLException.h"
#include <cstring>
#include <cstdio>


static int _NextProgramID = 0;




ShaderProgram::ShaderProgram()
		: program(glCreateProgram()), id(_NextProgramID++)
#ifdef GTA_USE_OPENGL_ES
		 , combinedVShader(NULL), combinedFShader(NULL)
#endif
{
}


ShaderProgram::~ShaderProgram()
{
	glUseProgram(0);
	glDeleteProgram(program);

#ifdef GTA_USE_OPENGL_ES
	if (combinedVShader)
		delete combinedVShader;
	if (combinedFShader)
		delete combinedFShader;
#endif
}


void ShaderProgram::link()
{
#ifdef GTA_USE_OPENGL_ES
	if (combinedVShader) {
		glesForceDetachShader(combinedVShader);
		delete combinedVShader;
	}
	if (combinedFShader) {
		glesForceDetachShader(combinedFShader);
		delete combinedFShader;
	}

	int vTotalLen = 0;
	int fTotalLen = 0;

	for (ShaderIterator it = shaders.begin() ; it != shaders.end() ; it++) {
		Shader* shader = *it;

		if (shader->getType() == GL_VERTEX_SHADER)
			vTotalLen += strlen(shader->glesGetCode()) + 20;
		else
			fTotalLen += strlen(shader->glesGetCode()) + 20;
	}

	char* vCombinedCode = new char[vTotalLen+1];
	char* fCombinedCode = new char[fTotalLen+1];

	char* vCombinedCodePtr = vCombinedCode;
	char* fCombinedCodePtr = fCombinedCode;

	for (ShaderIterator it = shaders.begin() ; it != shaders.end() ; it++) {
		Shader* shader = *it;

		if (shader->getType() == GL_VERTEX_SHADER) {
			strcpy(vCombinedCodePtr, shader->glesGetCode());
			vCombinedCodePtr += strlen(shader->glesGetCode());
			const char* lineCode = "\n#line 0\n";
			strcpy(vCombinedCodePtr, lineCode);
			vCombinedCodePtr += strlen(lineCode);
		} else {
			strcpy(fCombinedCodePtr, shader->glesGetCode());
			fCombinedCodePtr += strlen(shader->glesGetCode());
			const char* lineCode = "\n#line 0\n";
			strcpy(fCombinedCodePtr, lineCode);
			fCombinedCodePtr += strlen(lineCode);
		}
	}

	combinedVShader = new Shader(GL_VERTEX_SHADER);
	combinedFShader = new Shader(GL_FRAGMENT_SHADER);

	combinedVShader->loadSourceCode(vCombinedCode);
	combinedFShader->loadSourceCode(fCombinedCode);

	delete[] vCombinedCode;
	delete[] fCombinedCode;

	combinedVShader->glesForceCompile();
	combinedFShader->glesForceCompile();

	glesForceAttachShader(combinedVShader);
	glesForceAttachShader(combinedFShader);
#endif

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {
		GLint maxLength;
		GLint actualLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		char* log = new char[maxLength];
		glGetProgramInfoLog(program, maxLength, &actualLength, log);

		char* errmsg = new char[actualLength + 64];
		sprintf(errmsg, "Error linking shader program %d. Info log:\n\n%s", program, log);
		GLException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		delete[] log;
		throw ex;
	} else {
		GLint maxLength;
		GLint actualLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		if (maxLength != 0) {
			char* log = new char[maxLength];
			glGetProgramInfoLog(program, maxLength, &actualLength, log);

			printf("Shader program linking successful. Build log:\n==========\n%s\n==========\n", log);

			delete[] log;
		} else {
			printf("Shader program linking successful. Build log is empty\n");
		}
	}
}


#ifdef GTA_USE_OPENGL_ES
void ShaderProgram::glesForceAttachShader(Shader* shader)
{
	glAttachShader(program, shader->getGLIdentifier());
}


void ShaderProgram::glesForceDetachShader(Shader* shader)
{
	glDetachShader(program, shader->getGLIdentifier());
}
#endif


void ShaderProgram::attachShader(Shader* shader)
{
#ifndef GTA_USE_OPENGL_ES
	glAttachShader(program, shader->getGLIdentifier());
#endif
	shaders.push_back(shader);
}


void ShaderProgram::detachShader(Shader* shader)
{
#ifndef GTA_USE_OPENGL_ES
	glDetachShader(program, shader->getGLIdentifier());
#endif

	for (ShaderIterator it = shaders.begin() ; it != shaders.end() ; it++) {
		if (*it == shader) {
			shaders.erase(it);
		}
	}
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
