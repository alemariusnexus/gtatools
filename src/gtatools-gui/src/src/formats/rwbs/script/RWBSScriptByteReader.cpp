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

#include "RWBSScriptByteReader.h"
#include <nxcommon/script/math/ScriptVector3.h>
#include <nxcommon/script/math/ScriptVector4.h>
#include <nxcommon/script/math/ScriptMatrix4.h>
#include <memory>

using std::dynamic_pointer_cast;




SCRIPTOBJECT_DEFINE_HELPER_METHODS(RWBSScriptByteReader, "ByteReader")




template <typename T>
int _readArrayNumImpl(lua_State* lua)
{
	auto reader = RWBSScriptByteReader::getSelf(lua);
	lua_Unsigned num = luaL_checkunsigned(lua, 2);
	reader->readArray<T>(num);
	return 1;
}


template <typename T>
int _readMultiNumImpl(lua_State* lua)
{
	auto reader = RWBSScriptByteReader::getSelf(lua);
	size_t num = 1;
	if (lua_gettop(lua) > 1) {
		num = luaL_checkunsigned(lua, 2);
	}
	reader->readMulti<T>(num);
	return num;
}


template <class ScriptC, class StructC>
int _readMultiMathStructImpl (lua_State* lua)
{
	auto reader = RWBSScriptByteReader::getSelf(lua);
	size_t num = 1;

	if (lua_gettop(lua) > 1) {
		num = luaL_checkunsigned(lua, 2);
	}

	StructC s;
	for (size_t i = 0 ; i < num ; i++) {
		reader->read((uint8_t*) &s, sizeof(StructC));
		ScriptC::create(lua, s);
	}

	return (int) num;
}


template <class ScriptC, class StructC>
int _readArrayMathStructImpl(lua_State* lua)
{
	auto reader = RWBSScriptByteReader::getSelf(lua);
	lua_Unsigned num = luaL_checkunsigned(lua, 2);

	lua_newtable(lua);

	StructC s;
	for (size_t i = 0 ; i < num ; i++) {
		reader->read((uint8_t*) &s, sizeof(StructC));
		ScriptC::create(lua, s);
		lua_rawseti(lua, -2, i+1);
	}

	return 1;
}




void RWBSScriptByteReader::registerClass(lua_State* lua)
{
	createClass(lua);

	luaS_createmethod(lua, "readUInt64", &_readMultiNumImpl<uint64_t>);
	luaS_createmethod(lua, "readInt64", &_readMultiNumImpl<int64_t>);

	luaS_createmethod(lua, "readUInt32", &_readMultiNumImpl<uint32_t>);
	luaS_createmethod(lua, "readInt32", &_readMultiNumImpl<int32_t>);

	luaS_createmethod(lua, "readUInt16", &_readMultiNumImpl<uint16_t>);
	luaS_createmethod(lua, "readInt16", &_readMultiNumImpl<int16_t>);

	luaS_createmethod(lua, "readUInt8", &_readMultiNumImpl<uint8_t>);
	luaS_createmethod(lua, "readInt8", &_readMultiNumImpl<int8_t>);

	luaS_createmethod(lua, "readFloat", &_readMultiNumImpl<float>);
	luaS_createmethod(lua, "readDouble", &_readMultiNumImpl<double>);

	luaS_createmethod(lua, "readArrayUInt64", &_readArrayNumImpl<uint64_t>);
	luaS_createmethod(lua, "readArrayInt64", &_readArrayNumImpl<int64_t>);

	luaS_createmethod(lua, "readArrayUInt32", &_readArrayNumImpl<uint32_t>);
	luaS_createmethod(lua, "readArrayInt32", &_readArrayNumImpl<int32_t>);

	luaS_createmethod(lua, "readArrayUInt16", &_readArrayNumImpl<uint16_t>);
	luaS_createmethod(lua, "readArrayInt16", &_readArrayNumImpl<int16_t>);

	luaS_createmethod(lua, "readArrayUInt8", &_readArrayNumImpl<uint8_t>);
	luaS_createmethod(lua, "readArrayInt8", &_readArrayNumImpl<int8_t>);

	luaS_createmethod(lua, "readMultiUInt64", &_readMultiNumImpl<uint64_t>);

	luaS_createmethod(lua, "readString", [] (lua_State* lua) {
		auto reader = getSelf(lua);
		if (lua_gettop(lua) >= 2  &&  !lua_isnil(lua, 2)) {
			size_t len = lua_tonumber(lua, 2);
			CString str = reader->readString(len);
			lua_pushstring(lua, str.get());
			return 1;
		} else {
			return luaL_error(lua, "Invalid number of parameters for ByteReader:readString()");
		}
	});

	luaS_createmethod(lua, "readVector3", &_readMultiMathStructImpl<ScriptVector3, Vector3>);
	luaS_createmethod(lua, "readArrayVector3", &_readArrayMathStructImpl<ScriptVector3, Vector3>);

	luaS_createmethod(lua, "readVector4", &_readMultiMathStructImpl<ScriptVector4, Vector4>);
	luaS_createmethod(lua, "readArrayVector4", &_readArrayMathStructImpl<ScriptVector4, Vector4>);

	luaS_createmethod(lua, "readMatrix4", &_readMultiMathStructImpl<ScriptMatrix4, Matrix4>);
	luaS_createmethod(lua, "readArrayMatrix4", &_readArrayMathStructImpl<ScriptMatrix4, Matrix4>);

	luaS_createmethod(lua, "skip", [] (lua_State* lua) {
		auto reader = getSelf(lua);
		if (!lua_isnumber(lua, 2)) return luaL_error(lua, "Invalid number of arguments for ByteReader:skip()");
		lua_pushnumber(lua, reader->skip(lua_tonumber(lua, 2)));
		return 1;
	});

	luaS_createmethod(lua, "setPosition", [] (lua_State* lua) {
		auto reader = getSelf(lua);
		lua_Unsigned pos = luaS_checkuintrange(lua, 2, 1, reader->getBytesTotal());
		reader->data = reader->dataBase + pos - 1;
		return 0;
	});

	luaS_createmethod(lua, "getPosition", [] (lua_State* lua) {
		auto reader = getSelf(lua);
		lua_pushnumber(lua, reader->data - reader->dataBase + 1);
		return 1;
	});

	luaS_createmethod(lua, "getBytesTotal", [] (lua_State* lua) {
		auto reader = getSelf(lua);
		lua_pushnumber(lua, reader->getBytesTotal());
		return 1;
	});

	luaS_createmethod(lua, "getBytesLeft", [] (lua_State* lua) {
		auto reader = getSelf(lua);
		lua_pushnumber(lua, reader->getBytesLeft());
		return 1;
	});
}


shared_ptr<RWBSScriptByteReader> RWBSScriptByteReader::create(lua_State* lua, uint8_t* data, size_t size)
{
	lua_getglobal(lua, CLASS_NAME);
	RWBSScriptByteReader* inst = new RWBSScriptByteReader(lua, data, size);
	return createInstance(lua, inst);
}




RWBSScriptByteReader::RWBSScriptByteReader(lua_State* lua, uint8_t* data, size_t size)
		: ScriptObject(lua), dataBase(data), dataEnd(dataBase+size), data(dataBase)
{
}


RWBSScriptByteReader::~RWBSScriptByteReader()
{
	delete[] dataBase;
}


CString RWBSScriptByteReader::readString(size_t len)
{
	char* buf = new char[len+1];
	buf[len] = '\0';

	size_t numRead = read((uint8_t*) buf, len);

	if (numRead < len) {
		data -= numRead;
		delete[] buf;
		return CString();
	} else {
		return CString::from(buf);
	}
}

