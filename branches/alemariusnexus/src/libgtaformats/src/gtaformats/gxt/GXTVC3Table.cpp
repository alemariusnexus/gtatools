/*
 * GXTVC3Table.cpp
 *
 *  Created on: 18.09.2010
 *      Author: alemariusnexus
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
