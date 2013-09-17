/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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
#include <utility>

using std::pair;


static int _NextProgramID = 0;




ShaderProgram::ShaderProgram(const CString& name)
		: program(glCreateProgram()), id(_NextProgramID++),
#ifdef GTA_USE_OPENGL_ES
		 combinedVShader(NULL), combinedFShader(NULL),
#endif
		 name(name)
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
	uniformCache.clear();
	attribCache.clear();

#ifdef GTA_USE_OPENGL_ES
	if (combinedVShader) {
		glesForceDetachShader(combinedVShader);
		delete combinedVShader;
	}
	if (combinedFShader) {
		glesForceDetachShader(combinedFShader);
		delete combinedFShader;
	}

	size_t vTotalLen = 0;
	size_t fTotalLen = 0;

	for (ShaderIterator it = shaders.begin() ; it != shaders.end() ; it++) {
		Shader* shader = *it;

		if (shader->getType() == GL_VERTEX_SHADER)
			vTotalLen += shader->glesGetCode().length() + 20;
		else
			fTotalLen += shader->glesGetCode().length() + 20;
	}

	char* vCombinedCode = new char[vTotalLen+1];
	char* fCombinedCode = new char[fTotalLen+1];

	char* vCombinedCodePtr = vCombinedCode;
	char* fCombinedCodePtr = fCombinedCode;

	size_t combinedVShaderNameLen = 64;
	size_t combinedFShaderNameLen = 64;

	for (ShaderIterator it = shaders.begin() ; it != shaders.end() ; it++) {
		Shader* shader = *it;

		CString name = shader->getName();

		if (shader->getType() == GL_VERTEX_SHADER) {
			combinedVShaderNameLen += (name.get() ? name.length() + 32 : 16);
		} else {
			combinedFShaderNameLen += (name.get() ? name.length() + 32 : 16);
		}
	}

	char* combinedVShaderName = new char[combinedVShaderNameLen];
	char* combinedFShaderName = new char[combinedFShaderNameLen];

	strcpy(combinedVShaderName, "Combined Vertex Shader {");
	strcpy(combinedFShaderName, "Combined Fragment Shader {");

	bool firstVShader = true;
	bool firstFShader = true;

	for (ShaderIterator it = shaders.begin() ; it != shaders.end() ; it++) {
		Shader* shader = *it;

		if (shader->getType() == GL_VERTEX_SHADER) {
			strcpy(vCombinedCodePtr, shader->glesGetCode().get());
			vCombinedCodePtr += shader->glesGetCode().length();
			const char* lineCode = "\n#line 0\n";
			strcpy(vCombinedCodePtr, lineCode);
			vCombinedCodePtr += strlen(lineCode);

			if (!firstVShader)
				strcat(combinedVShaderName, "; ");

			if (shader->getName().get())
				strcat(combinedVShaderName, shader->getName().get());
			else
				strcat(combinedVShaderName, "[UNNAMED]");
			firstVShader = false;
		} else {
			strcpy(fCombinedCodePtr, shader->glesGetCode().get());
			fCombinedCodePtr += shader->glesGetCode().length();
			const char* lineCode = "\n#line 0\n";
			strcpy(fCombinedCodePtr, lineCode);
			fCombinedCodePtr += strlen(lineCode);

			if (!firstFShader)
				strcat(combinedFShaderName, "; ");

			if (shader->getName().get())
				strcat(combinedFShaderName, shader->getName().get());
			else
				strcat(combinedFShaderName, "[UNNAMED]");
			firstFShader = false;
		}
	}

	strcat(combinedVShaderName, "}");
	strcat(combinedFShaderName, "}");

	combinedVShader = new Shader(GL_VERTEX_SHADER, CString::from(combinedVShaderName));
	combinedFShader = new Shader(GL_FRAGMENT_SHADER, CString::from(combinedFShaderName));

	combinedVShader->loadSourceCode(CString::from(vCombinedCode));
	combinedFShader->loadSourceCode(CString::from(fCombinedCode));

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

		size_t nameLen = name.get() ? name.length() : 16;
		char* errmsg = new char[actualLength + 64 + nameLen];
		sprintf(errmsg, "Error linking shader program \"%s\" [#%d]. Info log:\n\n%s",
				name.get() ? name.get() : "[UNNAMED]", program, log);
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

			printf("Successfully compiled shader program \"%s\" [#%d]. Build log:\n==========\n%s\n==========\n",
					name.get() ? name.get() : "[UNNAMED]", program, log);

			delete[] log;
		} else {
			printf("Successfully compiled shader program \"%s\" [#%d]. Build log is empty\n",
					name.get() ? name.get() : "[UNNAMED]", program);
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
			break;
		}
	}
}


void ShaderProgram::makeCurrent()
{
	glUseProgram(program);
}


GLint ShaderProgram::getAttributeLocation(const CString& name) const
{
	AttribUniformMap::const_iterator it = attribCache.find(name);

	if (it == attribCache.end()) {
		GLint attrib = glGetAttribLocation(program, name.get());
		const_cast<ShaderProgram*>(this)->attribCache.insert(pair<CString, GLint>(name, attrib));
		return attrib;
	}

	return it->second;
}


GLint ShaderProgram::getUniformLocation(const CString& name) const
{
	AttribUniformMap::const_iterator it = uniformCache.find(name);

	if (it == uniformCache.end()) {
		GLint uniform = glGetUniformLocation(program, name.get());
		const_cast<ShaderProgram*>(this)->uniformCache.insert(pair<CString, GLint>(name, uniform));
		return uniform;
	}

	return it->second;
}


void ShaderProgram::disableShaders()
{
	glUseProgram(0);
}
