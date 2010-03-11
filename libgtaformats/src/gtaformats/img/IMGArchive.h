/*
 * IMGArchive.h
 *
 *  Created on: 21.01.2010
 *      Author: alemariusnexus
 */

#ifndef IMGARCHIVE_H_
#define IMGARCHIVE_H_

#include "../gf_config.h"
#include "IMGVisitor.h"
#include <istream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>

using std::istream;
using std::ifstream;


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

	/**	\brief The name of the entry (kind of like a file name).
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
 */
class IMGArchive {
public:
	/**	\brief The different versions of IMG files.
	 *
	 * 	VER1 is used in GTA3 and GTA VC. VER2 is used in GTA SA only.
	 */
	enum IMGVersion {
		VER1, // GTA3, GTA VC
		VER2 // GTA SA
	};

	/**	\brief Returns whether the file name is a valid IMG file name by looking at it's extension.
	 *
	 * 	This is done via a call to GFGuessFileType(). The file does not have to exist.
	 *
	 *	@param filename The file name.
	 *	@return Whether it's an IMG file (not .dir!).
	 */
	static bool isValidIMGFilename(const std::string& filename);

	/**	\brief Returns whether the file name is a valid DIR file name by looking at it's extension.
	 *
	 * 	This is done via a call to GFGuessFileType(). The file does not have to exist.
	 *
	 *	@param filename The file name.
	 *	@return Whether it's a DIR file.
	 */
	static bool isValidDIRFilename(const std::string& filename);

	/**	\brief Try to guess version of an IMG/DIR file.
	 *
	 * 	This method redirects to guessIMGVersion(istream*, bool).
	 *
	 *	@param filename The file name. It has to exist.
	 *	@return The guessed version.
	 */
	static IMGVersion guessIMGVersion(const char* filename);

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
	 *	@param filename The file name. Can be either a DIR or an IMG file.
	 */
	IMGArchive(const char* filename);

	/**	\brief Creates a VER2 archive from a stream, with random or stream access.
	 *
	 *	Note that the stream will not be automatically closed or deleted!
	 *
	 *	@param stream The stream to read from.
	 *	@param randomAccess When true, you may jump inside the file randomly in any direction. This will
	 *		be fine for most streams. However, for unbuffered streams where seeking is impossible, this
	 *		is not possible and you can not jump to entries before the current position. Use false then.
	 */
	IMGArchive(istream* stream, bool randomAccess = true);

	/**	\brief Creates an archive from two VER1 streams.
	 *
	 * 	The two streams are the ones for DIR and IMG. See IMGArchive(istream*, bool) for more information
	 * 	about randomAccess. Note that dirStream is not needed after the archive is once constructed, but
	 * 	it is not automatically deleted. You may close it after the constructor.
	 *
	 *	@param dirStream The stream from which to read the DIR data. Not needed after constructor.
	 *	@param imgStream The stream from which to read the IMG data.
	 *	@param randomAccess Whether to enable random access in imgStream. dirStream doesn't need this.
	 */
	IMGArchive(istream* dirStream, istream* imgStream, bool randomAccess = true);

	/**	\brief Creates an archive from a VER1 DIR and IMG file.
	 *
	 *	@param dirName The name of the DIR file.
	 *	@param imgName The name of the IMG file.
	 */
	IMGArchive(const char* dirName, const char* imgName);

	/**	\brief Deletes the archive.
	 *
	 *	This deletes the cached entry headers and deletes the stream if it was auto-generated by the
	 *	constructor.
	 */
	~IMGArchive();

	/**	\brief Sets the IMG stream to the start of the given entry and returns it.
	 *
	 * 	You can jump to any entry from anywhere you want. If random access is disabled, you may not go to
	 * 	entries before the current stream position. Note that no bounds checking is done, so it is
	 * 	possible to read beyond the end of the entry. You can not use end() to determine the entry's end,
	 * 	so you have to use IMGEntry::size.
	 *
	 *	@param entry The entry to jump to.
	 *	@return The IMG stream.
	 */
	istream* gotoEntry(IMGEntry* entry);

	/**	\brief Goes to the first entry with the given name.
	 *
	 *	@param name The entry's name. Searching is performed case-sensitive.
	 *	@return The IMG stream.
	 *	\see gotoEntry(IMGEntry*)
	 */
	istream* gotoEntry(const char* name);

	/**	\brief Visits the IMGEntry with the given visitor.
	 *
	 *	@param visitor The visitor.
	 *	@param entry The entry.
	 */
	void visit(IMGVisitor* visitor, IMGEntry* entry);

	/**	\brief Visits all entries with the given visitor.
	 *
	 *	@param visitor The visitor.
	 */
	void visitAll(IMGVisitor* visitor);

	/**	\brief Returns an entry by it's name.
	 *
	 * 	This is done case-sensitive.
	 *
	 *	@param name The name.
	 *	@return The entry.
	 */
	IMGEntry* getEntryByName(const char* name);

	/**	\brief Returns the list of entries.
	 *
	 *	@return The entries.
	 */
	IMGEntry** getEntries() {
		return entries;;
	}

	/**	\brief Returns the number of entries.
	 *
	 *	@return The number of entries.
	 */
	int32_t getEntryCount() {
		return numEntries;
	}

	/**	\brief Returns the IMG stream.
	 *
	 *	@return The IMG stream.
	 */
	istream* getStream() {
		return stream;
	}

	/**	\brief Returns the format version of the file represented by this archive.
	 *
	 *	@return The format version.
	 */
	IMGVersion getVersion()
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
	void reposition(int offset);

private:
	/**	\brief Opens a new stream on the file, assigning exception flags to it.
	 *
	 *	@param filename The file to open a stream for.
	 *	@return A stream with exception flags assigned.
	 */
	ifstream* openStream(const char* filename);

	/**	\brief Reads the IMG/DIR header out of the given stream.
	 *
	 *	The 'header' is the real header and the entry table for VER2 and the whole DIR file for VER1.
	 *
	 *	@param stream The stream to read from (the DIR stream for VER1).
	 */
	void readHeader(istream* stream);

private:
	istream* stream;
	IMGEntry** entries;
	int32_t numEntries;
	IMGVersion version;

	long long bytesRead;
	bool randomAccess;
	bool autoGeneratedStream;
};

#endif /* IMGARCHIVE_H_ */
