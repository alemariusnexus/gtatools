/*
 * DefaultResourceManager.h
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#ifndef DEFAULTRESOURCEMANAGER_H_
#define DEFAULTRESOURCEMANAGER_H_

#include <gf_config.h>
#include "../util/StringComparator.h"
#include "ResourceIndex.h"
#include "../txd/TXDArchive.h"
#include "../txd/TXDTexture.h"
#include "../img/IMGArchive.h"
#include "../dff/DFFMesh.h"
#include "../dff/DFFLoader.h"
#include <map>

using std::map;



/**	\brief A cache entry for textures inside a DefaultResourceManager.
 */
struct TextureCacheEntry
{
	/**	\brief The texture header.
	 */
	TXDTexture* texture;

	/**	\brief The texture data.
	 *
	 * 	This is only valid if the DefaultResourceManager was created with cacheTextureData set to true.
	 */
	uint8_t* data;
};


/**	\brief A cache entry for meshes inside a DefaultResourceManager.
 */
struct MeshCacheEntry
{
	/**	\brief The mesh object.
	 */
	DFFMesh* mesh;
};



/**	\brief General-purpose implementation of a resource cache.
 *
 * 	This class acts as a cache for textures, meshes and other resources. As you can see, it is derived from ResourceIndex, from
 * 	which it receives it's data. It provides you with the resources using the same interface as ResourceIndex, but places
 * 	recently used resources in a cache for quicker access.
 * 	This cache is valuable in cases where you often need to access the same resource, like rendering a scene in every frame (note
 * 	that for OpenGL another class with specialized interface and internal caching strategy exists: OpenGLResourceManager). You then
 * 	don't have to read the resource data from the disk every time you need it. For textures, you also don't have to convert them
 * 	to a suitable raster format all the time, but only once when they are cached.
 *
 * 	As stated above, you use the same interface as the ResourceIndex. The data will then be read from the cache, not from the index.
 * 	If the resource is not yet cached, it will be read from the index and automatically placed into the cache. You can also force
 * 	caching of a given resource using cacheTexture(const char*) or cacheMesh(const char*).
 *
 * 	Also note that because the actual resources are cached, you may not delete them yourself. They will be automatically deleted when
 * 	they are removed from the cache, or when the cache is destroyed.
 *
 * 	\note Today, the cache implementation uses a simple std::map for the entries and it won't remove resources automatically (meaning
 * 	that you have to do so manually if you have many objects. Otherwise, your RAM may overflow). Use the uncache*() methods here.
 */
class DefaultResourceManager : public ResourceIndex {
private:
	typedef map<const char*, TextureCacheEntry*, StringComparator> TextureCacheMap;
	typedef map<const char*, MeshCacheEntry*, StringComparator> MeshCacheMap;

public:
	/**	\brief Creates a new DefaultResourceManager.
	 *
	 * 	@param cacheTextureData if true, the data of textures will be cached, too. Set this to false if you don't need texture
	 * 		data to save memory.
	 */
	DefaultResourceManager(bool cacheTextureData = true);

	/**	\brief Destroys this DefaultResourceManager.
	 *
	 * 	This effectively calls the uncache*() functions for all cache entries.
	 *
	 * 	@see uncacheMesh(const char*)
	 * 	@see uncacheTexture(const char*)
	 */
	~DefaultResourceManager();

	/**	\brief Receives the texture header with the given name from the cache.
	 *
	 *	If it does not exist yet, it is searched for in the index and cached.
	 *
	 *	@param name The texture name.
	 *	@param texture A reference to the pointer which will be set to point to the texture header.
	 *	@return true if the texture was found (inside the cache OR the index), false otherwise.
	 */
	virtual bool getTextureHeader(const char* name, TXDTexture*& texture);

	/**	\brief Receives the texture header and data with the given name from the cache.
	 *
	 *	If it does not exist yet, it is searched for in the index and cached.
	 *
	 *	@param name The texture name.
	 *	@param texture A reference to the pointer which will be set to point to the texture header.
	 *	@param texture A reference to the pointer which will be set to point to the texture data block.
	 *	@return true if the texture was found (inside the cache OR the index), false otherwise.
	 */
	virtual bool getTexture(const char* name, TXDTexture*& texture, uint8_t*& rawData);

	/**	\brief Receives the mesh with the given name from the cache.
	 *
	 * 	If it does not exist yet, it is searched for in the index and cached.
	 *
	 *	@param name The mesh name.
	 *	@param texture A reference to the pointer which will be set to point to the mesh.
	 *	@return true if the texture was found (inside the cache OR the index), false otherwise.
	 */
	virtual bool getMesh(const char* name, DFFMesh*& mesh);

	/**	\brief Forces caching of the given texture.
	 *
	 *	This creates a cache entry for the given texture if there wasn't one yet.
	 *
	 *	@param name The texture name.
	 *	@return true if the cache entry was successfully created or already existed, false otherwise.
	 */
	bool cacheTexture(const char* name);

	/**	\brief Forces removing of the given texture from cache.
	 *
	 *	 This deletes the cache entry as well as the objects itself, so don't attempt to use the objects for that texture afterwards.
	 *
	 *	 @param name The texture name.
	 *	 @return true if the texture was previously cached and successfully removed, false otherwise.
	 */
	bool uncacheTexture(const char* name);

	/**	\brief Forces caching of the given mesh.
	 *
	 * 	This creates a cache entry for the given mesh if there wasn't one yet.
	 *
	 * 	@param name The mesh name.
	 * 	@return true if the cache entry was successfully created or already existed, false otherwise.
	 */
	bool cacheMesh(const char* name);

	/**	\brief Forces removing of the given mesh from cache.
	 *
	 *	This deletes the cache entry as well as the object itself, so don't attempt to use the object for that mesh afterwards.
	 *
	 *	@param name The mesh name.
	 *	@return true if the texture was previously cached and successfully removed, false otherwise.
	 */
	bool uncacheMesh(const char* name);

private:
	/**	\brief Creates a cache entry for the given texture.
	 *
	 * 	Note that if data is valid, it will be put into the cache even if cacheTextureData was false in the constructor. Also, it
	 * 	won't check if the texture is already cached.
	 *
	 *	@param texture The texture header.
	 *	@param data The texture data block.
	 */
	void cacheTexture(TXDTexture* texture, uint8_t* data = NULL);

	/**	\brief Creates a cache entry for the given texture from the index and provides the header and data.
	 *
	 * 	Note that this won't check if the texture is already cached.
	 *
	 *	@param name The texture name.
	 *	@param texture Reference to a pointer to be set to point at the texture header.
	 *	@param data Reference to a pointer to be set to point at the texture data block.
	 *	@return true if the cache entry was successfully created, false otherwise.
	 */
	bool cacheTexture(const char* name, TXDTexture*& texture, uint8_t*& data);

	/**	\brief Creates a cache entry for the given mesh.
	 *
	 * 	Note that this won't check if the mesh is already cached.
	 *
	 * 	@param name The mesh name.
	 * 	@param The mesh object.
	 */
	void cacheMesh(const char* name, DFFMesh* mesh);

	/**	\brief Creates a cache entry for the given mesh from the index and provides the mesh object.
	 *
	 *	Note that this won't check if the mesh is already cached.
	 *
	 *	@param name The mesh name.
	 *	@param mesh Reference to a pointer to be set to point at the mesh object.
	 *	@return true if the cache entry was successfully created, false otherwise.
	 */
	bool cacheMesh(const char* name, DFFMesh*& mesh, bool placeholder);

	/**	\brief Removes the given texture entry from cache.
	 *
	 * 	@param it The texture entry.
	 */
	void uncacheTexture(TextureCacheMap::iterator it);

	/**	\brief Removes the given mesh entry from cache.
	 *
	 * 	@param it The mesh entry.
	 */
	void uncacheMesh(MeshCacheMap::iterator it);

	/**	\brief Determines whether the given texture is already cached.
	 *
	 * 	@param name The texture name.
	 * 	@return true if it is found inside the cache, false otherwise.
	 */
	bool isTextureCached(const char* name);

	/**	\brief Determines whether the given mesh is already cached.
	 *
	 *	@param name The mesh name.
	 *	@return true if it is found inside the cache, false otherwise.
	 */
	bool isMeshCached(const char* name);

private:
	bool cacheTextureData;
	TextureCacheMap textureCache;
	MeshCacheMap meshCache;
};

#endif /* DEFAULTRESOURCEMANAGER_H_ */
