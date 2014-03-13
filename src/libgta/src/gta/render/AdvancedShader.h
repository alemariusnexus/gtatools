/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef ADVANCEDSHADER_H_
#define ADVANCEDSHADER_H_

#include <gta/config.h>
#include <nxcommon/CString.h>
#include "../Shader.h"
#include "../gl.h"


class AdvancedShader
{
public:
	AdvancedShader(GLenum type, const CString& name = CString());
	void setSourceCode(const CString& code) { this->code = code; }

private:
	void compile(const CString& headerCode);

private:
	Shader* shader;
	CString code;

	friend class AdvancedShaderProgram;
};

#endif /* ADVANCEDSHADER_H_ */
