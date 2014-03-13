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

#ifndef SCRIPTMATRIX4_H_
#define SCRIPTMATRIX4_H_

#include "../ScriptObject.h"
#include <nxcommon/math/Matrix4.h>



class ScriptMatrix4 : public ScriptObject
{
	SCRIPTOBJECT_CREATE_HELPER_METHODS(ScriptMatrix4, "Matrix4")

public:
	static void registerClass(lua_State* lua);
	static shared_ptr<ScriptMatrix4> create(lua_State* lua, const Matrix4& mat = Matrix4::Identity);

private:
	ScriptMatrix4(lua_State* lua, const Matrix4& vec) : ScriptObject(lua), mat(vec) {}
	ScriptMatrix4(lua_State* lua) : ScriptObject(lua), mat(Matrix4::Identity) {}

private:
	Matrix4 mat;
};

#endif /* SCRIPTMATRIX4_H_ */
