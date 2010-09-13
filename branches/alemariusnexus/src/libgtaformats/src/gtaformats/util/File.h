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

#ifndef FILE_H_
#define FILE_H_

#include "FilePath.h"
#include "FileIterator.h"
#include <cstdlib>
#include "../stdint.h"
#include "SharedData.h"
#include "SharedDataPointer.h"

struct IMGEntry;
class IMGArchive;
class InputStream;


#ifdef linux
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#else
#include <windows.h>
#endif


/**	\brief Types of files which can be returned by File::getType().
 */
enum FileType {
	TYPE_ERROR,		//<! An error occured when trying to get the file type.

	TYPE_FILE,		//<! The file is a regular file.
	TYPE_DIRECTORY,	//<! The file is a directory.
	TYPE_LINK,		//<! The file is a (symbolic) link.
	TYPE_OTHER		//<! The file has an unknown type (e.g. device files on Linux).
};



/**	\brief Platform-independent representation of a file.
 *
 * 	This class handles operations on files on the local filesystem. it uses the FilePath class to handle paths to the files. Files
 * 	do not have to exist to be used by this class (but some methods throw exceptions if the file does not exist).
 *
 * 	In addition to normal files, this class can also handle IMG/DIR archives like they were simple directories.
 * 	For example, if '/home/alemariusnexus/gta3.img' was an IMG archive, then a path '/home/alemariusnexus/gta3.img/bistro.txd'
 * 	might be used to access the file 'bistro.txd' inside 'gta3.img'. IMG archives will be treated just like directories and can be
 * 	iterated over. Files inside IMG files may be opened using openStream().
 */
class File {
public:
	typedef int64_t filesize;

public:
	/**	\brief Constructs a file from the given path.
	 *
	 * 	@param path The path to the file (does not have to exist).
	 */
	File(const char* path);

	/**	\brief Constructs a file from a given path.
	 *
	 * 	The path will be used without being copied.
	 *
	 *	@param path The path to be used. This constructor will not make a copy of it. Therefore, don't delete it without this
	 *		file being deleted too.
	 *	@param autoDeletePath If true, the path will be automatically deleted by this file's destructor.
	 */
	File(FilePath* path, bool autoDeletePath = true);

	/**	\brief Copy constructor.
	 */
	File(const File& other);

	/**	\brief Construct a file from a parent and child file.
	 *
	 * 	@param parent The parent file.
	 * 	@param The child file.
	 */
	File(const File& parent, const char* child);

	/**	\brief Destructor.
	 */
	~File();


	/**	Returns whether this file physically exists.
	 *
	 * 	A file physically exists if it is actually found on the filesystem. Files inside IMG files return false here, even if they
	 * 	exist inside the IMG files. Use this function to check for existance of actual filesystem files, because this will be
	 * 	quicker than exists() in some cases.
	 *
	 *	@return true if the file exists (files inside IMG/DIR archives always return false here).
	 */
	bool physicallyExists() const;

	/**	\brief Checks whether this file exists.
	 *
	 * 	Existance checks are also performed for files inside IMG archives. If you don't need this, consider using physicallyExists(),
	 * 	as it is quicker in some cases.
	 *
	 *	@return true if the file exists (files inside IMG/DIR archives are checked, too).
	 */
	bool exists() const;

	/**	\brief Determines the type of this file (e.g. regular file, directory, link).
	 *
	 * 	@return The file type or TYPE_ERROR in case of error.
	 */
	FileType getType() const;

	/**	\brief Get an iterator over the child files and directories of this file.
	 *
	 * 	IMG archives can be iterated, too. The entries will be iterated then.
	 *
	 *	@return The child iterator.
	 */
	FileIterator* getIterator() const;

	/**	\brief Returns the parent of this file.
	 *
	 *	That is the directory in which this file resides.
	 *
	 *	@return The parent of this file or NULL if it has no parent.
	 */
	File* getParent() const;

	/**	\brief Returns this files FilePath.
	 *
	 * 	@return The path of this file.
	 */
	const FilePath* getPath() const { return path; }

	/**	\brief Returns true if this file is a directory.
	 *
	 * 	@return true if it's a directory.
	 */
	bool isDirectory() const { return getType() == TYPE_DIRECTORY; }

	/**	\brief Returns true if this file is a (symbolic) link.
	 *
	 * 	@return true if it's a link.
	 */
	bool isLink() const { return getType() == TYPE_LINK; }

	/**	\brief Returns true if this file is a regular file (no directory, link etc.).
	 *
	 * 	@return true if it's a regular file.
	 */
	bool isRegularFile() const { return getType() == TYPE_FILE; }

	/**	\brief Tries to guess the content type of this file by looking at the file extension.
	 *
	 *	@return The guesses content type.
	 *	@see FilePath::guessContentType()
	 */
	FileContentType guessContentType() const;

	/**	\brief Opens a stream on this file.
	 *
	 * 	Files inside IMG/DIR archives can also be opened. The corresponding entries will then be opened using IMGArchive.
	 *
	 * 	@param flags The stream flags.
	 * 	@return The newly created stream.
	 */
	InputStream* openStream(int flags = 0) const;

	File* getChild(int childIdx) const;

	int getChildCount(bool recursive = false, bool archiveEntries = false) const;

	int indexOf(const File& other) const;

	int getDirectoryIndex() const;

	bool isArchiveFile() const;

	filesize getSize() const;

	bool mkdir() const;

	bool mkdirs() const;

	bool isChildOf(const File& other) const;

	bool operator==(const File& other) const;
	bool operator!=(const File& other) const { return !(*this == other); }
	bool operator>(const File& other) const;
	bool operator>=(const File& other) const { return !(*this < other); }
	bool operator<(const File& other) const;
	bool operator<=(const File& other) const { return !(*this > other); }

private:
	FilePath* path;
	bool autoDeletePath;
};

#endif /* FILE_H_ */
