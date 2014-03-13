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

#ifndef LUASYS_H_
#define LUASYS_H_

extern "C" {

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

}

#include <nxcommon/CString.h>
#include "_luasys_helpers.h"



void luaS_opencorelibs(lua_State* lua);
void luaS_newclass(lua_State* lua, bool inherit = false);
void luaS_newinstance(lua_State* lua);
void luaS_newinstance(lua_State* lua, const CString& className);
void luaS_createclass(lua_State* lua, const CString& name, bool inherit = false);
void luaS_createclass(lua_State* lua, const CString& name, const CString& baseName);
void luaS_createmethod(lua_State* lua, const CString& name, lua_CFunction cfunc);
void luaS_createmethod(lua_State* lua, const CString& className, const CString& methodName, lua_CFunction cfunc);
void luaS_setfield(lua_State* lua, const CString& name);
void luaS_setfield(lua_State* lua, const CString& name, int instIdx);
void luaS_getfield(lua_State* lua, const CString& name);
void luaS_getfield(lua_State* lua, const CString& name, int instIdx);
void luaS_createfunc(lua_State* lua, const CString& name, lua_CFunction cfunc);
int luaS_callmethod(lua_State* lua, int nargs, int nresults);
int luaS_fastcallmethod(lua_State* lua, int nargs, int nresults);
bool luaS_instanceof(lua_State* lua);
bool luaS_instanceof(lua_State* lua, const CString& clsName);
bool luaS_instanceof(lua_State* lua, const CString& clsName, int instIdx);
lua_Integer luaS_checkintrange(lua_State* lua, int arg, lua_Integer min, lua_Integer max);
lua_Unsigned luaS_checkuintrange(lua_State* lua, int arg, lua_Unsigned min, lua_Unsigned max);
bool luaS_checkinstanceof(lua_State* lua, const CString& clsName, int instArgIdx);


#endif /* LUASYS_H_ */
