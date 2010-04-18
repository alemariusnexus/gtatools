/*
 * DefaultResourceManager.h
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#ifndef DEFAULTRESOURCEMANAGER_H_
#define DEFAULTRESOURCEMANAGER_H_

#include "../gf_config.h"
#include "../internal/StringComparator.h"
#include "ResourceIndex.h"
#include "../txd/TXDArchive.h"
#include "../txd/TXDTexture.h"
#include "../img/IMGArchive.h"
#include "../dff/DFFMesh.h"
#include "../dff/DFFLoader.h"
#include <map>

using std::map;



struct TextureCacheEntry
{
	TXDTexture* texture;
	uint8_t* data;
};

struct MeshCacheEntry
{
	DFFMesh* mesh;
};


class DefaultResourceManager : public ResourceIndex {
private:
	typedef map<const char*, TextureCacheEntry*, StringComparator> TextureCacheMap;
	typedef map<const char*, MeshCacheEntry*, StringComparator> MeshCacheMap;

public:
	DefaultResourceManager(bool cacheTextureData = true);
	virtual bool getTextureHeader(const char* name, TXDTexture*& texture);
	virtual bool getTexture(const char* name, TXDTexture*& texture, uint8_t*& rawData);
	virtual bool getMesh(const char* name, DFFMesh*& mesh);
	void cacheTexture(const char* name);
	bool uncacheTexture(const char* name);
	void cacheMesh(const char* name);
	bool uncacheMesh(const char* name);

private:
	void cacheTexture(TXDTexture* texture, uint8_t* data = NULL);

private:
	bool cacheTextureData;
	TextureCacheMap textureCache;
	MeshCacheMap meshCache;
};

#endif /* DEFAULTRESOURCEMANAGER_H_ */
