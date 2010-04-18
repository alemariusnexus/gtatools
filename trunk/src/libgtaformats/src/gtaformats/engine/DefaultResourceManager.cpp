/*
 * DefaultResourceManager.cpp
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#include "DefaultResourceManager.h"
#include <utility>

using std::pair;


DefaultResourceManager::DefaultResourceManager(bool cacheTextureData)
		: cacheTextureData(true)
{
}


bool DefaultResourceManager::getTextureHeader(const char* name, TXDTexture*& texture)
{
	TextureCacheMap::iterator it = textureCache.find(name);

	if (it == textureCache.end()) {
		return ResourceIndex::getTextureHeader(name, texture);
	}

	TextureCacheEntry* entry = it->second;
	texture = entry->texture;
	return true;
}


bool DefaultResourceManager::getTexture(const char* name, TXDTexture*& texture, uint8_t*& rawData)
{
	TextureCacheMap::iterator it = textureCache.find(name);

	if (it == textureCache.end()) {
		return ResourceIndex::getTexture(name, texture, rawData);
	}

	TextureCacheEntry* entry = it->second;
	texture = entry->texture;
	rawData = entry->data;
	return true;
}


bool DefaultResourceManager::getMesh(const char* name, DFFMesh*& mesh)
{
	MeshCacheMap::iterator it = meshCache.find(name);

	if (it == meshCache.end()) {
		return ResourceIndex::getMesh(name, mesh);
	}

	MeshCacheEntry* entry = it->second;
	mesh = entry->mesh;
	return true;
}


void DefaultResourceManager::cacheTexture(TXDTexture* texture, uint8_t* data)
{
	TextureCacheEntry* entry = new TextureCacheEntry;
	entry->texture = texture;
	entry->data = data;

	textureCache.insert(pair<const char*, TextureCacheEntry*>(texture->getDiffuseName(), entry));
}


bool DefaultResourceManager::uncacheTexture(const char* name)
{
	return textureCache.erase(name) == 1;
}


void DefaultResourceManager::cacheTexture(const char* name)
{
	TXDTexture* tex = NULL;
	uint8_t* data = NULL;

	if (cacheTextureData) {
		ResourceIndex::getTexture(name, tex, data);
	} else {
		ResourceIndex::getTextureHeader(name ,tex);
	}

	cacheTexture(tex, data);
}


void DefaultResourceManager::cacheMesh(const char* name)
{
	DFFMesh* mesh;
	ResourceIndex::getMesh(name, mesh);

	MeshCacheEntry* entry = new MeshCacheEntry;
	entry->mesh = mesh;

	meshCache.insert(pair<const char*, MeshCacheEntry*>(name, entry));
}


bool DefaultResourceManager::uncacheMesh(const char* name)
{
	return meshCache.erase(name) == 1;
}

