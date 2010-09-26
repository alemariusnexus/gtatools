/*
 * ResourceManager.cpp
 *
 *  Created on: 12.09.2010
 *      Author: alemariusnexus
 */

#include "ResourceManager.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTexture.h>
#include <gtaformats/util/util.h>
#include <gtaformats/dff/DFFLoader.h>
#include <utility>
#include <squish.h>

using std::pair;
using std::locale;
using namespace squish;



ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
	TextureCacheMap::iterator cit;

	for (cit = textureCache.begin() ; cit != textureCache.end() ; cit++) {
		TextureCacheEntry* entry = cit->second;
		glDeleteTextures(1, &entry->texture);
		delete entry;
	}

	TextureMap::iterator it;

	for (it = textures.begin() ; it != textures.end() ; it++) {
		TXDEntry* entry = it->second;
		delete entry->file;

		TextureEntryMap::iterator eit;

		for (eit = entry->textures.begin() ; eit != entry->textures.end() ; eit++) {
			TextureEntry* tex = eit->second;
			delete tex;
		}

		delete entry;
	}

	MeshMap::iterator mit;

	for (mit = meshes.begin() ; mit != meshes.end() ; mit++) {
		MeshEntry* entry = mit->second;
		delete entry->file;
	}
}


void ResourceManager::addResource(const File& file)
{
	if (file.isDirectory()  ||  file.isArchiveFile()) {
		FileIterator* it = file.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			addResource(*child);
			delete child;
		}

		delete it;
	} else {
		FileContentType type = file.guessContentType();

		if (type == CONTENT_TYPE_TXD) {
			TXDArchive txd(file);
			char* lTxdName = new char[strlen(file.getPath()->getFileName())+1];
			strtolower(lTxdName, file.getPath()->getFileName());
			*strchr(lTxdName, '.') = '\0';

			hash_t txdHash = hash(lTxdName);

			delete[] lTxdName;

			TXDEntry* txdEntry = new TXDEntry;
			txdEntry->parent = NULL;
			txdEntry->file = new File(file);

			for (int16_t i = 0 ; i < txd.getTextureCount() ; i++) {
				TXDTexture* tex = txd.nextTexture();

				char* lName = new char[strlen(tex->getDiffuseName())+1];
				strtolower(lName, tex->getDiffuseName());

				hash_t texHash = hash(lName);

				delete[] lName;

				TextureEntry* entry = new TextureEntry;
				entry->textureIndex = i;
				txdEntry->textures.insert(pair<hash_t, TextureEntry*>(texHash, entry));
				delete tex;
			}

			textures.insert(pair<hash_t, TXDEntry*>(txdHash, txdEntry));
		} else if (type == CONTENT_TYPE_DFF) {
			char* lMeshName = new char[strlen(file.getPath()->getFileName())+1];
			strtolower(lMeshName, file.getPath()->getFileName());
			*strchr(lMeshName, '.') = '\0';
			MeshEntry* entry = new MeshEntry;
			entry->file = new File(file);
			meshes.insert(pair<hash_t, MeshEntry*>(hash(lMeshName), entry));
		}
	}
}


bool ResourceManager::getTexture(hash_t texName, hash_t txdName, TXDTexture*& tex)
{
	TXDEntry* txdEntry;
	TextureEntry* texEntry = findTexture(texName, txdName, txdEntry);

	if (!tex) {
		return false;
	}

	uint8_t* nullptr;
	readTexture(texEntry, txdEntry, tex, nullptr, false);

	return true;
}


bool ResourceManager::getTexture(hash_t texName, hash_t txdName, TXDTexture*& tex, uint8_t*& data)
{
	TXDEntry* txdEntry;
	TextureEntry* texEntry = findTexture(texName, txdName, txdEntry);

	if (!texEntry) {
		return false;
	}

	readTexture(texEntry, txdEntry, tex, data, true);

	return true;
}


bool ResourceManager::cacheTexture(hash_t texName, hash_t txdName)
{
	TXDEntry* txdEntry;
	TextureEntry* texEntry = findTexture(texName, txdName, txdEntry);

	if (!texEntry) {
		return false;
	}

	cacheTexture(txdEntry, texEntry);
	return true;
}


bool ResourceManager::cacheTextures(hash_t txdName)
{
	TextureMap::iterator it = textures.find(txdName);

	if (it == textures.end()) {
		return false;
	}

	TXDEntry* entry = it->second;
	cacheTexture(entry, NULL);
	return true;
}


void ResourceManager::uncacheTexture(hash_t texName, hash_t txdName)
{
	TXDEntry* txdEntry;
	TextureEntry* texEntry = findTexture(texName, txdName, txdEntry);

	if (texEntry) {
		uncacheTexture(texEntry);
	}
}


void ResourceManager::uncacheTextures(hash_t txdName)
{
	TextureMap::iterator it = textures.find(txdName);

	if (it != textures.end()) {
		uncacheTextures(it->second);
	}
}


DFFMesh* ResourceManager::getMesh(hash_t name)
{
	MeshMap::iterator it = meshes.find(name);

	if (it == meshes.end()) {
		return NULL;
	}

	MeshEntry* entry = it->second;
	DFFLoader dff;
	DFFMesh* mesh = dff.loadMesh(*entry->file);
	return mesh;
}


GLuint ResourceManager::bindTexture(hash_t texName, hash_t txdName)
{
	TXDEntry* txdEntry;
	TextureEntry* texEntry = findTexture(texName, txdName, txdEntry);

	if (!texEntry) {
		return 0;
	}

	TextureCacheMap::iterator it = textureCache.find(texEntry);

	if (it == textureCache.end()) {
		if (cacheTexture(texName, txdName)) {
			return bindTexture(texName, txdName);
		}

		return 0;
	}

	TextureCacheEntry* entry = it->second;
	glBindTexture(GL_TEXTURE_2D, entry->texture);
	return entry->texture;
}


ResourceManager::TextureEntry* ResourceManager::findTexture(hash_t texName, hash_t txdName,
		TXDEntry*& txdEntry)
{
	TextureMap::iterator it = textures.find(txdName);

	if (it == textures.end()) {
		return NULL;
	}

	txdEntry = it->second;

	TXDEntry* txd = it->second;
	TextureEntryMap::iterator eit = txd->textures.find(texName);

	if (eit == txd->textures.end()) {
		return NULL;
	}

	return eit->second;
}


bool ResourceManager::findTextureArchive(hash_t texName, hash_t* txdName)
{
	TextureMap::iterator it;

	for (it = textures.begin() ; it != textures.end() ; it++) {
		TXDEntry* txd = it->second;
		TextureEntryMap::iterator tit = txd->textures.find(texName);

		if (tit != txd->textures.end()) {
			*txdName = it->first;
			return true;
		}
	}

	return false;
}


void ResourceManager::readTexture(TextureEntry* texEntry, TXDEntry* txdEntry, TXDTexture*& tex,
		uint8_t*& data, bool readData)
{
	TXDArchive txd(*txdEntry->file);
	for (int16_t i = 0 ; i < texEntry->textureIndex ; i++) delete txd.nextTexture();

	tex = txd.nextTexture();

	if (readData) {
		data = txd.readTextureData(tex);
	}
}


void ResourceManager::cacheTexture(TXDEntry* txdEntry, TextureEntry* texEntry)
{
	if (texEntry) {
		TXDTexture* tex;
		uint8_t* data;
		readTexture(texEntry, txdEntry, tex, data, true);

		GLuint glTex;
		glGenTextures(1, &glTex);
		glBindTexture(GL_TEXTURE_2D, glTex);

		GLint uWrap = GL_REPEAT;
		GLint vWrap = GL_REPEAT;

		switch (tex->getUWrapFlags()) {
		case TXD_WRAP_NONE:
			uWrap = GL_REPEAT;
			break;
		case TXD_WRAP_WRAP:
			uWrap = GL_REPEAT;
			break;
		case TXD_WRAP_MIRROR:
			uWrap = GL_MIRRORED_REPEAT;
			break;
		case TXD_WRAP_CLAMP:
			uWrap = GL_CLAMP_TO_EDGE;
			break;
		}

		switch (tex->getVWrapFlags()) {
		case TXD_WRAP_NONE:
			vWrap = GL_REPEAT;
			break;
		case TXD_WRAP_WRAP:
			vWrap = GL_REPEAT;
			break;
		case TXD_WRAP_MIRROR:
			vWrap = GL_MIRRORED_REPEAT;
			break;
		case TXD_WRAP_CLAMP:
			vWrap = GL_CLAMP_TO_EDGE;
			break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, vWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		int16_t w = tex->getWidth();
		int16_t h = tex->getHeight();

		TXDCompression compr = tex->getCompression();

		uint8_t* pixels = new uint8_t[w*h*4];
		tex->convert(pixels, data, MIRROR_NONE, 4, 0, 1, 2, 3);
		delete[] data;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		delete[] pixels;

		delete tex;

		TextureCacheEntry* entry = new TextureCacheEntry;
		entry->texture = glTex;
		textureCache.insert(pair<TextureEntry*, TextureCacheEntry*>(texEntry, entry));
	} else {
		TextureEntryMap::iterator it;

		for (it = txdEntry->textures.begin() ; it != txdEntry->textures.end() ; it++) {
			cacheTexture(txdEntry, it->second);
		}
	}
}


void ResourceManager::uncacheTexture(TextureEntry* texEntry)
{
	TextureCacheMap::iterator it = textureCache.find(texEntry);

	if (it == textureCache.end()) {
		return;
	}

	TextureCacheEntry* entry = it->second;
	glDeleteTextures(1, &entry->texture);
	delete entry;
	textureCache.erase(it);
}


void ResourceManager::uncacheTextures(TXDEntry* txdEntry)
{
	TextureEntryMap::iterator it;

	for (it = txdEntry->textures.begin() ; it != txdEntry->textures.end() ; it++) {
		TextureEntry* entry = it->second;
		uncacheTexture(entry);
	}
}


bool ResourceManager::isTextureCached(TextureEntry* texEntry)
{
	return textureCache.find(texEntry) != textureCache.end();
}
