/*
 * GXTPlainKeyTable.cpp
 *
 *  Created on: 07.02.2010
 *      Author: alemariusnexus
 */

#include "GXTPlainKeyTable.h"


GXTPlainKeyTable::GXTPlainKeyTable(EntryMap* entries, KeyMap* keys)
		: GXTTable(entries), keys(keys)
{
}


const char* GXTPlainKeyTable::getKey(int32_t hash)
{
	return keys->find(hash)->second;
}
