/*
 * GXTTable.h
 *
 *  Created on: 17.09.2010
 *      Author: alemariusnexus
 */

#ifndef GXTTABLE_H_
#define GXTTABLE_H_

#include "../gf_config.h"
#include "../util/encoding.h"
#include <map>

using std::map;



/**	\brief Represents a generic GXT table.
 *
 * 	GXT Tables are named containers für GXT entries. A GXT entry consists of a key and a value (the actual
 * 	text). The format of the key differs between GXT versions and is either a plain string or a CRC32 hash.
 * 	Entries are all stored in a given internal encoding and convenience methods are provided to convert them
 * 	to other encodings. Note that values won't automatically be encoded in the internal encoding if you
 * 	insert them. It's just a hint for the conversion functions.
 *
 * 	@see Crc32()
 */
class GXTTable {
public:
	/**	\brief An entry iterator.
	 */
	typedef map<int32_t, char*>::iterator EntryIterator;

public:
	/**	\brief Creates an empty new GXTTable with the given internal encoding.
	 *
	 * 	@param internalEncoding The encoding in which the entry values are stored.
	 */
	GXTTable(Encoding internalEncoding);

	/**	\brief Destructor.
	 */
	virtual ~GXTTable() {}

	/**	\brief Returns the encoding in which the entries are stored.
	 *
	 * 	@return The internal encoding.
	 */
	Encoding getInternalEncoding() { return internalEncoding; }

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@param keyHash The CRC32 hash of the entry key.
	 * 	@return The entry value in the internal encoding.
	 */
	virtual char* getValue(int32_t keyHash) = 0;

	/**	\brief Returns the beginning of the entry map.
	 *
	 * 	@return The beginning of the entry map.
	 */
	virtual EntryIterator getFirstEntry() = 0;

	/**	\brief Returns the end of the entry map.
	 *
	 * 	@return The end of the entry map.
	 */
	virtual EntryIterator getLastEntry() = 0;

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@param key The key name of the entry.
	 * 	@return The entry value in the internal encoding.
	 */
	char* getValue(const char* key);

	/**	\brief Returns the value of an entry transcoded to UTF-8.
	 *
	 * 	This will always allocate a NEW string, even if the internal encoding is UTF-8, too.
	 *
	 * 	@param keyHash The CRC32 hash of the entry key.
	 * 	@return The entry value transcoded to UTF-8. You have to delete it when finished.
	 */
	char* getValueUTF8(int32_t keyHash);

	/**	\brief Returns the value of an entry transcoded to UTF-8.
	 *
	 * 	@param key The key name of the entry.
	 * 	@see getValueUTF8(int32_t)
	 */
	char* getValueUTF8(const char* key);

	/**	\brief Returns the value of an entry transcoded to UTF-16.
	 *
	 * 	This will always allocate a NEW string, even if the internal encoding is UTF-16, too.
	 *
	 * 	@param keyHash The CRC32 hash of the entry key.
	 * 	@return The entry value transcoded to UTF-16. You have to delete it when finished.
	 */
	char* getValueUTF16(int32_t keyHash);

	/**	\brief Returns the value of an entry transcoded to UTF-16.
	 *
	 * 	@param key The key name of the entry.
	 * 	@see getValueUTF16(int32_t)
	 */
	char* getValueUTF16(const char* key);

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@see getValue(int32_t)
	 */
	char* operator[](int32_t keyHash) { return getValue(keyHash); }

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@see getValue(const char*)
	 */
	char* operator[](const char* key) { return getValue(key); }

private:
	Encoding internalEncoding;
};

#endif /* GXTTABLE_H_ */
