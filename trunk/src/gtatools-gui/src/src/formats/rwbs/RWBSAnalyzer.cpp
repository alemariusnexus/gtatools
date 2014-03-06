#include "RWBSAnalyzer.h"
#include "script/RWBSScriptByteReader.h"
#include "script/ScriptRWSection.h"
#include <gta/script/math/luamath.h>




RWBSAnalyzer* RWBSAnalyzer::load(const File& scriptFile)
{
	lua_State* lua = luaL_newstate();

	luaL_openlibs(lua);
	luaS_opencorelibs(lua);

	if (luaL_dofile(lua, scriptFile.getPath()->toString().get()) != LUA_OK) {
		const char* errmsg = lua_tostring(lua, -1);
		fprintf(stderr, "Error loading Lua script: %s\n", errmsg);
		lua_close(lua);
		return NULL;
	}

	ScriptRWSection::registerClass(lua);
	RWBSScriptByteReader::registerClass(lua);
	OpenLuaMathLibrary(lua);

	lua_getglobal(lua, "GetAnalyzerInfo");
	if (lua_pcall(lua, 0, 1, 0) != LUA_OK) {
		const char* errmsg = lua_tostring(lua, -1);
		fprintf(stderr, "Error calling GetAnalyzerInfo(): %s\n", errmsg);
		lua_close(lua);
		return NULL;
	}

	QString name(lua_tostring(lua, -1));

	return new RWBSAnalyzer(lua, name);
}


RWBSAnalyzer::RWBSAnalyzer(lua_State* lua, const QString& name)
		: lua(lua), name(name), resultsValid(false)
{
}


void RWBSAnalyzer::notifySectionChanged(RWSection* sect)
{
	lua_getglobal(lua, "SectionChanged");
	ScriptRWSection::create(lua, sect);
	lua_pcall(lua, 1, 1, 0);

	if (resultsValid  &&  lua_toboolean(lua, -1)) {
		resultsValid = false;
	}

	lua_pop(lua, 1);
}


void RWBSAnalyzer::notifyContentsChanged(QList<RWSection*> rootSects)
{
	lua_getglobal(lua, "ContentsChanged");
	for (int i = 0 ; i < rootSects.size() ; i++) {
		ScriptRWSection::create(lua, rootSects[i]);
	}
	lua_pcall(lua, rootSects.size(), 1, 0);

	if (resultsValid  &&  lua_toboolean(lua, -1)) {
		resultsValid = false;
	}

	lua_pop(lua, 1);
}


void RWBSAnalyzer::notifyCurrentSectionChanged(RWSection* sect)
{
	lua_getglobal(lua, "CurrentSectionChanged");

	if (sect)
		ScriptRWSection::create(lua, sect);
	else
		lua_pushnil(lua);

	if (lua_pcall(lua, 1, 1, 0) != LUA_OK) {
		const char* errmsg = lua_tostring(lua, -1);
		fprintf(stderr, "Error calling CurrentSectionChanged(): %s\n", errmsg);
	}

	if (resultsValid  &&  lua_toboolean(lua, -1)) {
		resultsValid = false;
	}

	lua_pop(lua, 1);
}


QString RWBSAnalyzer::getAnalysisResults()
{
	if (resultsValid)
		return lastResults;

	lua_getglobal(lua, "GetResults");
	lua_pcall(lua, 0, 1, 0);
	QString cont(lua_tostring(lua, -1));
	lastResults = cont;
	resultsValid = true;
	return cont;
}
