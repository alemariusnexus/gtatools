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

#ifndef SCRIPTRWSECTION_H_
#define SCRIPTRWSECTION_H_

#include <nxcommon/script/ScriptObject.h>
#include <gtaformats/rwbs/RWSection.h>



class ScriptRWSection : public ScriptObject
{
	SCRIPTOBJECT_DECLARE_HELPER_METHODS(ScriptRWSection)

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
