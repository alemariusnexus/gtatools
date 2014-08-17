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

#include "ScriptRWBSBlock.h"
#include <memory>

using std::dynamic_pointer_cast;




SCRIPTOBJECT_DEFINE_HELPER_METHODS(ScriptRWBSBlock, "RWBlock")




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

