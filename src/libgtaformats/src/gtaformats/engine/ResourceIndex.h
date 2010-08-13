/*
	Copyright 2010 David Lerch

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

#ifndef RESOURCEINDEX_H_
#define RESOURCEINDEX_H_

#include <gf_config.h>
#include "../util/StringComparator.h"
#include "../txd/TXDTexture.h"
#include "../txd/TXDArchive.h"
#include "../img/IMGArchive.h"
#include "../dff/DFFMesh.h"
#include "../util/File.h"
#include <map>
#include <vector>
#include "../util/thread/Mutex.h"

using std::map;
using std::vector;


/**	\brief The entry for a texture inside the ResourceIndex.
 */
struct TextureEntry
{
	/**	\brief The TXD file.
	 */
	File* file;

	/**	\brief The index of the texture inside the TXD file.
	 */
	int32_t txdIndex;
};


/**	\brief The entry for a mesh inside the ResourceIndex.
 */
struct MeshEntry
{
	/**	\brief The DFF file.
	 */
	File* file;
};





/**	\brief Index for quick searches of meshes, textures and other resources.
 *
 * 	This class is a file index aiming to provide quicker access to textures, meshes and other resources by their name. This is done
 * 	by initially giving a list of files to the ResourceIndex, which it searches for those resources. All found resources are indexed
 * 	in a map. By searching for their names, the ResourceIndex can easily figure out in which file the resource resides and can
 * 	quickly load it from there.
 *
 * 	\note If you search for resources, you always have to provide the names in <b>lower-case</b>, because all names are internally
 * 		stored lower-case and searching is case-sensitive.
 */
class ResourceIndex {
public:
	enum RasterFormat {
		Native,
		R8G8B8A8
	};

private:
	/**	\brief The texture cache map.
	 */
	typedef map<const char*, TextureEntry*, StringComparator> TextureMap;

	/**	\brief The mesh cache map.
	 */
	typedef map<const char*, MeshEntry*, StringComparator> MeshMap;

public:
	/**	\brief Deleted this ResourceIndex.
	 *
	 * 	Only the internal index will be deleted. You can still use all dynamically created resource objects which you received
	 * 	from this index afterwards.
	 */
	~ResourceIndex();

	/**	\brief Add a new resource to be indexed.
	 *
	 * 	This resource can be a TXD or DFF file as well as a directory or an IMG/DIR file (in which case it is recursively searched
	 * 	for other resource files). An exception will be thrown if the file type is invalid.
	 *
	 * 	@param file The resource file (TXD, DFF, IMG/DIR or a directory)
	 */
	void addResource(const File& file);

	/**	\brief Finds the texture header with the given name in the index.
	 *
	 * 	This method fetches the texture file from the index and then reads the header of the given texture from it. It is quicker
	 * 	than getTexture(), because it only fetches the texture header, not data.
	 *
	 * 	@param name The texture name.
	 * 	@param texture The reference to a pointer which will be set to point at the new texture. If no texture was found, it is
	 * 		left untouched.
	 * 	@return true if the texture was found in the index, false otherwise.
	 * 	@see getTexture()
	 */
	virtual bool getTextureHeader(const char* name, TXDTexture*& texture);

	/**	\brief Finds the texture header and data with the given name in the index.
	 *
	 *	This method fetches the texture file from the index and then reads the header and data of the given texture from it.
	 *
	 *	@param name The texture name.
	 *	@param texture The reference to a pointer which will be set to point at the new texture header. If no texture was found, it
	 *		is left untouched.
	 *	@param rawData The reference to a pointer which will be set to point at the beginning of the texture data array. If no
	 *		texture was found, it is left untouched.
	 *	@return true if the texture was found in the index, false otherwise.
	 *	@see getTextureHeader(), which is quicker if you only need the texture header.
	 */
	virtual bool getTexture(const char* name, TXDTexture*& texture, uint8_t*& rawData);

	/**	\brief Finds the mesh with the given name in the index.
	 *
	 * 	This method fetches the mesh file from the index and then loads it into a DFFMesh.
	 *
	 * 	@param name The mesh name.
	 * 	@param mesh The reference to a pointer which will be set to point at the new mesh. If no mesh was found, it is left
	 * 		untouched.
	 * 	@return true if the texture was found in the index, false otherwise.
	 */
	virtual bool getMesh(const char* name, DFFMesh*& mesh);

	void setTextureRasterFormat(RasterFormat format) { this->textureFormat = format; };

	/**	\brief Returns the file in which the texture was defined.
	 *
	 * 	The file belongs to this index, so don't delete it!
	 *
	 * 	@param name The texture name.
	 * 	@return The (TXD-) file in which the texture was defined or NULL if the texture was not found inside the cache.
	 */
	const File* findTexture(const char* name);

	/**	\brief Returns the file in which the mesh was defined.
	 *
	 * 	The file belongs to this index, so don't delete it!
	 *
	 * 	@param name The mesh name.
	 * 	@return The model file in which the mesh was defined or NULL if the texture was not found inside the cache.
	 */
	const File* findMesh(const char* name);

private:
	/**	\brief Creates an index entry for all textures inside the given TXDArchive.
	 *
	 * 	@param txd The archive in which the textures reside.
	 * 	@param file The file representing the TXDArchive.
	 */
	void indexTXD(TXDArchive* txd, const File& file);

	/**	\brief Creates an index entry for the given DFF mesh.
	 *
	 * 	@param file The DFF file.
	 */
	void indexDFF(const File& file);

	/**	\brief Finds and opens the texture with the given name.
	 *
	 * 	The corresponding TXDArchive will be opened and positioned at the beginning of the texture's data block. The TXDArchive
	 * 	and texture header will be provided to the caller. The caller is responsible for deleting the TXDArchive and header.
	 *
	 * 	@param name The texture name.
	 *	@param txd The opened TXDArchive, placed at the beginning of the texture's data block.
	 *	@param texture The texture header.
	 *	@return true if the texture was found in the index, false otherwise.
	 */
	bool gotoTexture(const char* name, TXDArchive*& txd, TXDTexture*& texture);

protected:
	RasterFormat textureFormat;

private:
	TextureMap textureIndex;
	MeshMap meshIndex;

	Mutex textureMutex;
	Mutex meshMutex;
};

#endif /* RESOURCEINDEX_H_ */
