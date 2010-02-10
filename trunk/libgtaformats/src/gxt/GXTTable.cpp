/*
 * GXTTable.cpp
 *
 *  Created on: 07.02.2010
 *      Author: alemariusnexus
 */

#include "GXTTable.h"



GXTTable::GXTTable(EntryMap* entries)
		: entries(entries)
{
}


GXTTable::~GXTTable()
{
	EntryMap::iterator it;

	for (it = entries->begin() ; it != entries->end() ; it++) {
		delete[] it->second;
	}

	delete entries;
}


const GXTTable::value_t* GXTTable::getValue(int32_t hash)
{
	return entries->find(hash)->second;
}
