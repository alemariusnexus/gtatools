#ifndef SCRIPTRWBSBLOCK_H_
#define SCRIPTRWBSBLOCK_H_

#include <gta/script/ScriptObject.h>
#include <gta/script/luasys.h>
#include <algorithm>




class ScriptRWBSBlock : public ScriptObject
{
	SCRIPTOBJECT_CREATE_HELPER_METHODS(ScriptRWBSBlock, "RWBlock")

public:
	static void registerClass(lua_State* lua);
	static shared_ptr<ScriptRWBSBlock> create(lua_State* lua);

public:
	virtual ~ScriptRWBSBlock();

private:
	ScriptRWBSBlock(lua_State* lua);
};





#endif /* SCRIPTRWBSBLOCK_H_ */
