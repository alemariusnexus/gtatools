/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "ScriptRWSection.h"
#include "RWBSScriptByteReader.h"





void ScriptRWSection::registerClass(lua_State* lua)
{
	createClass(lua);


	//luaS_createmethodgetnum<RWSection, int32_t, &RWSection::getID>(lua, "getID");
	//luaS_createmethodgetnum<RWSection, int32_t, &RWSection::getSize>(lua, "getSize");
	//luaS_createmethodgetnum<RWSection, int32_t, &RWSection::getVersion>(lua, "getVersion");

	luaS_createmethod(lua, "getID", [] (lua_State* lua) {
		auto sect = getSelf(lua);
		lua_pushnumber(lua, sect->sect->getID());
		return 1;
	});

	luaS_createmethod(lua, "getSize", [] (lua_State* lua) {
		auto sect = getSelf(lua);
		lua_pushnumber(lua, sect->sect->getSize());
		return 1;
	});

	luaS_createmethod(lua, "getVersion", [] (lua_State* lua) {
		auto sect = getSelf(lua);
		lua_pushnumber(lua, sect->sect->getVersion());
		return 1;
	});

	luaS_createmethod(lua, "getName", [] (lua_State* lua) {
		auto sect = getSelf(lua);
		char buf[64];
		RWGetSectionShortName(sect->sect->getID(), buf);
		lua_pushstring(lua, buf);
		return 1;
	});

	luaS_createmethod(lua, "getFullName", [] (lua_State* lua) {
		auto sect = getSelf(lua);
		char buf[64];
		RWGetSectionName(sect->sect->getID(), buf);
		lua_pushstring(lua, buf);
		return 1;
	});

	luaS_createmethod(lua, "getDataReader", [] (lua_State* lua) {
		auto sect = getSelf(lua);
		uint8_t* dataCpy = new uint8_t[sect->sect->getSize()];
		memcpy(dataCpy, sect->sect->getData(), sect->sect->getSize());
		RWBSScriptByteReader::create(lua, dataCpy, sect->sect->getSize());
		return 1;
	});

	luaS_createmethod(lua, "clone", [] (lua_State* lua) {
		auto sect = getSelf(lua);
		RWSection* csect = new RWSection(*sect->sect);
		create(lua, csect, true);
		return 1;
	});

	luaS_createmethod(lua, "getChildren", [] (lua_State* lua) {
		auto sect = getSelf(lua);

		lua_newtable(lua);

		int i = 0;
		for (RWSection::ChildIterator it = sect->sect->getChildBegin() ; it != sect->sect->getChildEnd() ; it++, i++) {
			RWSection* csect = *it;
			create(lua, csect, false);
			lua_rawseti(lua, -2, i+1);
		}

		return 1;
	});

	luaS_createmethod(lua, "getParent", [] (lua_State* lua) {
		auto sect = getSelf(lua);

		if (sect->sect->getParent())
			create(lua, sect->sect->getParent(), false);
		else
			lua_pushnil(lua);

		return 1;
	});
}


shared_ptr<ScriptRWSection> ScriptRWSection::create(lua_State* lua, RWSection* sect, bool takeOwnership)
{
	lua_getglobal(lua, CLASS_NAME);
	ScriptRWSection* inst = new ScriptRWSection(lua, sect, takeOwnership);
	return createInstance(lua, inst);
}




ScriptRWSection::ScriptRWSection(lua_State* lua, RWSection* sect, bool takeOwnership)
		: ScriptObject(lua), sect(sect), haveOwnership(takeOwnership)
{
}


ScriptRWSection::~ScriptRWSection()
{
	if (haveOwnership)
		delete sect;
}

