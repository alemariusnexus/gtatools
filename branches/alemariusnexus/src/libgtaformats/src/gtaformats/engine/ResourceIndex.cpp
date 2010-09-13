/*
 * ResourceIndex.cpp
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#include "ResourceIndex.h"
#include "../gf_filetype.h"
#include "../dff/DFFMesh.h"
#include "../dff/DFFLoader.h"
#include "EngineException.h"
#include "../util/util.h"
#include "../img/IMGArchive.h"
#include <utility>
#include <cstring>

using std::pair;


ResourceIndex::~ResourceIndex()
{
	TXDMap::iterator it;

	for (it = txdIndex.begin() ; it != txdIndex.end() ; it++) {
		delete[] it->first;

		TextureMap* map = it->second;
		TextureMap::iterator tit;

		for (tit = map->begin() ; tit != map->end() ; tit++) {
			delete[] tit->first;
			delete tit->second->file;
			delete tit->second;
		}

		delete map;
	}

	txdIndex.clear();
	textureIndex.clear();


	MeshMap::iterator mit;

	for (mit = meshIndex.begin() ; mit != meshIndex.end() ; mit++) {
		delete[] mit->first;
		delete mit->second->file;
		delete mit->second;
	}

	meshIndex.clear();
}


void ResourceIndex::addResource(const File& file)
{
	if (file.isDirectory()) {
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
			indexTXD(&txd, file);
		} else if (type == CONTENT_TYPE_IMG  ||  type == CONTENT_TYPE_DIR) {
			IMGArchive img(file);
			IMGEntry** entries = img.getEntries();

			for (int32_t i = 0 ; i < img.getEntryCount() ; i++) {
				IMGEntry* entry = entries[i];
				char* path = new char[strlen(file.getPath()->toString())+strlen(entry->name)+2];
				strcpy(path, file.getPath()->toString());
				strcat(path, "/");
				strcat(path, entry->name);

				File f(path);

				FileContentType etype = f.guessContentType();

				if (etype == CONTENT_TYPE_TXD) {
					InputStream* stream = img.gotoEntry(entry);
					TXDArchive txd(stream);
					indexTXD(&txd, f);
					delete stream;
				} else if (etype == CONTENT_TYPE_DFF) {
					indexDFF(f);
				}

				delete[] path;
			}
		} else if (type == CONTENT_TYPE_DFF) {
			indexDFF(file);
		}
	}
}


void ResourceIndex::indexTXD(TXDArchive* txd, const File& file)
{
	TextureMap* map = new TextureMap;

	textureMutex.lock();

	for (int32_t i = 0 ; i < txd->getTextureCount() ; i++) {
		TXDTexture* tex = txd->nextTexture();
		const char* name = tex->getDiffuseName();

		char* lName = new char[strlen(name)+1];
		strtolower(lName, name);

		delete tex;

		TextureEntry* entry = new TextureEntry;
		entry->file = new File(file);
		entry->txdIndex = i;

		/*TextureMap::iterator it = textureIndex.find(lName);
		if (it != textureIndex.end()) {
			printf("Texture already defined: %s\n", lName);
			printf("  Previous definition in %s\n", it->second->file->getPath()->toString());
			printf("  Now defined in %s\n", file.getPath()->toString());
		}*/

		map->insert(pair<const char*, TextureEntry*>(lName, entry));
		textureIndex.insert(pair<const char*, TextureEntry*>(lName, entry));
	}

	const char* fullTxdName = file.getPath()->getFileName();
	char* txdName = new char[strlen(fullTxdName)];
	strtolower(txdName, fullTxdName);
	*strchr(txdName, '.') = '\0';

	if (strcmp(txdName, "gta_tree_boak") == 0) {
		printf("Found!\n");
	}

	txdIndex.insert(pair<const char*, TextureMap*>(txdName, map));
	textureMutex.unlock();
}


void ResourceIndex::indexDFF(const File& file)
{
	const char* fnameBegin = file.getPath()->getFileName();

	char* meshName = new char[strlen(fnameBegin)+1];
	strtolower(meshName, fnameBegin);
	meshName[strlen(meshName)-4] = '\0';

	MeshEntry* entry = new MeshEntry;
	entry->file = new File(file);

	meshMutex.lock();

	MeshMap::iterator it = meshIndex.find(meshName);
	if (it != meshIndex.end()) {
		printf("Mesh already defined: %s\n", meshName);
		printf("  Previous definition in %s\n", it->second->file->getPath()->toString());
		printf("  Now defined in %s\n", file.getPath()->toString());
	}

	meshIndex.insert(pair<const char*, MeshEntry*>(meshName, entry));
	meshMutex.unlock();
}


bool ResourceIndex::gotoTexture(const char* name, const char* txdName, TXDArchive*& txd, TXDTexture*& texture)
{
	textureMutex.lock();

	TextureMap* map;

	if (txdName) {
		TXDMap::iterator it = txdIndex.find(txdName);

		if (it == txdIndex.end()) {
			textureMutex.unlock();
			return false;
		}

		map = it->second;
	} else {
		map = &textureIndex;
	}

	TextureMap::iterator tit = map->find(name);

	if (tit == map->end()) {
		textureMutex.unlock();
		return false;
	}

	TextureEntry* texEntry = tit->second;

	int txdIndex = texEntry->txdIndex;

	txd = new TXDArchive(*texEntry->file);

	textureMutex.unlock();

	for (int32_t i = 0 ; i < txdIndex ; i++) delete txd->nextTexture();
	texture = txd->nextTexture();

	return true;
}


bool ResourceIndex::getTextureHeader(const char* name, const char* txdName, TXDTexture*& texture)
{
	TXDArchive* txd = NULL;
	bool retval = gotoTexture(name, txdName, txd, texture);

	if (txd) {
		delete txd;
	}

	return retval;
}


bool ResourceIndex::getTexture(const char* name, const char* txdName, TXDTexture*& texture, uint8_t*& data)
{
	TXDArchive* txd = NULL;
	bool retval = gotoTexture(name, txdName, txd, texture);

	if (retval) {
		uint8_t* rawData = txd->readTextureData(texture);

		int32_t pixels = texture->getWidth() * texture->getHeight();

		switch (textureFormat) {
		case Native:
			data = rawData;
			break;

		case R8G8B8A8:
			data = new uint8_t[pixels * 4];
			texture->convert(data, rawData, MIRROR_NONE, 4, 0, 1, 2, 3);
			delete[] rawData;
			break;
		}
	}

	if (txd) {
		delete txd;
	}

	return retval;
}


bool ResourceIndex::getMesh(const char* name, DFFMesh*& mesh)
{
	meshMutex.lock();

	MeshMap::iterator it = meshIndex.find(name);

	if (it == meshIndex.end()) {
		meshMutex.unlock();
		return false;
	}

	MeshEntry* meshEntry = it->second;

	DFFLoader dff;
	mesh = dff.loadMesh(*meshEntry->file);

	meshMutex.unlock();

	return true;
}


const File* ResourceIndex::findTexture(const char* name)
{
	textureMutex.lock();

	TextureMap::iterator it = textureIndex.find(name);

	if (it == textureIndex.end()) {
		textureMutex.unlock();
		return NULL;
	}

	TextureEntry* texEntry = it->second;
	File* file = new File(*texEntry->file);

	textureMutex.unlock();

	return file;
}


const File* ResourceIndex::findMesh(const char* name)
{
	meshMutex.lock();

	MeshMap::iterator it = meshIndex.find(name);

	if (it == meshIndex.end()) {
		meshMutex.unlock();
		return NULL;
	}

	MeshEntry* entry = it->second;
	File* file = new File(*entry->file);

	meshMutex.unlock();

	return file;
}


