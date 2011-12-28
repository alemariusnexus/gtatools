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

#ifndef SHADER_H_
#define SHADER_H_

#include <gta/config.h>
#include "gl.h"
#include <gtaformats/util/File.h>


class Shader {
	friend class ShaderProgram;

public:
	Shader(GLenum type, const char* name = NULL);
	~Shader();
	void loadSourceCode(const char* code, int length = -1);
	void loadSourceCode(const File& file);
	void compile();
	GLenum getType() const { return type; }
	const char* getName() const { return name; }

private:
#ifdef GTA_USE_OPENGL_ES
	void glesForceCompile();
	const char* glesGetCode() const { return code; }
#endif
	GLuint getGLIdentifier() { return shader; }

private:
	GLenum type;
#ifdef GTA_USE_OPENGL_ES
	char* code;
#endif
	GLuint shader;
	char* name;
};

#endif /* SHADER_H_ */
