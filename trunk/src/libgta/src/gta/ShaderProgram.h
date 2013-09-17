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

#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include <gta/config.h>
#include <gtaformats/util/StringComparator.h>
#include <vector>
#include <map>
#include "gl.h"
#include "Shader.h"

using std::map;
using std::vector;


class ShaderProgram {
private:
	typedef vector<Shader*> ShaderList;
	typedef ShaderList::iterator ShaderIterator;
	typedef ShaderList::const_iterator ConstShaderIterator;
	typedef map<CString, GLint, StringComparator> AttribUniformMap;

public:
	static void disableShaders();

public:
	ShaderProgram(const CString& name = CString());
	~ShaderProgram();
	void link();
	void attachShader(Shader* shader);
	void detachShader(Shader* shader);
	void makeCurrent();
	GLint getAttributeLocation(const CString& name) const;
	GLint getUniformLocation(const CString& name) const;
	bool operator==(const ShaderProgram& other) const { return id == other.id; }
	bool operator!=(const ShaderProgram& other) const { return !(*this == other); }

private:
#ifdef GTA_USE_OPENGL_ES
	void glesForceAttachShader(Shader* shader);
	void glesForceDetachShader(Shader* shader);
#endif

private:
	GLuint program;
	unsigned int id;
	ShaderList shaders;
#ifdef GTA_USE_OPENGL_ES
	Shader* combinedVShader;
	Shader* combinedFShader;
#endif
	CString name;
	AttribUniformMap attribCache;
	AttribUniformMap uniformCache;
};

#endif /* SHADERPROGRAM_H_ */
