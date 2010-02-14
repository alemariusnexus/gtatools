/*
 * GXTPlainKeyTable.h
 *
 *  Created on: 07.02.2010
 *      Author: alemariusnexus
 */

#ifndef GXTPLAINKEYTABLE_H_
#define GXTPLAINKEYTABLE_H_

#include "../config.h"
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
