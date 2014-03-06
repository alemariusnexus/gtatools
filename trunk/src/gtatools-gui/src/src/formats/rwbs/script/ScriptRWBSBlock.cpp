#include "ScriptRWBSBlock.h"

using boost::dynamic_pointer_cast;




void ScriptRWBSBlock::registerClass(lua_State* lua)
{
	createClass(lua);
}


shared_ptr<ScriptRWBSBlock> ScriptRWBSBlock::create(lua_State* lua)
{
	lua_getglobal(lua, CLASS_NAME);
	ScriptRWBSBlock* inst = new ScriptRWBSBlock(lua);
	return createInstance(lua, inst);
}




ScriptRWBSBlock::ScriptRWBSBlock(lua_State* lua)
		: ScriptObject(lua)
{
}


ScriptRWBSBlock::~ScriptRWBSBlock()
{
}

