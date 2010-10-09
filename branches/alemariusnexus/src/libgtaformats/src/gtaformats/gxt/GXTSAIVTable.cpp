/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#include "GXTSAIVTable.h"
#include "../gta.h"


GXTSAIVTable::GXTSAIVTable(Encoding internalEncoding)
		: GXTTable(internalEncoding)
{
}


GXTSAIVTable::~GXTSAIVTable()
{
	EntryMap::iterator it;

	for (it = entries.begin() ; it != entries.end() ; it++) {
		delete[] it->second;
	}
}


char* GXTSAIVTable::getValue(int32_t keyHash)
{
	EntryMap::iterator it = entries.find(keyHash);

	if (it == entries.end()) {
		return NULL;
	}

	return it->second;
}


void GXTSAIVTable::setValue(int32_t keyHash, char* value)
{
	entries[keyHash] = value;
}


void GXTSAIVTable::setValue(const char* key, char* value)
{
	entries[Crc32(key)] = value;
}
