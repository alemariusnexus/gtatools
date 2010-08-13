/*
 * DefaultResourceManager.cpp
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#include "DefaultResourceManager.h"
#include <utility>
#include "../util/util.h"

using std::pair;


DefaultResourceManager::DefaultResourceManager(bool cacheTextureData)
		: cacheTextureData(true)
{
}


DefaultResourceManager::~DefaultResourceManager()
{
	TextureCacheMap::iterator tit;

	for (tit = textureCache.begin() ; tit != textureCache.end() ; tit++) {
		uncacheTexture(tit);
	}


	MeshCacheMap::iterator mit;

	for (mit = meshCache.begin() ; mit != meshCache.end() ; mit++) {
		uncacheMesh(mit);
	}
}


bool DefaultResourceManager::getTextureHeader(const char* name, TXDTexture*& texture)
{
	TextureCacheMap::iterator it = textureCache.find(name);

	if (it == textureCache.end()) {
		uint8_t* tmpData;
		return cacheTexture(name, texture, tmpData);
	}

	TextureCacheEntry* entry = it->second;
	texture = entry->texture;
	return true;
}


bool DefaultResourceManager::getTexture(const char* name, TXDTexture*& texture, uint8_t*& rawData)
{
	TextureCacheMap::iterator it = textureCache.find(name);

	if (it == textureCache.end()) {
		return cacheTexture(name, texture, rawData);
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
		return cacheMesh(name, mesh, true);
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

	char* texName = new char[strlen(texture->getDiffuseName()) + 1];
	strtolower(texName, texture->getDiffuseName());

	textureCache.insert(pair<const char*, TextureCacheEntry*>(texName, entry));
}


void DefaultResourceManager::uncacheTexture(TextureCacheMap::iterator it)
{
	delete[] it->first;
	delete it->second->texture;
	delete[] it->second->data;
	delete it->second;
	textureCache.erase(it);
}


bool DefaultResourceManager::uncacheTexture(const char* name)
{
	TextureCacheMap::iterator it = textureCache.find(name);

	if (it == textureCache.end()) {
		return false;
	}

	uncacheTexture(it);
	return true;
}


bool DefaultResourceManager::cacheTexture(const char* name, TXDTexture*& texture, uint8_t*& data)
{
	if (cacheTextureData) {
		if (!ResourceIndex::getTexture(name, texture, data)) {
			return false;
		}
	} else {
		if (!ResourceIndex::getTextureHeader(name, texture)) {
			return false;
		}
	}

	cacheTexture(texture, data);
	return true;
}


bool DefaultResourceManager::cacheTexture(const char* name)
{
	if (isTextureCached(name)) {
		return true;
	}

	TXDTexture* tex = NULL;
	uint8_t* data = NULL;
	return cacheTexture(name, tex, data);
}


void DefaultResourceManager::cacheMesh(const char* name, DFFMesh* mesh)
{
	MeshCacheEntry* entry = new MeshCacheEntry;
	entry->mesh = mesh;

	meshCache.insert(pair<const char*, MeshCacheEntry*>(name, entry));
}


bool DefaultResourceManager::cacheMesh(const char* name, DFFMesh*& mesh, bool placeholder)
{
	if (!ResourceIndex::getMesh(name, mesh)) {
		return false;
	}

	cacheMesh(name, mesh);
	return true;
}


bool DefaultResourceManager::cacheMesh(const char* name)
{
	if (isMeshCached(name)) {
		return true;
	}

	DFFMesh* mesh;
	return cacheMesh(name, mesh, true);
}


void DefaultResourceManager::uncacheMesh(MeshCacheMap::iterator it)
{
	delete[] it->first;
	delete it->second->mesh;
	delete it->second;
	meshCache.erase(it);
}


bool DefaultResourceManager::uncacheMesh(const char* name)
{
	MeshCacheMap::iterator it = meshCache.find(name);

	if (it != meshCache.end()) {
		uncacheMesh(it);
		return true;
	}

	return false;
}


bool DefaultResourceManager::isTextureCached(const char* name)
{
	return textureCache.find(name) != textureCache.end();
}


bool DefaultResourceManager::isMeshCached(const char* name)
{
	return meshCache.find(name) != meshCache.end();
}

