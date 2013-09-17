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

#ifndef SHADER_H_
#define SHADER_H_

#include <gta/config.h>
#include "gl.h"
#include <gtaformats/util/File.h>
#include <gtaformats/util/CString.h>


class Shader {
	friend class ShaderProgram;

public:
	Shader(GLenum type, const CString& name = CString());
	~Shader();
	void loadSourceCode(const CString& code);
	void loadSourceCode(const File& file);
	void compile();
	GLenum getType() const { return type; }
	CString getName() const { return name; }

private:
#ifdef GTA_USE_OPENGL_ES
	void glesForceCompile();
	CString glesGetCode() const { return code; }
#endif
	GLuint getGLIdentifier() { return shader; }

private:
	GLenum type;
#ifdef GTA_USE_OPENGL_ES
	CString code;
#endif
	GLuint shader;
	CString name;
};

#endif /* SHADER_H_ */
