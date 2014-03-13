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

#ifndef SCRIPTVECTOR4_H_
#define SCRIPTVECTOR4_H_

#include "../ScriptObject.h"
#include <nxcommon/math/Vector4.h>



class ScriptVector4 : public ScriptObject
{
	SCRIPTOBJECT_CREATE_HELPER_METHODS(ScriptVector4, "Vector4")

public:
	static void registerClass(lua_State* lua);
	static shared_ptr<ScriptVector4> create(lua_State* lua, const Vector4& vec = Vector4::Zero);
	Vector4 getVector() const { return vec; }

private:
	ScriptVector4(lua_State* lua, const Vector4& vec) : ScriptObject(lua), vec(vec) {}
	ScriptVector4(lua_State* lua) : ScriptObject(lua), vec(Vector4::Zero) {}

private:
	Vector4 vec;
};

#endif /* SCRIPTVECTOR4_H_ */
