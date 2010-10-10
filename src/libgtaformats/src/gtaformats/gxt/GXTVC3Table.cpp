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

#include "GXTVC3Table.h"
#include "../gta.h"



GXTVC3Table::GXTVC3Table(Encoding internalEncoding, bool keyNames)
		: GXTTable(internalEncoding), keyNames(keyNames ? new KeyNameMap : NULL)
{
}


GXTVC3Table::~GXTVC3Table()
{
	if (keyNames) {
		KeyNameMap::iterator it;

		for (it = keyNames->begin() ; it != keyNames->end() ; it++) {
			delete[] it->second;
		}

		delete keyNames;
	}

	EntryMap::iterator it;

	for (it = entries.begin() ; it != entries.end() ; it++) {
		delete[] it->second;
	}
}


char* GXTVC3Table::getValue(int32_t keyHash)
{
	EntryMap::iterator it = entries.find(keyHash);

	if (it == entries.end()) {
		return NULL;
	}

	return it->second;
}


void GXTVC3Table::setValue(int32_t keyHash, char* value)
{
	entries[keyHash] = value;
}


void GXTVC3Table::setValue(char* key, char* value)
{
	int32_t hash = Crc32(key);

	if (keyNames) {
		(*keyNames)[hash] = key;
	}

	entries[hash] = value;
}


char* GXTVC3Table::getKeyName(int32_t keyHash)
{
	if (!keyNames) {
		return NULL;
	}

	KeyNameMap::iterator it = keyNames->find(keyHash);

	if (it == keyNames->end()) {
		return NULL;
	}

	return it->second;
}
