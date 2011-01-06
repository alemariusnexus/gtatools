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

#ifndef IMGARCHIVE_H_
#define IMGARCHIVE_H_

#include "../config.h"
#include "IMGVisitor.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include "../util/stream/InputStream.h"
#include "../util/stream/FileInputStream.h"


/**	\brief The size of a block in IMG files in bytes.
 */
#define IMG_BLOCK_SIZE 2048



class IMGVisitor;


/**	\brief An entry inside an IMGArchive.
 */
struct IMGEntry {
	/**	\brief The offset of the entry, in IMG blocks.
	 */
	int32_t offset;

	/**	\brief The size of the entry, in IMG blocks.
	 */
	int32_t size;

	/**	\brief The name of the entry (kind of a file name).
	 */
	char name[24];
};



/**	\brief Represents a single GTA IMG archive.
 *
 * 	IMG archives are general-purpose archives used by the GTA series. They have three versions: GTA3/VC,
 * 	GTA SA and GTA IV, where the first two are supported by this class. The supported archive formats
 * 	are identical with the exception that VER1 (GTA3/VC) files are stored in two files (.dir and .img),
 * 	while VER2 files (GTA SA) are stored in a single .img. Also, the four-byte header is missing in VER1.
 *
 * 	This class is suited both for random and stream access. When used in stream mode, seeking is not
 * 	possible and therefore you may not jump to an entry before the current position.
 *
 * 	@see File - You can simply treat DIR/IMG archives like plain directories with the file class.
 */
class IMGArchive {
public:
	/**	\brief The different versions of IMG files.
	 *
	 * 	VER1 is used in GTA3 and GTA VC. VER2 is used in GTA SA only.
	 */
	enum IMGVersion {
		VER1,	//!< GTA3, GTA VC (IMG header placed in a separate DIR file)
		VER2	//!< GTA SA
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
	static IMGVersion guessIMGVersion(InputStream* stream, bool returnToStart = true);

public:
	/**	\brief Creates an archive from a file.
	 *
	 * 	If the file is a DIR file (seen by the extension), then a VER1 file is assumed. The IMG file is
	 * 	assumed to have the same name, with the extension replaced by .img.
	 * 	If it's an IMG file (by extension), then guessVersion() is used to determine whether it's VER1 or
	 * 	VER2. If it's VER1, the DIR file is assumed to have the same name, with it's extension replaced
	 * 	by .dir.
	 * 	If nothing of this matches, an IMGException is thrown.
	 *
	 *	@param file The file. Can be either a DIR or an IMG file.
	 */
	IMGArchive(const File& file, bool deleteStream = true);

	/**	\brief Creates a VER2 archive from a stream, with random or stream access.
	 *
	 *	Note that the stream will not be automatically closed or deleted!
	 *
	 *	@param stream The stream to read from.
	 */
	IMGArchive(InputStream* stream, bool deleteStream = false);

	/**	\brief Creates an archive from two VER1 streams.
	 *
	 * 	The two streams are the ones for DIR and IMG. See IMGArchive(InputStream*, bool) for more information
	 * 	about randomAccess. Note that dirStream is not needed after the archive is once constructed, but
	 * 	it is not automatically deleted. You may close it after the constructor.
	 *
	 *	@param dirStream The stream from which to read the DIR data. Not needed after constructor.
	 *	@param imgStream The stream from which to read the IMG data.
	 */
	IMGArchive(InputStream* dirStream, InputStream* imgStream, bool deleteIMGStream = false);

	/**	\brief Creates an archive from a VER1 DIR and IMG file.
	 *
	 *	@param dirFile The DIR file.
	 *	@param imgFile The IMG file.
	 */
	IMGArchive(const File& dirFile, const File& imgFile, bool deleteStream = true);

	/**	\brief Deletes the archive.
	 *
	 *	This deletes the cached entry headers and deletes the stream if it was auto-generated by the
	 *	constructor.
	 */
	~IMGArchive();

	/**	\brief Sets the IMG stream to the start of the given entry.
	 *
	 * 	You can jump to any entry from anywhere you want. If random access is disabled, you may not go to
	 * 	entries before the current stream position. A new RangedInputStream will be created so that you can't read beyond the
	 * 	bounds of the entry. However, that is just a filter stream, so the actual stream used is the IMG stream. Therefore, you
	 * 	may not delete the IMG stream (which might be auto-generated by IMGArchive constructors and might therefore be destroyed
	 * 	together with the IMGArchive) before your reading is completed.
	 *
	 * 	@param entry The entry to go to.
	 * 	@param autoCloseStream if true, when the returned RangedInputStream is deleted, the IMG stream will be automatically
	 * 		deleted too. Only set this to true if you know that your IMG stream lives longer than the IMGArchive, because attempts
	 * 		to use an IMGArchive with a deleted stream may result in undefined behavior.
	 * 	@return A new RangedInputStream that reads from the IMG stream. It's range is defined as the bounds of the IMG entry, so
	 * 		you can't read beyond it.
	 */
	InputStream* gotoEntry(const IMGEntry* entry, bool autoCloseStream = false);

	/**	\brief Goes to the first entry with the given name.
	 *
	 * 	You <b>should</b> see gotoEntry(IMGEntry*, bool) for more details on behavior and parameters of this method.
	 *
	 *	@param name The entry's name. Searching is performed case-sensitive.
	 *	@param autoCloseStream Whether you want the RangedInputStream to automatically delete the IMG stream when deleted.
	 *	@return A new RangedInputStream for the first entry with the given name or NULL, if it's not found.
	 *	@see gotoEntry(IMGEntry*, bool)
	 */
	InputStream* gotoEntry(const char* name, bool autoCloseStream = false);

	/**	\brief Returns an entry by it's name.
	 *
	 * 	This is done case-sensitive.
	 *
	 *	@param name The name.
	 *	@return The entry or NULL, if it's not found.
	 */
	const IMGEntry* getEntryByName(const char* name) const;

	/**	\brief Returns the list of entries.
	 *
	 *	@return The entries.
	 */
	const IMGEntry* getEntries() const
	{
		return entries;
	}

	/**	\brief Returns the number of entries.
	 *
	 *	@return The number of entries.
	 */
	int32_t getEntryCount() const
	{
		return numEntries;
	}

	/**	\brief Returns the IMG stream.
	 *
	 *	@return The IMG stream.
	 */
	const InputStream* getStream() const {
		return stream;
	}

	/**	\brief Returns the format version of the file represented by this archive.
	 *
	 *	@return The format version.
	 */
	IMGVersion getVersion() const
	{
		return version;
	}


	/**	\brief Sets the position of the internal position marker for non-random access stream.
	 *
	 * 	If random access is not enabled, you always have to call this method when you manually read some
	 * 	bytes from the stream (e.g. to read data after gotoEntry()). In random access mode, no internal
	 * 	marker is needed and thus this method does nothing.
	 *
	 *	@param offset The new offset of the stream, relative to the current marker position.
	 */
	//void reposition(int offset);

private:
	/**	\brief Opens a new stream on the file.
	 *
	 * 	This method exists for historical reasons and might be removed some day.
	 *
	 *	@param file The file to open a stream for.
	 *	@return The stream.
	 */
	InputStream* openStream(const File& file);

	/**	\brief Reads the IMG/DIR header out of the given stream.
	 *
	 *	The 'header' is the real header and the entry table for VER2 and the whole DIR file for VER1.
	 *
	 *	@param stream The stream to read from (the DIR stream for VER1).
	 */
	void readHeader(InputStream* stream);

private:
	InputStream* stream;
	IMGEntry* entries;
	int32_t numEntries;
	IMGVersion version;

	bool deleteStream;
};

#endif /* IMGARCHIVE_H_ */
