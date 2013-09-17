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

#ifndef TXDARCHIVE_H_
#define TXDARCHIVE_H_

#include "../rwbs/RWSection.h"
#include "TXDTextureHeader.h"
#include <istream>
#include <map>
#include "../util/strutil.h"
#include "../util/File.h"

using std::istream;
using std::map;



/**	\brief Represents a GTA TXD archive.
 *
 *	This class is a high-level representation of the GTA Texture Dictionary (TXD) files. A TXD file contains
 *	a number of textures in different formats. Each texture consists of a header and the texture data (and
 *	probably something else, because <i>in theory</i>, TXD is an extensible format).
 *
 *	This class loads the complete archive content into memory on initialization and provides an interface to
 *	work with this data. The archive content is internally represented using an RWSection object.
 *
 *	If you use this class to write to TXD files, you have to be careful to choose only texture properties
 *	which are supported by the original GTA if you want to use them for it. Which combinations are actually
 *	supported isn't known very well, so basically you have to test and hope. One thing known to cause
 *	problems is using mipmaps in uncompressed textures, but there will sure be others, too.
 *
 *	\warning All writing access to TXD files is <b>experimental</b>, so <b>backup</b> your files before you
 *		write anything!
 */
class TXDArchive {
public:
	typedef vector<TXDTextureHeader*> TextureList;
	typedef TextureList::iterator TextureIterator;
	typedef TextureList::const_iterator ConstTextureIterator;

private:
	typedef map<CString, RWSection*, StringComparator> TexNativeMap;

public:
	/**	\brief Creates a new, empty TXD archive.
	 */
	TXDArchive();

	/**	\brief Creates a TXDArchive by reading data from a stream.
	 *
	 * 	The stream will not be used after this constructor, so you can safely delete it.
	 *
	 * 	@pram stream The stream to read from.
	 */
	TXDArchive(istream* stream);

	/**	\brief Creates a TXDArchie by reading data from a file.
	 *
	 * 	@param file The file to read from.
	 */
	TXDArchive(const File& file);

	/**	\brief Destroys this TXDArchive.
	 *
	 *	This will delete all the TXD data from memory, so be sure not to use a TXDTextureHeader directly
	 *	obtained from this class afterwards (if you want to, make a copy).
	 */
	~TXDArchive();

	/**	\brief Returns the number of textures currently in the archive.
	 *
	 * 	@return The number of textures.
	 */
	int16_t getTextureCount() const { return texHeaders.size(); }

	/**	\brief Returns the begin iterator of the texture headers.
	 *
	 * 	@return The begin iterator.
	 */
	TextureIterator getHeaderBegin() { return texHeaders.begin(); }

	/**	\brief Returns the end iterator of the texture headers.
	 *
	 * 	@return The end iterator.
	 */
	TextureIterator getHeaderEnd() { return texHeaders.end(); }

	/**	\brief Returns the begin iterator of the texture headers.
	 *
	 * 	@return The begin iterator.
	 */
	ConstTextureIterator getHeaderBegin() const { return texHeaders.begin(); }

	/**	\brief Returns the end iterator of the texture headers.
	 *
	 * 	@return The end iterator.
	 */
	ConstTextureIterator getHeaderEnd() const { return texHeaders.end(); }

	/**	\brief Returns the header of the texture at the given index.
	 *
	 *	@param num The texture index.
	 *	@return The texture header. The object behind this pointer belongs to this class, so do not delete it.
	 */
	TXDTextureHeader* getHeader(int16_t num) { return texHeaders[num]; }

	/**	\brief Returns the header of the texture at the given index.
	 *
	 * 	Same as getHeader(int16_t).
	 *
	 *	@param num The texture index.
	 *	@return The texture header. The object behind this pointer belongs to this class, so do not delete it.
	 *	@see getHeader(int16_t)
	 */
	TXDTextureHeader* operator[](int16_t num) { return getHeader(num); }

	/**	\brief Returns the raw pixel data of the given texture.
	 *
	 *	The texture data returned is just the concatenated pixel data of all mipmaps. If the texture is
	 *	paletted, the palette data is prepended.
	 *
	 *	@param header The texture to read. The actual texture is identified by diffuse name, so you do not
	 *		have to use the same TXDTextureHeader object as stored by this class, as long as the name is the
	 *		same.
	 *	@return The texture data. This is a dynamically allocated memory block. Ownership is passed to the
	 *		caller.
	 */
	uint8_t* getTextureData(TXDTextureHeader* header);

	/**	\brief Writes changes of the given texture header back into the internal RWSections.
	 *
	 * 	You need to call this everytimes you make changes to a header and want to apply them.
	 *
	 * 	@param header The texture header whose changes are to apply.
	 */
	void applyTextureHeader(TXDTextureHeader* header);

	/**	\brief Sets the raw data for the given texture.
	 *
	 * 	The data is expected to be in the format described in getTextureData(TXDTextureHeader*). If the header
	 * 	given as a parameter is not in sync with the internal RWSections, you'll have to sync it using
	 * 	applyTextureHeader(TXDTextureHeader*) first.
	 *
	 * 	@param header The texture header whose data you want to change.
	 * 	@param data The new texture data. A copy of it is stored, so no ownership will be taken.
	 */
	void setTextureData(TXDTextureHeader* header, uint8_t* data);

	/**	\brief Renames a texture.
	 *
	 *	You have to explicitly call this method for name changes, because textures are internally identified
	 *	using their names.
	 *
	 *	@param header The header you want to change. It should have the old texture name set.
	 *	@param name The new name for the texture.
	 */
	void rename(TXDTextureHeader* header, const CString& name);

	/**	\brief Adds a new texture to the end of the archive.
	 *
	 * 	@param header The header of the new texture. Ownership of it is taken.
	 * 	@param data The texture data, in the format described in getTextureData(TXDTextureHeader*). A copy
	 * 		of it will be stored, so no ownership will be taken.
	 */
	void addTexture(TXDTextureHeader* header, uint8_t* data = NULL);

	/**	\brief Removes the given texture from the archive.
	 *
	 * 	This will also delete the internal TXDTextureHeader, so if you obtained your header from this class,
	 * 	you may not use it afterwards.
	 *
	 *	@param header The header of the texture to be removed.
	 */
	void removeTexture(TXDTextureHeader* header);

	/**	\brief Write out the TXD archive to the given stream.
	 *
	 * 	This will write out the internal RWSections, so you have to be sure that all your changes are
	 * 	synced using applyTextureHeader(TXDTextureHeader*).
	 *
	 * 	@param stream The stream to write to.
	 */
	void write(ostream* stream);

	/**	\brief Write out the TXD archive to the given file.
	 *
	 * 	This method calls write(ostream*).
	 *
	 * 	@param file The file to write to.
	 */
	void write(const File& file);

private:
	void init(istream* stream);
	void applyTextureCountChange();

private:
	istream* stream;
	RWSection* texDict;
	TextureList texHeaders;
	TexNativeMap texNativeMap;
};

#endif /* TXDARCHIVE_H_ */
