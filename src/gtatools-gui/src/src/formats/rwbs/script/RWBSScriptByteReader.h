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

#ifndef RWBSSCRIPTBYTEREADER_H_
#define RWBSSCRIPTBYTEREADER_H_

#include <nxcommon/script/ScriptObject.h>
#include <nxcommon/script/luasys.h>
#include <algorithm>




class RWBSScriptByteReader : public ScriptObject
{
	SCRIPTOBJECT_DECLARE_HELPER_METHODS(RWBSScriptByteReader)

public:
	static void registerClass(lua_State* lua);
	static shared_ptr<RWBSScriptByteReader> create(lua_State* lua, uint8_t* data, size_t size);

public:
	virtual ~RWBSScriptByteReader();

	template <typename T>
	T read() { T v = *((T*) data); data += sizeof(T); return v; }

	size_t read(uint8_t* buf, size_t len) { len = std::min(len, getBytesLeft()); memcpy(buf, data, len); data += len; return len; }
	CString readString(size_t len);

	size_t getBytesTotal() const { return dataEnd-dataBase; }
	size_t getBytesLeft() const { return dataEnd-data; }

	size_t skip(size_t len) { len = std::min(len, getBytesLeft()); data += len; return len; }

	template <typename T>
	void readArray(size_t numElements);

	template <typename T>
	void readMulti(size_t numElements);

private:
	RWBSScriptByteReader(lua_State* lua, uint8_t* data, size_t size);

private:
	uint8_t* dataBase;
	uint8_t* dataEnd;;
	uint8_t* data;
};






template <typename T>
void RWBSScriptByteReader::readArray(size_t numElements)
{
	if (getBytesLeft() < numElements*sizeof(T)) {
		lua_pushnil(lua);
	} else {
		lua_newtable(lua);

		T* dptr = (T*) data;
		for (size_t i = 0 ; i < numElements ; i++) {
			lua_pushnumber(lua, dptr[i]);
			lua_rawseti(lua, -2, i+1);
		}

		data += numElements*sizeof(T);
	}
}


template <typename T>
void RWBSScriptByteReader::readMulti(size_t numElements)
{
	size_t readSize = std::min(numElements, getBytesLeft()/sizeof(T));
	T* dptr = (T*) data;
	T* dend = dptr+readSize;

	for (; dptr != dend ; dptr++) {
		lua_pushnumber(lua, *dptr);
	}

	for (size_t i = readSize ; i < numElements ; i++) {
		lua_pushnil(lua);
	}

	data += readSize*sizeof(T);
}

#endif /* RWBSSCRIPTBYTEREADER_H_ */
