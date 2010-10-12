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
#include <map>
#include <locale>
#include <GL/glew.h>
#include <cstdio>

using std::map;
using std::collate;
using std::locale;
using std::use_facet;



class ResourceManager {
public:
	typedef long hash_t;

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
		//TXDTexture* texture;
		//uint8_t* data;
		GLuint texture;
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

public:
	ResourceManager();
	~ResourceManager();
	void addResource(const File& file);
	hash_t hash(const char* name) { return use_facet< collate<char> >(locale()).hash(name, name+strlen(name)); }
	GLuint bindTexture(hash_t texName, hash_t txdName);
	bool getTexture(hash_t texName, hash_t txdName, TXDTexture*& tex);
	bool getTexture(hash_t texName, hash_t txdName, TXDTexture*& tex, uint8_t*& data);
	bool cacheTexture(hash_t texName, hash_t txdName);
	bool cacheTextures(hash_t txdName);
	void uncacheTexture(hash_t texName, hash_t txdName);
	void uncacheTextures(hash_t txdName);
	DFFMesh* getMesh(hash_t name);
	bool findTextureArchive(hash_t texName, hash_t* txdName);

private:
	void addResource(const File& file, InputStream* stream);
	TextureEntry* findTexture(hash_t texName, hash_t txdName, TXDEntry*& txdEntry);
	void readTexture(TextureEntry* texEntry, TXDEntry* txdEntry, TXDTexture*& tex, uint8_t*& data,
			bool readData = true);
	void cacheTexture(TXDEntry* txdEntry, TextureEntry* texEntry);
	void uncacheTexture(TextureEntry* texEntry);
	void uncacheTextures(TXDEntry* txdEntry);
	bool isTextureCached(TextureEntry* texEntry);

private:
	TextureMap textures;
	TextureCacheMap textureCache;
	MeshMap meshes;
};

#endif /* RESOURCEMANAGER_H_ */
