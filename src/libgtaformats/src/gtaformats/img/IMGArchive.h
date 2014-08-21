/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef IMGARCHIVE_H_
#define IMGARCHIVE_H_

#include <gtaformats/config.h>
#include <nxcommon/strutil.h>
#include <nxcommon/StringComparator.h>
#include <nxcommon/iterator.h>
#include <nxcommon/cxx11hash.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <istream>
#include <ostream>
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <utility>
#include <memory>
#include <cmath>

class File;

using std::istream;
using std::ostream;
using std::iostream;
using std::list;
using std::vector;
using std::map;
using std::unordered_map;
using std::pair;
using std::shared_ptr;


/**	\brief The size of a block in IMG files in bytes.
 *
 * 	Don't change this, it is fixed in the IMG file format.
 */
#define IMG_BLOCK_SIZE 2048L

/**	\brief Convert a number of IMG blocks to a number of bytes.
 */
#define IMG_BLOCKS2BYTES(n) ((n) * (uint64_t) (2048))

/**	\brief Convert a number of bytes to a number of IMG blocks.
 *
 * 	This will always yield an integer. It is rounded up if necessary.
 */
#define IMG_BYTES2BLOCKS(n) ((uint32_t) ceil((n) / 2048.0f))




#pragma pack(push, 1)

/**	\brief An entry inside an IMGArchive.
 *
 * 	You should not change any of the values in here directly if the entry belongs to an IMGArchive. That can only be
 * 	done through IMGArchive methods, or if the IMGEntry does not (yet) belong to an archive.
 */
struct IMGEntry {
	/**	\brief The offset of the entry, in IMG blocks.
	 *
	 * 	This is measured from the beginning of the IMG file in both VER1 and VER2.
	 */
	uint32_t offset;

	/**	\brief The size of the entry, in IMG blocks.
	 */
	uint32_t size;

	/**	\brief The name of the entry (kind of a file name).
	 *
	 * 	It must be null-terminated.
	 */
	char name[24];
};

#pragma pack(pop)



/**	\brief Represents a single GTA IMG archive.
 *
 * 	IMG archives are general-purpose archives used by the GTA series. They have three versions: GTA3/VC,
 * 	GTA SA and GTA IV, where the first two are supported by this class. The supported archive formats
 * 	are identical with the exception that VER1 (GTA3/VC) files are stored in two files (.dir and .img),
 * 	while VER2 files (GTA SA) are stored in a single .img. Also, the IMG header is missing in VER1.
 *
 *	An IMG archive consists of the IMG header (VER2 only), the entry header section (also called entry table;
 *	for VER1 archives, this is the whole content of the DIR file. For VER2, it is the first part of the IMG
 *	file), and the content section (Also called the data section; The whole content of the IMG file for VER1,
 *	or a part of the IMG file for VER2). The entry header section stores the name and size of each entry, as
 *	well as the offset into the content section to it's data. The content section contains the actual data of
 *	the entry.
 *
 *	All positions and sizes are measured in IMG blocks, which are 2048 bytes in size (see IMG_BLOCK_SIZE),
 *	unless otherwise noted.
 *
 *	\warning All write access to IMGArchives is <b>experimental</b>, so always make a <b>backup</b> of your
 *	files when using it!
 *
 * 	@see File - You can simply treat DIR/IMG archives like plain directories with the File class. However,
 * 		writing to IMGArchives is not supported that way.
 */
class IMGArchive {
private:
	typedef list<IMGEntry> EntryList;

public:
	typedef EntryList::iterator EntryIterator;
	typedef EntryList::const_iterator ConstEntryIterator;

private:
	typedef unordered_map<CString, EntryIterator, CXX11Hash<CString> > EntryMap;

public:
	/**	\brief The different versions of IMG files.
	 *
	 * 	VER1 is used in GTA3 and GTA VC. VER2 is used in GTA SA only.
	 */
	enum IMGVersion {
		VER1,	//!< GTA3, GTA VC (IMG header placed in a separate DIR file)
		VER2	//!< GTA SA
	};

	/**	\brief The modes in which you can open an IMGArchive. ReadOnly and ReadWrite are mutually exclusive.
	 */
	enum IMGMode {
		ReadOnly = 1 << 0,				/*!< Only open the file for reading. All writing operations will
											 throw an exception. */
		ReadWrite = 1 << 1,				//!< Open the file for both reading and writing.
		DisableNameZeroFill = 1 << 7	/*!< Disable filling up the entry name with zeroes after the first
		 	 	 	 	 	 	 	 	 	 null terminator is found. */
	};

	/**	\brief The modes used for moving entries inside the archive.
	 *
	 * 	@see moveEntries()
	 */
	enum MoveMode {
		MoveSwap,		/*!< A second range is constructed starting at (or ending with) the insertion
		 	 	 	 	 	 position, and advancing up to (or from) the move range. This range is then
		 	 	 	 	 	 swapped with the move range by calling swapConsecutive().

		 	 	 	 	 	 This mode keeps the archive packed (at least as packed as it was before) and
		 	 	 	 	 	 preserves the order of the other entries, but it might take a longer time and
		 	 	 	 	 	 more memory because more blocks might have to be moved. */
		MoveToEnd		/*!< Entries following the insertion position are moved to the end of the archive
		 	 	 	 	 	 until enough space is available to move the entries.

		 	 	 	 	 	 This mode is faster than MoveSwap, but it leaves holes in the archive, which
		 	 	 	 	 	 might need to be filled using pack() once in a while. Also, this mode does NOT
		 	 	 	 	 	 preserve the order of the other entries. */
	};


	/**	\brief Try to guess version of an IMG/DIR file.
	 *
	 * 	This method redirects to guessIMGVersion(istream*, bool).
	 *
	 *	@param filename The file name. It has to exist.
	 *	@return The guessed version.
	 */
	static IMGVersion guessIMGVersion(const File& file);

	/**	\brief Try to guess version of an IMG/DIR stream.
	 *
	 * 	The version is guessed by looking at the header, which is the string 'VER2' for VER2, while VER1
	 * 	has no such header. You may pass IMG or DIR files here (because the method returns VER1 for each
	 * 	case where there is no 'VER2' in the header, which is true for VER1 IMG files, too).
	 *
	 *	@param stream The stream from which to read.
	 *	@param returnToStart When true, the stream will be put back to the position where it was before.
	 *	@return The guessed version.
	 */
	static IMGVersion guessIMGVersion(istream* stream, bool returnToStart = true);

	/**	\brief Creates a new, empty VER2 IMG archive.
	 *
	 * 	This writes out the IMG header to imgStream and leaves the archive empty. A new IMGArchive object is
	 * 	then created with the given mode and returned.
	 *
	 * 	Ownership of the stream is taken by the new object.
	 *
	 * 	@param imgStream The stream to which the IMG file should be written. It becomes the new IMGArchive's
	 * 		IMG stream.
	 * 	@param mode The open mode.
	 * 	@return The freshly created archive object.
	 */
	static IMGArchive* createArchive(iostream* imgStream, int mode = ReadWrite);

	/**	\brief Creates a new, empty VER1 IMG archive.
	 *
	 * 	This method actually does nothing, because VER1 IMG files have no IMG header. This means that even
	 * 	an empty DIR/IMG pair is a valid VER1 archive which could directly be opened using the IMGArchive
	 * 	constructor. This method is here for convenience.
	 *
	 * 	Ownership of both streams is taken by the new object.
	 *
	 * 	\note Because nothing is actually written by this method, if the streams are file streams to files
	 * 		which don't exist yet, they won't be created by this method. Constructing an IMGArchive on such
	 * 		a non-existant file stream will however throw an exception, so be sure that your files <b>both
	 * 		exist</b> before you call this method.
	 *
	 * 	@param dirStream The stream to which the IMG header (DIR) should be written. It becomes the new
	 * 		IMGArchive's DIR stream.
	 * 	@param imgStream The stream to which the IMG file should be written. It becomes the new IMGArchive's
	 * 		IMG stream.
	 * 	@param mode The open mode.
	 * 	@return The freshly created archive object.
	 */
	static IMGArchive* createArchive(iostream* dirStream, iostream* imgStream,
			int mode = ReadWrite);

	/**	\brief Creates a new, empty IMG archive.
	 *
	 * 	If version is VER2, an empty VER2 archive is created for the given file.
	 *
	 * 	If version is VER1, the corresponding DIR/IMG file is searched in the same directory using the same
	 * 	file name, with the extension substituted by dir/img. A VER1 archive is then created using this
	 * 	DIR/IMG pair.
	 *
	 *	@param file For VER2, this is the IMG file. For VER1, this is either the DIR or the IMG file.
	 *	@param version The version you wish to create.
	 *	@param mode The open mode.
	 *	@return The freshly created archive object.
	 */
	static IMGArchive* createArchive(const File& file, IMGVersion version = VER2, int mode = ReadWrite);

	/**	\brief Creates a new, empty VER1 IMG archive.
	 *
	 * 	This method simply calls createArchive(iostream*, iostream*, int).
	 *
	 *	@param dirFile The DIR file to use.
	 *	@param imgFile The IMG file to use.
	 *	@param mode The open mode.
	 *	@return The freshly created archive object.
	 *	@see createArchive(iostream*, iostream*, int)
	 */
	static IMGArchive* createArchive(const File& dirFile, const File& imgFile, int mode = ReadWrite);

public:
	/**	\brief Opens an IMG archive from a file.
	 *
	 * 	If the file is a DIR file (seen by the extension), then a VER1 file is assumed. The IMG file is
	 * 	assumed to have the same name, with the extension replaced by .img.
	 * 	If it's an IMG file (by extension), then guessVersion() is used to determine whether it's VER1 or
	 * 	VER2. If it's VER1, the DIR file is assumed to have the same name, with it's extension replaced
	 * 	by .dir.
	 * 	If nothing of this matches, an IMGException is thrown.
	 *
	 *	@param file The file. Can be either a DIR or an IMG file.
	 *	@param mode The open mode.
	 */
	IMGArchive(const File& file, int mode = ReadOnly);

	/**	\brief Opens a VER2 archive from a stream.
	 *
	 * 	Ownership of the stream is taken by the new object.
	 *
	 *	@param stream The stream to read from.
	 *	@param mode The open mode.
	 */
	IMGArchive(istream* stream, int mode = ReadOnly);

	/**	\brief Creates a VER1 archive from a DIR and an IMG stream.
	 *
	 * 	Ownership of both streams is taken by the new object.
	 *
	 *	@param dirStream The stream from which to read the DIR data.
	 *	@param imgStream The stream from which to read the IMG data.
	 *	@param mode The open mode.
	 */
	IMGArchive(istream* dirStream, istream* imgStream, int mode = ReadOnly);

	/**	\brief Opens an archive from a VER1 DIR and IMG file.
	 *
	 *	@param dirFile The DIR file.
	 *	@param imgFile The IMG file.
	 *	@param mode The open mode.
	 */
	IMGArchive(const File& dirFile, const File& imgFile, int mode = ReadOnly);

	/**	\brief Deletes the archive.
	 *
	 *	This method calls sync(), frees any memory it allocated, and deletes the streams.
	 */
	~IMGArchive();

	/**	\brief Sets the IMG stream to the start of the given entry.
	 *
	 * 	You can jump to any entry from anywhere you want. A new RangedStream will be created so that you can't
	 * 	read beyond the bounds of the entry. However, that is just a filter stream, so the actual stream used
	 * 	is the IMG stream. Therefore, you may not delete the IMG stream (which might be auto-generated by
	 * 	IMGArchive constructors and will therefore be destroyed together with the IMGArchive) before you're
	 * 	finished using the returned stream.
	 *
	 * 	@param it Iterator to the entry you want to go to.
	 * 	@return A new RangedStream that reads from the IMG stream. It's range is defined as the bounds of the
	 * 		IMG entry, so you can't read beyond it.
	 */
	istream* gotoEntry(const ConstEntryIterator& it);

	/**	\brief Goes to the first entry with the given name.
	 *
	 * 	You <b>should</b> see gotoEntry(ConstEntryIterator*, bool) for more details on behavior and parameters
	 * 	of this method.
	 *
	 *	@param name The entry's name. Searching is performed case-insensitive.
	 *	@return A new RangedStream for the first entry with the given name or NULL, if it's not found.
	 *	@see gotoEntry(const ConstEntryIterator&)
	 */
	istream* gotoEntry(const CString& name);

	/**	\brief Returns a ConstEntryIterator for the first entry with the given name.
	 *
	 * 	Searching is done case-insensitive.
	 *
	 *	@param name The name.
	 *	@return The entry iterator or the end iterator, if it's not found.
	 *	@see EntryIterator getEntryByName(const CString&)
	 */
	ConstEntryIterator getEntryByName(const CString& name) const;

	/**	\brief Returns an EntryIterator for the first entry with the given name.
	 *
	 * 	Searching is done case-insensitive.
	 *
	 *	@param name The name.
	 *	@return The entry iterator or the end iterator, if it's not found.
	 *	@see ConstEntryIterator getEntryByName(const CString&)
	 */
	EntryIterator getEntryByName(const CString& name);

	/**	\brief Returns an iterator to the beginning of the entry list.
	 *
	 * 	@return The begin iterator.
	 * 	@see ConstEntryIterator getEntryBegin()
	 */
	EntryIterator getEntryBegin() { return entries.begin(); }

	/**	\brief Returns an iterator to the end of the entry list.
	 *
	 * 	@return The past-the-end iterator.
	 * 	@see ConstEntryIterator getEntryEnd()
	 */
	EntryIterator getEntryEnd() { return entries.end(); }

	/**	\brief Returns a const iterator to the beginning of the entry list.
	 *
	 * 	@return The begin iterator.
	 * 	@see EntryIterator getEntryBegin()
	 */
	ConstEntryIterator getEntryBegin() const { return entries.begin(); }

	/**	\brief Returns a const iterator to the end of the entry list.
	 *
	 * 	@return The past-the-end iterator.
	 * 	@see EntryIterator getEntryEnd()
	 */
	ConstEntryIterator getEntryEnd() const { return entries.end(); }

	/**	\brief Synonym for getEntryBegin().
	 */
	EntryIterator begin() { return entries.begin(); }

	/**	\brief Synonym for getEntryBegin().
	 */
	ConstEntryIterator begin() const { return entries.begin(); }

	/**	\brief Synonym for getEntryEnd().
	 */
	EntryIterator end() { return entries.end(); }

	/**	\brief Synonym for getEntryEnd().
	 */
	ConstEntryIterator end() const { return entries.end(); }

	/**	\brief Returns the number of entries in the archive.
	 *
	 *	@return The number of entries.
	 */
	uint32_t getEntryCount() const { return entries.size(); }


	/**	\brief Returns the IMG stream.
	 *
	 *	@return The IMG stream.
	 */
	const istream* getIMGStream() const { return imgStream; }


	/**	\brief Returns the IMG stream.
	 *
	 *	@return The IMG stream.
	 */
	istream* getIMGStream() { return imgStream; }


	/**	\brief Returns the DIR stream.
	 *
	 * 	For VER2 archives, this is the same object as the IMG stream.
	 *
	 *	@return The DIR stream.
	 */
	const istream* getDIRStream() const { return dirStream; }


	/**	\brief Returns the DIR stream.
	 *
	 * 	For VER2 archives, this is the same object as the IMG stream.
	 *
	 *	@return The DIR stream.
	 */
	istream* getDIRStream() { return dirStream; }


	/**	\brief Returns the format version of this archive.
	 *
	 *	@return The format version.
	 */
	IMGVersion getVersion() const { return version; }


	/**	\brief Returns the number of blocks reserved for the entry header section.
	 *
	 * 	This is 0 for VER1 archives as well as for empty VER2 archives, because those don't have anything
	 * 	to reserve.
	 *
	 *	@return The number of blocks reserved for the header section or 0 if there's no need to reserve
	 *		anything.
	 */
	uint32_t getHeaderReservedSize() const;

	/**	\brief Returns the number of entries which fit in the reserved entry header section.
	 *
	 * 	This is 0 for VER1 archives as well as for empty VER2 archives.
	 *
	 * 	@return The number of entries which fit in the entry header section, or 0. This is not the number of
	 * 		actual current entries.
	 * 	@see getHeaderReservedSize()
	 */
	uint32_t getReservedHeaderCount() const
	{
		uint32_t res = getHeaderReservedSize();
		if (res == 0) return 0;
		// This has to be a VER2 archive (res would be 0 for VER1) => 8 bytes reserved for IMG header.
		return (res * IMG_BLOCK_SIZE - 8) / sizeof(IMGEntry);
	}

	/**	\brief Returns the number of entries for which space is left in the entry header section.
	 *
	 * 	This is basically getReservedHeaderCount() - getEntryCount(), or UINT32_MAX for VER1 archives.
	 *
	 * 	@return The number of entries for which space is left in the entry header section, or UINT32_MAX
	 * 		for VER1 archives.
	 */
	uint32_t getReservedHeadersLeft() const
	{
		uint32_t res = getReservedHeaderCount();
		if (res == 0) return UINT32_MAX;
		return res - entries.size();
	}

	/**	\brief Reserves space in the entry header section for a given number of entries.
	 *
	 * 	If the current reserved size of the entry header section is too small to hold the given number of
	 * 	entry headers, it will be resized. This means that the first IMG entries will be moved to the end
	 * 	of the archive until there's enough space left in the beginning to store the requested number of
	 * 	entry headers. Moving entries is quite an expensive operation, because all their content has to be
	 * 	moved as well, so you should avoid calling this method too often. Also, it's better to do one big
	 * 	resize than many small ones.
	 *
	 * 	The case when the requested entry header section would be bigger than the whole current archive is
	 * 	also handled correctly by moving all the entries as far as necessary.
	 *
	 * 	This method will currently not shrink the header section, but it might (and that is very likely) do
	 * 	so in future versions. It will, however, never truncate the header section in a way that there's no
	 * 	more space left to store the current entries. It can therefore never be used to remove entries from
	 * 	the archive.
	 *
	 *	@param numHeaders The number of headers which should at least fit into the entry header section after
	 *		this call.
	 *	@return true if reserving was successful, or if there was no need to reserve anything. false
	 *		otherwise.
	 */
	bool reserveHeaderSpace(uint32_t numHeaders);


	/**	\brief Add new entries to the archive.
	 *
	 * 	This method adds the given entries with all content set to zeroes. It will automatically resize the
	 * 	header section if necessary.
	 *
	 * 	The arguments to this functions can be any InputIterator type that can be dereferenced to an IMGEntry
	 * 	(NOT to an IMGEntry*).
	 *
	 * 	Each entry in the array should have a valid name and size assigned. The offsets will be automatically
	 * 	calculated by this method.
	 *
	 * 	Zeroing the contents of the entries is done by a call to expandSize().
	 *
	 *	@param beg Beginning of the entry range.
	 *	@param end Past-the-end iterator of the entry range.
	 *	@return A pair that denotes the range of EntryIterators for the added entries (beginning and
	 *		past-the-end iterator). If this method fails, both of these will be the end iterators and no
	 *		entries will be added.
	 *	@see addEntry()
	 *	@see expandSize()
	 */
	template <class Iterator>
	pair<EntryIterator, EntryIterator> addEntries(const Iterator& beg, const Iterator& end)
	{
		if (!reserveHeaderSpace(entries.size() + std::distance(beg, end))) {
			return pair<EntryIterator, EntryIterator>(entries.end(), entries.end());
		}

		pair<EntryIterator, EntryIterator> res(entries.end(), entries.end());

		EntryIterator entryIt = entries.end();

		for (Iterator it = beg ; it != end ; it++) {
			entryIt = doAddEntry(it->name, it->size);

			if (it == beg) {
				res.first = entryIt;
			}
		}

		res.second = ++entryIt;

		return res;
	}

	/**	\brief Adds a single new entry to the archive.
	 *
	 * 	@param name The name of the new entry. Must be 23 characters or shorter (excluding null-terminator)
	 * 	@param size The size you want to allocate for the entry, in IMG blocks.
	 * 	@return An iterator to the new entry, or the past-the-end iterator if the method failed.
	 * 	@see addEntries()
	 */
	EntryIterator addEntry(const CString& name, uint32_t size);

	/**	\brief Changes the size of an entry.
	 *
	 * 	If the new size is smaller than the old one, or if there's enough space left after the content
	 * 	sections of this entry, the header's size property is simply changed.
	 *
	 * 	If however the new size is greater than the old one, and there's no more space left after the content
	 * 	section (because the content of another entry follows, which is very likely to be the case), the
	 * 	entry and it's content is moved to the end of the archive, and the content section is expanded with
	 *	zeroes by a call to expandSize().
	 *
	 *	\note This way of moving files to the end of the archive creates 'holes' where the entry was before.
	 *		These holes will currently not be used for anything and only take up space. You should therefore
	 *		avoid resizing entries too often and/or pack the archive once in a while using pack().
	 *
	 *	@param it The iterator to the entry you wish to resize.
	 *	@param size The new size of the entry, in IMG blocks.
	 *	@see expandSize()
	 *	@see pack()
	 */
	void resizeEntry(const EntryIterator& it, uint32_t size);


	/**	\brief Removes an entry from the archive.
	 *
	 * 	This simply removes it's entry header from the archive. Its content will however not be overwritten,
	 * 	and it might creates 'holes' in the archive which only take up space. So be sure to not remove entries
	 * 	too often and/or call pack() once in a while.
	 *
	 * 	Removing the end iterator is a no-op.
	 *
	 * 	@param it The iterator to the entry you wish to remove.
	 * 	@see pack()
	 */
	void removeEntry(const EntryIterator& it);

	/**	\brief Renames an entry.
	 *
	 *	This just changes it's name in the entry header. Using a name longer than 23 characters (excluding null
	 *	terminator) will throw an exception.
	 *
	 *	@param it The iterator to the entry you wish to rename.
	 *	@param name The new name. Maximum is 23 characters excluding null termiantor.
	 */
	void renameEntry(const EntryIterator& it, const CString& name);

	/**	\brief Renames an entry.
	 *
	 * 	@param oldName The old name of the entry that is to be renamed.
	 * 	@param newName The entry's new name.
	 * 	@see renameEntry(EntryIterator, const CString&)
	 */
	void renameEntry(const CString& oldName, const CString& newName);

	/**	\brief Move entries to a new position.
	 *
	 * 	This method moves all entries between moveBegin and moveEnd to pos. If there's not enough space left
	 * 	after pos, other entries might have to be moved. How this situation is handeled depends on the
	 * 	MoveMode selected.
	 *
	 * 	All iterators between moveBegin and moveEnd (including these two) are invalid after this method. Also,
	 * 	if the MoveSwap mode is used, all iterators in the second range are also invalidated.
	 *
	 * 	@param pos Iterator pointing to the entry before which the entries should be placed.
	 * 	@param moveBegin Iterator to the first entry being moved.
	 * 	@param moveEnd One past the iterator of the last entry to be moved.
	 * 	@param MoveMode How entries are moved when there's not enough space at pos.
	 */
	void moveEntries(const EntryIterator& pos, const EntryIterator& moveBegin, const EntryIterator& moveEnd,
			MoveMode mmode = MoveToEnd);

	/**	\brief Writes out changes to the header section.
	 *
	 * 	You can use this to explicitly flush your changes. However, this method will automatically be called
	 * 	by the IMGArchive destructor.
	 *
	 * 	If the archive is opened ReadOnly, this method does nothing.
	 *
	 * 	This method calls rewriteHeaderSection() to do the actual work.
	 *
	 *	@see rewriteHeaderSection()
	 * 	@see ~IMGArchive()
	 */
	void sync() { if ((mode & ReadWrite) != 0) rewriteHeaderSection(); }

	/**	\brief Packs the entries of this archive as tightly as possible.
	 *
	 *	This is used to eliminate 'holes' in the archive - defragmentation, remember? ;) -, as well as a
	 *	too-big header section.
	 *
	 *	Packing is done by copying the whole content of the archive to a temporary file, and rewriting the
	 *	whole archive, after offsets have been recalculated. Note that the packing is only as good as the
	 *	choice of the entry sizes, because this method will never resize any entry. It only packs them
	 *	together to fill holes, and resizes the entry header section to a minimum.
	 *
	 *	Because the IMGArchive does not know whether its stream is a file or something else, the archive
	 *	is not actually truncated. If it's a file, it's size remains the same afterwards, but the blocks in
	 *	the end might not be used anymore if packing had any effect. You have to manually truncate the file
	 *	afterwards if you want to see a change in size. You can use the return value of this method to
	 *	compute the new size of the file.
	 *
	 *	@return The new size of the archive, in blocks. Anything after this size may be truncated after
	 *		this method.
	 *	@see File::resize() may be used to truncate the file to its actual size after packing.
	 */
	uint32_t pack();

	/**	\brief Return the size of this archive.
	 *
	 * 	This is the combined size of the header and content section. For VER1 files, it take into account
	 * 	both the DIR and the IMG file.
	 *
	 * 	@return The size of this archive, in IMG blocks.
	 */
	uint32_t getSize() const
	{
		return version == VER2 ? getDataEndOffset() : getDataEndOffset() + IMG_BYTES2BLOCKS(entries.size() * sizeof(IMGEntry));
	}

	/**	\brief Swap two consecutive ranges of entries.
	 *
	 *	This method constructs two ranges, the first one going from r1Begin to r1End (excluding r1End itself),
	 *	the second one going from r1End to r2End (excluding r2End itself). These two entries and their
	 *	contents are swapped in the archive.
	 *
	 *	This method keeps the archive packed (at least as packed as it was before) and does not affect the
	 *	order of any entries outside the two swapped ranges.
	 *
	 *	All iterators referring to swapped elements should be considered invalid after calling this method.
	 *
	 *	@param r1Begin Iterator pointing to the beginning of the first range.
	 *	@param r1End Iterator pointing to one element past the end of the first range. This is also the
	 *		beginning of the second range.
	 *	@param r2End Iterator pointing to one element past the end of the second range.
	 */
	void swapConsecutive(const EntryIterator& r1Begin, const EntryIterator& r1End, const EntryIterator& r2End);

private:

	/**	\brief Reads the IMG/DIR header out of the given stream.
	 *
	 *	The 'header' is the real IMG header and the entry table for VER2 and the whole DIR file for VER1.
	 */
	void readHeader();


	/**	\brief Rewrite the header section.
	 *
	 * 	Functionality is documented in sync().
	 *
	 * 	@see sync()
	 */
	void rewriteHeaderSection();


	/**	\brief Returns the offset to the current end of the data section.
	 *
	 * 	For empty archives, this is the same as the reserved size of the entry header section.
	 *
	 * 	@return The offset of the end of the data section, in IMG blocks.
	 */
	uint32_t getDataEndOffset() const
	{
		if (entries.size() == 0) return headerReservedSpace;
		ConstEntryIterator it = --entries.end(); IMGEntry e = *it; return e.offset + e.size;
	}


	/**	\brief Move entries to a new position without checking for overlapping content.
	 *
	 * 	This moves all entries between moveBegin and moveEnd to pos, placing their contents at the newOffset.
	 * 	This method just overwrites any content at newOffset (i.e. it does not move the entries which were
	 * 	there before), so it should be used with caution.
	 *
	 * 	All iterators referring to moved entries are invalid after calling this method. All other iterators
	 * 	renaim valid.
	 *
	 * 	@param pos The position before which the new entries are inserted.
	 * 	@param moveBegin Iterator of the first entry to be moved.
	 * 	@param moveEnd One past the iterator of the last entry to be moved.
	 * 	@param newOffset The offset to store the first moved entry's contents, in IMG blocks.
	 */
	void forceMoveEntries(const EntryIterator& pos, const EntryIterator& moveBegin, const EntryIterator& moveEnd,
			uint32_t newOffset);


	/**	\brief Fills up the archive with zeroes in the end until it is size IMG blocks big.
	 *
	 * 	For VER1 files, this is just about the IMG file and does NOT include the DIR file.
	 *
	 * 	@param size The new size of the archive, in IMG blocks.
	 * 	@see expandSize()
	 */
	void expandSize(uint32_t size);

	/**	\brief Fills up the archive with zeroes until it has the size denoted by the entry headers.
	 *
	 * 	This method is used internally to create the empty content sections for new or moved entries.
	 *
	 * 	@see expandSize(int32_t)
	 */
	void expandSize();

	/**	\brief Add an entry without checking for enough reserved header size,
	 *
	 * 	@param name The name of the new entry.
	 * 	@param size The size of the new entry, in IMG blocks.
	 * 	@return Iterator to the new entry.
	 */
	EntryIterator doAddEntry(const CString& name, uint32_t size);

private:
	istream* imgStream;
	istream* dirStream;

	EntryList entries;
	EntryMap entryMap;
	IMGVersion version;

	// Only used to keep the setting for empty archives.
	uint32_t headerReservedSpace;

	int mode;
};


#endif /* IMGARCHIVE_H_ */
