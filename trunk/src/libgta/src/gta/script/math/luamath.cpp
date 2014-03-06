#include "luamath.h"
#include "ScriptVector3.h"
#include "ScriptVector4.h"
#include "ScriptMatrix4.h"



void OpenLuaMathLibrary(lua_State* lua)
{
	ScriptVector3::registerClass(lua);
	ScriptVector4::registerClass(lua);
	ScriptMatrix4::registerClass(lua);
}
