#include "ScriptObject.h"
#include <gtaformats/util/InvalidStateException.h>





ScriptObject::ScriptObject(lua_State* lua)
		: lua(lua)
{
}


ScriptObject::~ScriptObject()
{
}

