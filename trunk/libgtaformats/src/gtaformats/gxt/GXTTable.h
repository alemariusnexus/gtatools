/*
	Copyright 2010 David Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GXTTABLE_H_
#define GXTTABLE_H_

#include "../gf_config.h"
#include <map>
#include "../gta.h"

using std::map;


class GXTTable
{
public:
	typedef wchar_t value_t;
	typedef map<int32_t, const value_t*> EntryMap;

public:
	GXTTable(EntryMap* entries);
	~GXTTable();
	const value_t* getValue(int32_t hash);
	const value_t* getValue(const char* key) { return getValue(Crc32(key)); };
	const value_t* operator[](const char* key) { return getValue(key); };
	const value_t* operator[](int32_t hash) { return getValue(hash); };
	EntryMap::iterator getFirstEntry() { return entries->begin(); }
	EntryMap::iterator getLastEntry() { return entries->end(); }

private:
	EntryMap* entries;
};

#endif /* GXTTABLE_H_ */
