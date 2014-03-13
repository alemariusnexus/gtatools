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

#ifndef SCRIPTVECTOR3_H_
#define SCRIPTVECTOR3_H_

#include "../luasys.h"
#include "../ScriptObject.h"
#include <nxcommon/math/Vector3.h>



class ScriptVector3 : public ScriptObject
{
	SCRIPTOBJECT_CREATE_HELPER_METHODS(ScriptVector3, "Vector3")

public:
	static void registerClass(lua_State* lua);
	static shared_ptr<ScriptVector3> create(lua_State* lua, const Vector3& vec = Vector3::Zero);
	Vector3 getVector() const { return vec; }

private:
	ScriptVector3(lua_State* lua, const Vector3& vec) : ScriptObject(lua), vec(vec) {}
	ScriptVector3(lua_State* lua) : ScriptObject(lua), vec(Vector3::Zero) {}

private:
	Vector3 vec;
};

#endif /* SCRIPTVECTOR3_H_ */
