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

#ifndef GXTPLAINKEYTABLE_H_
#define GXTPLAINKEYTABLE_H_

#include "../gf_config.h"
#include "GXTTable.h"
#include <map>

using std::map;


class GXTPlainKeyTable : public GXTTable
{
public:
	typedef map<int32_t, const char*> KeyMap;

public:
	GXTPlainKeyTable(EntryMap* entries, KeyMap* keys);
	const char* getKey(int32_t hash);

private:
	KeyMap* keys;
};

#endif /* GXTPLAINKEYTABLE_H_ */
