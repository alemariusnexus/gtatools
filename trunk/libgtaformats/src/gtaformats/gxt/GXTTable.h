/*
 * GXTTable.h
 *
 *  Created on: 07.02.2010
 *      Author: alemariusnexus
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
