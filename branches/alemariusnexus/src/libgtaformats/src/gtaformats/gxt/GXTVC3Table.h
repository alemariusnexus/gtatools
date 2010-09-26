/*
 * GXTVC3Table.h
 *
 *  Created on: 18.09.2010
 *      Author: alemariusnexus
 */

#ifndef GXTVC3TABLE_H_
#define GXTVC3TABLE_H_

#include "GXTTable.h"
#include <map>

using std::map;



/**	\brief A GXT table as used by GTA Vice City and GTA III.
 *
 * 	This version of GXT stores keys as plain text. However, for quicker access the entries are mapped using
 * 	the CRC32 checksum of the key name, just as with newer versions of GXT. But if you want, an additional
 * 	map can be maintained which maps the CRC32 hashes to the names again.
 *	This version of GXT usually stored values as GXT16.
 */
class GXTVC3Table : public GXTTable {
private:
	typedef map<int32_t, char*> EntryMap;
	typedef map<int32_t, char*> KeyNameMap;

public:
	/**	\brief Creates a new GXTVC3Table.
	 *
	 *	@param keyNames If true, the key names will be preserved so that you can use getKeyName().
	 */
	GXTVC3Table(Encoding internalEncoding = GXT16, bool keyNames = false);

	/**	\brief Destroys this GXTVC3Table including it's entries.
	 */
	virtual ~GXTVC3Table();

	virtual char* getValue(int32_t keyHash);

	virtual EntryIterator getFirstEntry() { return entries.begin(); }

	virtual EntryIterator getLastEntry() { return entries.end(); }

	virtual int getEntryCount() { return entries.size(); }

	/**	\brief Inserts or updates an entry.
	 *
	 * 	Note that if you use this version of setValue() with the CRC32 key hash, no key name will be
	 * 	accessible for this entry even if you chose to preserve key names. To preserve the key, you should use
	 * 	setValue(char*, char*).
	 *
	 * 	@param keyHash The CRC32 hash of the
	 * 	@param value The entry value.
	 * 	@ee setValue(char*, char*)
	 */
	void setValue(int32_t keyHash, char* value);

	/**	\brief Inserts or updates an entry.
	 *
	 * 	This method preserves the key name for getKeyName() if you chose to do so in the constructor.
	 *
	 * 	@param key The key of the entry.
	 * 	@param value The entry value.
	 * 	@see setValue(int32_t, char*)
	 */
	void setValue(char* key, char* value);

	/**	\brief Returns the preserved key name for a CRC32 key hash.
	 *
	 * 	This only works if you chose to preserve key names in the constructor and if you added the entry
	 * 	identified by the key hash by it's name rather than only by it's hash.
	 *
	 * 	@param keyHash The CRC32 key hash for which you want to get the key name.
	 * 	@return The key name or NULL if there's no preserved key for this entry.
	 */
	char* getKeyName(int32_t keyHash);

private:
	EntryMap entries;
	KeyNameMap* keyNames;
};

#endif /* GXTVC3TABLE_H_ */
