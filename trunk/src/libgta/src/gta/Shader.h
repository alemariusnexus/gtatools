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

#ifndef SHADER_H_
#define SHADER_H_

#include "config.h"
#include <GL/glew.h>
#include <gtaformats/util/File.h>


class Shader {
	friend class ShaderProgram;

public:
	Shader(GLenum type);
	void loadSourceCode(const char* code, int length = -1);
	void loadSourceCode(const File& file);
	void compile();

private:
	GLuint getGLIdentifier() { return shader; }

private:
	GLuint shader;
	GLenum type;
};

#endif /* SHADER_H_ */
