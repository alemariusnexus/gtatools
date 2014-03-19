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

#include "luasys.h"
#include <res_scriptsys_script.h>



void luaS_opencorelibs(lua_State* lua)
{
	luaL_loadbuffer(lua, (const char*) res_scriptsys_script_data, sizeof(res_scriptsys_script_data), "scriptsys.lua");
	lua_pcall(lua, 0, 0, 0);
}


void luaS_newclass(lua_State* lua, bool inherit)
{
	lua_getglobal(lua, "CreateClass");

	if (inherit)
		lua_pushvalue(lua, -2);

	lua_pcall(lua, inherit ? 1 : 0, 1, 0);
}


void luaS_newinstance(lua_State* lua)
{
	lua_newtable(lua);
	lua_pushvalue(lua, -2);
	lua_setmetatable(lua, -2);
	lua_remove(lua, -2);
}


void luaS_newinstance(lua_State* lua, const CString& className)
{
	lua_getglobal(lua, className.get());
	luaS_newinstance(lua);
}


void luaS_createclass(lua_State* lua, const CString& name, bool inherit)
{
	luaS_newclass(lua, inherit);
	lua_setglobal(lua, name.get());
}


void luaS_createclass(lua_State* lua, const CString& name, const CString& baseName)
{
	if (!baseName.isNull()) {
		lua_getglobal(lua, baseName.get());
		luaS_createclass(lua, name, true);
	} else {
		luaS_createclass(lua, name, false);
	}
}


void luaS_createmethod(lua_State* lua, const CString& name, lua_CFunction cfunc)
{
	lua_pushstring(lua, name.get());
	lua_pushcfunction(lua, cfunc);
	lua_settable(lua, -3);
}


void luaS_createmethod(lua_State* lua, const CString& className, const CString& methodName, lua_CFunction cfunc)
{
	lua_getglobal(lua, className.get());
	luaS_createmethod(lua, methodName, cfunc);
}


void luaS_setfield(lua_State* lua, const CString& name)
{
	luaS_setfield(lua, name, -2);
}


void luaS_setfield(lua_State* lua, const CString& name, int instIdx)
{
	int absIdx = lua_absindex(lua, instIdx);
	lua_pushstring(lua, name.get());
	lua_pushvalue(lua, -2);
	lua_settable(lua, absIdx);
	lua_pop(lua, 1);
}


void luaS_getfield(lua_State* lua, const CString& name)
{
	lua_pushstring(lua, name.get());
	lua_gettable(lua, -2);
}


void luaS_getfield(lua_State* lua, const CString& name, int instIdx)
{
	int absIdx = lua_absindex(lua, instIdx);
	lua_pushstring(lua, name.get());
	lua_gettable(lua, absIdx);
}


void luaS_createfunc(lua_State* lua, const CString& name, lua_CFunction cfunc)
{
	lua_pushcfunction(lua, cfunc);
	lua_setglobal(lua, name.get());
}


int luaS_callmethod(lua_State* lua, int nargs, int nresults)
{
	// instance (kept)
	// method name (popped)
	// arguments (popped)

	lua_pushvalue(lua, -1-nargs);
	lua_gettable(lua, -3-nargs);
	lua_replace(lua, -2-nargs);
	lua_pushvalue(lua, -2-nargs);
	lua_insert(lua, -1-nargs);
	return lua_pcall(lua, nargs+1, nresults, 0);


}


int luaS_fastcallmethod(lua_State* lua, int nargs, int nresults)
{
	// method name (popped)
	// instance (popped)
	// arguments (popped)

	lua_pushvalue(lua, -2-nargs);
	lua_gettable(lua, -2-nargs);
	lua_replace(lua, -3-nargs);
	return lua_pcall(lua, nargs+1, nresults, 0);
}


bool luaS_instanceof(lua_State* lua)
{
	// instance (kept)
	// class (popped)

	if (!lua_istable(lua, -2)  ||  !lua_istable(lua, -1)) {
		lua_pop(lua, 1);
		return false;
	}

	lua_pushstring(lua, "instanceof");
	lua_insert(lua, -2);
	luaS_callmethod(lua, 1, 1);
	bool res = lua_toboolean(lua, -1);
	lua_pop(lua, 1);
	return res;
}


bool luaS_instanceof(lua_State* lua, const CString& clsName)
{
	lua_getglobal(lua, clsName.get());
	return luaS_instanceof(lua);
}


bool luaS_instanceof(lua_State* lua, const CString& clsName, int instIdx)
{
	lua_pushvalue(lua, instIdx);
	bool res = luaS_instanceof(lua, clsName);
	lua_pop(lua, 1);
	return res;
}


lua_Integer luaS_checkintrange(lua_State* lua, int arg, lua_Integer min, lua_Integer max)
{
	lua_Integer val = luaL_checkinteger(lua, arg);

	if (val < min  ||  val > max) {
		char buf[128];
		sprintf(buf, "Argument out of range! Should be [%d, %d].", min, max);
		return luaL_argerror(lua, arg, buf);
	}

	return val;
}


lua_Unsigned luaS_checkuintrange(lua_State* lua, int arg, lua_Unsigned min, lua_Unsigned max)
{
	lua_Unsigned val = luaL_checkunsigned(lua, arg);

	if (val < min  ||  val > max) {
		char buf[128];
		sprintf(buf, "Argument out of range! Should be [%u, %u].", min, max);
		return luaL_argerror(lua, arg, buf);
	}

	return val;
}


bool luaS_checkinstanceof(lua_State* lua, const CString& clsName, int instArgIdx)
{
	if (!luaS_instanceof(lua, clsName, instArgIdx)) {
		char* msg = new char[64 + clsName.length()];
		sprintf(msg, "Argument must be a %s!", clsName.get());
		CString cmsg = CString::from(msg);
		luaL_argerror(lua, instArgIdx, cmsg.get());
		return false;
	} else {
		return true;
	}
}
