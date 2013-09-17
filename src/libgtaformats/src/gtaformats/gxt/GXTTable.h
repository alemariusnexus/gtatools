/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef GXTTABLE_H_
#define GXTTABLE_H_

#include <gtaformats/config.h>
#include "../util/CString.h"
#include "../util/encoding.h"
#include "../gta.h"
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
	typedef map<crc32_t, char*>::iterator EntryIterator;
	typedef map<crc32_t, char*>::const_iterator ConstEntryIterator;
	typedef map<crc32_t, char*> EntryMap;
	typedef map<crc32_t, CString> KeyNameMap;

public:
	/**	\brief Creates an empty new GXTTable with the given internal encoding.
	 *
	 * 	@param internalEncoding The encoding in which the entry values are stored.
	 */
	GXTTable(const CString& name, Encoding internalEncoding, bool keepKeyNames = false);

	/**	\brief Destructor.
	 */
	~GXTTable();

	CString getName() const { return name; }

	/**	\brief Returns the encoding in which the entries are stored.
	 *
	 * 	@return The internal encoding.
	 */
	Encoding getInternalEncoding() const { return internalEncoding; }

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@param keyHash The CRC32 hash of the entry key.
	 * 	@return The entry value in the internal encoding.
	 */
	char* getValue(int32_t keyHash);

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@param keyHash The CRC32 hash of the entry key.
	 * 	@return The entry value in the internal encoding.
	 */
	const char* getValue(int32_t keyHash) const;

	/**	\brief Returns the beginning of the entry map.
	 *
	 * 	@return The beginning of the entry map.
	 */
	EntryIterator getFirstEntry() { return entries.begin(); }

	/**	\brief Returns the constant beginning of the entry map.
	 *
	 * 	@return The constant beginning of the entry map.
	 */
	ConstEntryIterator getFirstEntry() const { return entries.begin(); }

	/**	\brief Returns the end of the entry map.
	 *
	 * 	@return The end of the entry map.
	 */
	EntryIterator getLastEntry() { return entries.end(); }

	/**	\brief Returns the constant end of the entry map.
	 *
	 * 	@return The constant end of the entry map.
	 */
	ConstEntryIterator getLastEntry() const { return entries.end(); }

	int getEntryCount() { return entries.size(); }

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@param key The key name of the entry.
	 * 	@return The entry value in the internal encoding.
	 */
	char* getValue(const CString& key) { return getValue(Crc32(key.get())); }

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@param key The key name of the entry.
	 * 	@return The entry value in the internal encoding.
	 */
	const char* getValue(const CString& key) const { return getValue(Crc32(key.get())); }

	/**	\brief Returns the value of an entry transcoded to UTF-8.
	 *
	 * 	This will always allocate a NEW string, even if the internal encoding is UTF-8, too.
	 *
	 * 	@param keyHash The CRC32 hash of the entry key.
	 * 	@return The entry value transcoded to UTF-8. You have to delete it when finished.
	 */
	char* getValueUTF8(crc32_t keyHash) const;

	/**	\brief Returns the value of an entry transcoded to UTF-8.
	 *
	 * 	@param key The key name of the entry.
	 * 	@see getValueUTF8(int32_t)
	 */
	char* getValueUTF8(const CString& key) const;

	/**	\brief Returns the value of an entry transcoded to UTF-16.
	 *
	 * 	This will always allocate a NEW string, even if the internal encoding is UTF-16, too.
	 *
	 * 	@param keyHash The CRC32 hash of the entry key.
	 * 	@return The entry value transcoded to UTF-16. You have to delete it when finished.
	 */
	char* getValueUTF16(crc32_t keyHash) const;

	/**	\brief Returns the value of an entry transcoded to UTF-16.
	 *
	 * 	@param key The key name of the entry.
	 * 	@see getValueUTF16(int32_t)
	 */
	char* getValueUTF16(const CString& key) const;

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@see getValue(int32_t)
	 */
	char* operator[](crc32_t keyHash) { return getValue(keyHash); }

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@see getValue(int32_t)
	 */
	const char* operator[](crc32_t keyHash) const { return getValue(keyHash); }

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@see getValue(const char*)
	 */
	char* operator[](const CString& key) { return getValue(key); }

	/**	\brief Returns the value of an entry in the internal encoding.
	 *
	 * 	@see getValue(const char*)
	 */
	const char* operator[](const CString& key) const { return getValue(key); }

	void setValue(crc32_t keyHash, char* value);

	void setValue(const CString& key, char* value);

	void setKeyName(crc32_t keyHash, const CString& name);

	CString getKeyName(crc32_t keyHash) const;

private:
	CString name;
	Encoding internalEncoding;
	EntryMap entries;
	KeyNameMap* keyNames;
};

#endif /* GXTTABLE_H_ */
