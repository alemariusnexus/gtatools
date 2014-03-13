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

#include "ScriptVector4.h"




void ScriptVector4::registerClass(lua_State* lua)
{
	createClass(lua);


	luaS_createmethod(lua, "_init", [] (lua_State* lua) {
		Vector4 vec = Vector4::Zero;

		size_t argc = lua_gettop(lua)-1;

		if (argc == 0) {
		} else if (argc == 1) {
			if (!luaS_instanceof(lua, CLASS_NAME, 2))
				return luaL_error(lua, "Invalid argument for Vector4(vec) constructor!");
			auto osvec = getInstance(lua, 2);
			vec = osvec->vec;
		} else if (argc == 3) {
			vec = Vector4(lua_tonumber(lua, 2), lua_tonumber(lua, 3), lua_tonumber(lua, 4), 1.0);
		} else if (argc == 4) {
			vec = Vector4(lua_tonumber(lua, 2), lua_tonumber(lua, 3), lua_tonumber(lua, 4), lua_tonumber(lua, 5));
		}

		ScriptVector4* svec = new ScriptVector4(lua, vec);
		initInstance(lua, svec, 1);
		return 0;
	});

	luaS_createmethod(lua, "__add", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		auto osvec = getInstance(lua, 2);
		create(lua, svec->vec + osvec->vec);
		return 1;
	});

	luaS_createmethod(lua, "__sub", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		auto osvec = getInstance(lua, 2);
		create(lua, svec->vec - osvec->vec);
		return 1;
	});

	luaS_createmethod(lua, "__mul", [] (lua_State* lua) {
		shared_ptr<ScriptVector4> svec;
		int otherIdx;

		if (lua_istable(lua, 1)) {
			svec = getSelf(lua);
			otherIdx = 2;
		} else {
			svec = getInstance(lua, 2);
			otherIdx = 1;
		}

		if (lua_isnumber(lua, otherIdx)) {
			create(lua, svec->vec * lua_tonumber(lua, otherIdx));
			return 1;
		} else if (luaS_instanceof(lua, CLASS_NAME, otherIdx)) {
			auto osvec = getInstance(lua, otherIdx);
			lua_pushnumber(lua, svec->vec.dot(osvec->vec));
			return 1;
		} else {
			return luaL_argerror(lua, otherIdx, "Argument must be a Vector4 or a number!");
		}
	});

	luaS_createmethod(lua, "__tostring", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		char buf[256];
		sprintf(buf, "%f, %f, %f, %f", svec->vec.getX(), svec->vec.getY(), svec->vec.getZ(), svec->vec.getW());
		lua_pushstring(lua, buf);
		return 1;
	});

	luaS_createmethod(lua, "get", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		lua_pushnumber(lua, svec->vec.getX());
		lua_pushnumber(lua, svec->vec.getY());
		lua_pushnumber(lua, svec->vec.getZ());
		lua_pushnumber(lua, svec->vec.getW());
		return 4;
	});

	luaS_createmethod(lua, "getX", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		lua_pushnumber(lua, svec->vec.getX());
		return 1;
	});

	luaS_createmethod(lua, "getY", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		lua_pushnumber(lua, svec->vec.getX());
		return 1;
	});

	luaS_createmethod(lua, "getZ", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		lua_pushnumber(lua, svec->vec.getX());
		return 1;
	});

	luaS_createmethod(lua, "getW", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		lua_pushnumber(lua, svec->vec.getW());
		return 1;
	});

	luaS_createmethod(lua, "set", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		float x = luaL_checknumber(lua, 2);
		float y = luaL_checknumber(lua, 3);
		float z = luaL_checknumber(lua, 4);
		float w = 1.0f;

		if (lua_gettop(lua) > 4)
			w = luaL_checknumber(lua, 5);

		svec->vec = Vector4(x, y, z, w);
		return 0;
	});

	luaS_createmethod(lua, "dot", [] (lua_State* lua) {
		auto svec = getSelf(lua);
		auto osvec = checkInstanceof(lua, 2);
		lua_pushnumber(lua, svec->vec.dot(osvec->vec));
		return 1;
	});

	/*luaS_createmethod(lua, "length", [] (lua_State* lua) {
		ScriptVector4* svec = luaS_getselfinstance<ScriptVector4>(lua);
		lua_pushnumber(lua, svec->vec.length());
		return 1;
	});

	luaS_createmethod(lua, "normalize", [] (lua_State* lua) {
		ScriptVector4* svec = luaS_getselfinstance<ScriptVector4>(lua);
		svec->vec.normalize();
		return 0;
	});

	luaS_createmethod(lua, "normalized", [] (lua_State* lua) {
		ScriptVector4* svec = luaS_getselfinstance<ScriptVector4>(lua);
		Vector4 vec = svec->vec;
		vec.normalize();
		create(lua, vec);
		return 1;
	});*/
}


shared_ptr<ScriptVector4> ScriptVector4::create(lua_State* lua, const Vector4& vec)
{
	lua_getglobal(lua, CLASS_NAME);
	ScriptVector4* inst = new ScriptVector4(lua, vec);
	return createInstance(lua, inst);
}
