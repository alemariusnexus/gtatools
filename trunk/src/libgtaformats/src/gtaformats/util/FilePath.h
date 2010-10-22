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

#ifndef FILEPATH_H_
#define FILEPATH_H_


/**	\brief Content types which can be guessed by FilePath::guessContentType().
 */
enum FileContentType {
	CONTENT_TYPE_UNKNOWN,	//!< The content type is unknown.

	CONTENT_TYPE_IMG,		//!< The file is an IMG archive.
	CONTENT_TYPE_TXD,		//!< The file is a texture dictionary (TXD).
	CONTENT_TYPE_DIR,		//!< The file is a DIR archive.
	CONTENT_TYPE_DFF,		//!< The file is a DFF mesh.
	CONTENT_TYPE_IDE,		//!< The file is an item definition (IDE) file.
	CONTENT_TYPE_IPL,		//!< The file is an item placement (IPL) file.
	CONTENT_TYPE_DAT		//!< The file is a DAT file.
};


/**	\brief Represents file paths.
 */
class FilePath {
public:
	enum Options
	{
		CorrectCase = 1,
		BackslashAsSeparator = 2
	};

public:
	/** \brief Constructs a new FilePath object from a path string.
	 *
	 *	@param path The path string.
	 */
	FilePath(const char* path, int flags = 0);

	/**	\brief Copy constructor.
	 *
	 * 	@param other The path to copy.
	 */
	FilePath(const FilePath& other);

	/**	\brief Creates a new FilePath from a parent path and child file name.
	 *
	 * 	No checking will be done to ensure the path really exists.
	 *
	 * 	@param parent The parent FilePath.
	 *	@param child The child file name.
	 */
	FilePath(const FilePath& parent, const char* child, int flags = 0);

	~FilePath();

	/**	\brief Returns the file name extension of this path.
	 *
	 *	@return The file name extension (without the '.').
	 */
	const char* getExtension() const;

	/**	\brief Returns the file name.
	 *
	 * 	That is the last part of the path.
	 *
	 *	@return The file name.
	 */
	const char* getFileName() const;

	/**	\brief Returns a part of the path.
	 *
	 * 	Parts are seperated by the directory separator character '/' and begin at index 0.
	 *
	 *	@param The index of the path part.
	 *	@return The path part (a newly created string).
	 */
	char* getPathPart(int partNo);

	/**	\brief Returns the string representation of this path.
	 *
	 *	@return The string representation of this path.
	 */
	const char* toString() const { return path; };

	/**	\brief Returns the parent path.
	 *
	 *	@return The parent path or NULL if this path has no parent.
	 */
	FilePath* getParentPath() const;

	/**	\brief Tries to guess the content type of the file pointed to by this path by looking at it's extension.
	 *
	 * 	@return The guessed content type.
	 */
	FileContentType guessContentType() const;

	/**	\brief Returns true if this path points to a file inside an IMG or DIR archive.
	 *
	 * 	This is <b>not true for IMG/DIR archives themselves!</b>. See the File class for more information on IMG paths.
	 *
	 *	@return true if this path points to a file inside an IMG/DIR archive.
	 */
	bool isIMGPath() const;

	bool isChildOf(const FilePath& other) const;

	bool operator==(const FilePath& other) const;
	bool operator!=(const FilePath& other) const { return !(*this == other); }
	bool operator>(const FilePath& other) const;
	bool operator>=(const FilePath& other) const { return !(*this < other); }
	bool operator<(const FilePath& other) const;
	bool operator<=(const FilePath& other) const { return !(*this > other); }

private:
	/**	\brief Normalizes a path.
	 *
	 * 	This removes directory separators from the end of the path and may do other normalizing tasks.
	 *
	 * 	@param src The input path.
	 * 	@return The normalized path.
	 */
	static char* normalize(const char* src, int flags);

private:
	char* path;
};

#endif /* FILEPATH_H_ */
