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

#include "BasicTransparencyAlgorithm.h"


#include <res_vertex_default_shader.h>
#include <res_fragment_default_shader.h>



void BasicTransparencyAlgorithm::install()
{
	if (!inited) {
		const char* vertexDefaultData;
		const char* fragmentDefaultData;

		int vertexDefaultDataLen;
		int fragmentDefaultDataLen;

		vertexDefaultData					= (const char*) res_vertex_default_shader_data;
		fragmentDefaultData					= (const char*) res_fragment_default_shader_data;

		vertexDefaultDataLen					= sizeof(res_vertex_default_shader_data);
		fragmentDefaultDataLen					= sizeof(res_fragment_default_shader_data);

		vertexDefaultShader = new Shader(GL_VERTEX_SHADER, "Basic Transparency Algorithm Vertex Shader");
		vertexDefaultShader->loadSourceCode(CString(vertexDefaultData, vertexDefaultDataLen));
		vertexDefaultShader->compile();

		fragmentDefaultShader = new Shader(GL_FRAGMENT_SHADER,
				"Basic Transparency Algorithm Fragment Shader");
		fragmentDefaultShader->loadSourceCode(CString(fragmentDefaultData, fragmentDefaultDataLen));
		fragmentDefaultShader->compile();

		inited = true;
	}
}


void BasicTransparencyAlgorithm::installShaders(ShaderProgram* program)
{
	program->attachShader(vertexDefaultShader);
	program->attachShader(fragmentDefaultShader);
}


void BasicTransparencyAlgorithm::uninstallShaders(ShaderProgram* program)
{
	program->detachShader(vertexDefaultShader);
	program->detachShader(fragmentDefaultShader);
}
