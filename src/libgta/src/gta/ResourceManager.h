/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <gtaformats/util/File.h>
#include <gtaformats/util/StringComparator.h>
#include <gtaformats/txd/TXDTexture.h>
#include <gtaformats/dff/DFFMesh.h>
#include <gtaformats/util/thread/Mutex.h>
#include <map>
#include <GL/glew.h>
#include <cstdio>
#include "TextureIndex.h"
#include "Engine.h"
#include "Mesh.h"
#include "ItemDefinition.h"
#include <set>

using std::map;
using std::set;



class ResourceManager {
private:
	struct MeshEntry
	{
		File* file;
	};


	struct TextureEntry
	{
		int16_t textureIndex;
	};

	typedef map<hash_t, TextureEntry*> TextureEntryMap;

	struct TextureCacheEntry
	{
		GLuint texture;
	};

	struct MeshCacheEntry
	{
		Mesh* mesh;
	};


	struct TXDEntry
	{
		TXDEntry* parent;
		TextureEntryMap textures;
		File* file;
	};

	typedef map<hash_t, TXDEntry*> TextureMap;
	typedef map<TextureEntry*, TextureCacheEntry*> TextureCacheMap;
	typedef map<hash_t, MeshEntry*> MeshMap;
	typedef map<hash_t, MeshCacheEntry*> MeshCacheMap;
	typedef map<int32_t, ItemDefinition*> ItemDefinitionMap;

public:
	ResourceManager();
	~ResourceManager();
	void addResource(const File& file);
	GLuint bindTexture(const TextureIndex& index);
	bool getTexture(const TextureIndex& index, TXDTexture*& tex);
	bool getTexture(const TextureIndex& index, TXDTexture*& tex, uint8_t*& data);
	bool cacheTexture(const TextureIndex& index);
	bool cacheTextures(hash_t txdHash);
	void uncacheTexture(const TextureIndex& index);
	void uncacheTextures(hash_t txdHash);
	bool findTextureArchive(hash_t texName, hash_t* txdName);
	bool getMesh(hash_t name, Mesh*& mesh);
	bool cacheMesh(hash_t name);
	void uncacheMesh(hash_t name);
	void uncacheAllMeshesBut(const set<hash_t>& keep);
	void defineItem(int32_t id, ItemDefinition* item);
	ItemDefinition* getItemDefinition(int32_t id);

private:
	void addResource(const File& file, InputStream* stream);
	TextureEntry* findTexture(const TextureIndex& index, TXDEntry*& txdEntry);
	void readTexture(TextureEntry* texEntry, TXDEntry* txdEntry, TXDTexture*& tex, uint8_t*& data,
			bool readData = true);
	void cacheTexture(TXDEntry* txdEntry, TextureEntry* texEntry);
	void uncacheTexture(TextureEntry* texEntry);
	void uncacheTextures(TXDEntry* txdEntry);
	bool isTextureCached(TextureEntry* texEntry);
	bool readMesh(hash_t name, Mesh*& mesh);
	bool cacheMesh(hash_t name, Mesh* mesh);

private:
	TextureMap textures;
	TextureCacheMap textureCache;
	MeshMap meshes;
	MeshCacheMap meshCache;
	ItemDefinitionMap items;

	Mutex textureMutex;
	Mutex textureCacheMutex;
	Mutex meshMutex;
};

#endif /* RESOURCEMANAGER_H_ */
