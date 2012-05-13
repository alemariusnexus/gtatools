/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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
#include <res_global_shader.h>
#include <res_global_vertex_shader.h>
#include <res_global_fragment_shader.h>

using std::streamoff;



Shader::Shader(GLenum type, const CString& name)
		: type(type),
#ifdef GTA_USE_OPENGL_ES
		  shader(0),
#else
		  shader(glCreateShader(type)),
#endif
		  name(name)
{
}


Shader::~Shader()
{
	glDeleteShader(shader);
}


void Shader::loadSourceCode(const CString& code)
{
	int major, minor;
	gtaglGetVersion(major, minor);

	int versionGLSL;

#ifdef GTA_USE_OPENGL_ES
	versionGLSL = 100;
#else
	if (gtaglIsVersionSupported(3, 1)) {
		versionGLSL = 140;
	} else {
		versionGLSL = 110;
	}
#endif

	char prepCode[256];
	sprintf(prepCode,
#ifndef GTA_USE_OPENGL_ES
			"#version %d\n"
			"\n"
#endif
			"#ifndef GTAGL_VERSION_MAJOR\n"
			"#define GTAGL_VERSION_MAJOR %d\n"
			"#endif\n"
			"\n"
			"#ifndef GTAGL_VERSION_MINOR\n"
			"#define GTAGL_VERSION_MINOR %d\n"
			"#endif\n"
			"\n"
#ifdef GTA_USE_OPENGL_ES
			"#ifndef GTAGL_ES\n"
			"#define GTAGL_ES\n"
			"#endif\n"
			"\n"
#endif
			"#line 0\n"
#ifndef GTA_USE_OPENGL_ES
			, versionGLSL
#endif
			, major, minor);

	CString newCode(prepCode);

	newCode.append(CString((const char*) res_global_shader_data, sizeof(res_global_shader_data)));
	newCode.append("\n\n#line 0\n");

	if (type == GL_VERTEX_SHADER) {
		newCode.append(CString((const char*) res_global_vertex_shader_data,
				sizeof(res_global_vertex_shader_data)));
	} else {
		newCode.append(CString((const char*) res_global_fragment_shader_data,
				sizeof(res_global_fragment_shader_data)));
	}

	newCode.append("\n\n#line 0\n");

	newCode.append(code);


#ifdef GTA_USE_OPENGL_ES
	this->code = newCode;
#else
	GLint len = newCode.length();
	const char* ccode = newCode.get();
	glShaderSource(shader, 1, &ccode, &len);
#endif
}


void Shader::loadSourceCode(const File& file)
{
	istream* stream = file.openInputStream();
	stream->seekg(0, istream::end);
	unsigned int len = (unsigned int) stream->tellg();
	stream->seekg(0, istream::beg);
	char* buffer = new char[len+1];
	stream->read(buffer, len);
	buffer[len] = '\0';
	delete stream;
	loadSourceCode(CString::from(buffer));
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
#ifdef GTA_USE_OPENGL_ES
	if (shader == 0) {
		shader = glCreateShader(type);
	}

	CString newCode("#version 100\n");
	newCode.append(code);

	GLint len = newCode.length();
	const char* ccode = newCode.get();
	glShaderSource(shader, 1, &ccode, &len);
#endif

	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		GLint maxLength;
		GLint actualLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char* log = new char[maxLength];
		glGetShaderInfoLog(shader, maxLength, &actualLength, log);

		size_t nameLen = name.get() ? name.length() : 16;
		char* errmsg = new char[actualLength + nameLen + 64];
		sprintf(errmsg, "Error compiling shader \"%s\" [#%d]. Info log:\n\n%s",
				name.get() ? name.get() : "[UNNAMED]", shader, log);
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

			printf("Successfully compiled shader \"%s\" [#%d]. Build log:\n==========\n%s\n==========\n\n",
					name.get() ? name.get() : "[UNNAMED]", shader, log);

			delete[] log;
		} else {
			printf("Successfully compiled shader \"%s\" [#%d]. Build log is empty\n\n",
					name.get() ? name.get() : "[UNNAMED]", shader);
		}
	}
}
