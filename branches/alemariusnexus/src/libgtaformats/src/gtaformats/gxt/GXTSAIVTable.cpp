/*
 * GXTSAIVTable.cpp
 *
 *  Created on: 18.09.2010
 *      Author: alemariusnexus
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
