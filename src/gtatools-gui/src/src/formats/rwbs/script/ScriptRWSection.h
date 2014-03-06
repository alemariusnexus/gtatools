#ifndef SCRIPTRWSECTION_H_
#define SCRIPTRWSECTION_H_

#include <gta/script/ScriptObject.h>
#include <gtaformats/rwbs/RWSection.h>



class ScriptRWSection : public ScriptObject
{
	SCRIPTOBJECT_CREATE_HELPER_METHODS(ScriptRWSection, "RWSection")

public:
	virtual ~ScriptRWSection();
	static void registerClass(lua_State* lua);
	static shared_ptr<ScriptRWSection> create(lua_State* lua, RWSection* sect, bool takeOwnership = false);

private:
	ScriptRWSection(lua_State* lua, RWSection* sect, bool takeOwnership = false);

private:
	RWSection* sect;
	bool haveOwnership;
};

#endif /* SCRIPTRWSECTION_H_ */
