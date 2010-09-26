/*
 * GXTSAIVTable.h
 *
 *  Created on: 18.09.2010
 *      Author: alemariusnexus
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
