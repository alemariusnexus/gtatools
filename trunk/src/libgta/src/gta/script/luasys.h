#ifndef LUASYS_H_
#define LUASYS_H_

extern "C" {

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

}

#include <gtaformats/util/CString.h>
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
