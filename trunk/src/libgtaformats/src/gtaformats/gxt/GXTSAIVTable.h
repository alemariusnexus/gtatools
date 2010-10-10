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

#ifndef GXTSAIVTABLE_H_
#define GXTSAIVTABLE_H_

#include "GXTTable.h"
#include <map>

using std::map;



/**	\brief A GXT table as used by GTA San Andreas and GTA IV.
 *
 * 	This GXT version does not store key names, but only their CRC32 checksums, which is why this table can't
 * 	provide you with any key names.
 * 	Entry values in these GXT files are usually stored as GXT8.
 */
class GXTSAIVTable : public GXTTable {
private:
	typedef map<int32_t, char*> EntryMap;

public:
	/**	\brief Creates an empty, new GXTSAIVTable.
	 *
	 * 	@see GXTTable(Encoding)
	 */
	GXTSAIVTable(Encoding internalEncoding = GXT8);

	/**	\brief Destroys this GTASAIVTable with all it's entries.
	 */
	virtual ~GXTSAIVTable();

	virtual char* getValue(int32_t keyHash);

	virtual EntryIterator getFirstEntry() { return entries.begin(); }

	virtual EntryIterator getLastEntry() { return entries.end(); }

	virtual int getEntryCount() { return entries.size(); }

	/**	\brief Inserts or updates an entry value.
	 *
	 * 	@param keyHash The CRC32 checksum of the entry key.
	 * 	@param value The new value. This GXTSAIVTable will adopt the string.
	 */
	void setValue(int32_t keyHash, char* value);

	/**	\brief Inserts or updates an entry value.
	 *
	 * 	@param keyHash The key name.
	 * 	@param value The new value. This GXTSAIVTable will adopt the string.
	 */
	void setValue(const char* key, char* value);

private:
	EntryMap entries;
};

#endif /* GXTSAIVTABLE_H_ */
