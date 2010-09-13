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
#include <utility>
#include <cstring>

using std::pair;


ResourceIndex::~ResourceIndex()
{
	TextureMap::iterator it;

	for (it = textureIndex.begin() ; it != textureIndex.end() ; it++) {
		delete[] it->first;
		delete it->second->file;
		delete it->second;
	}

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
			FileIterator* it = file.getIterator();
			File* entry;

			while ((entry = it->next())  !=  NULL) {
				FileContentType entryType = entry->guessContentType();

				if (entryType == CONTENT_TYPE_TXD  ||  entryType == CONTENT_TYPE_DFF) {
					addResource(*entry);
				}

				delete entry;
			}

			delete it;
		} else if (type == CONTENT_TYPE_DFF) {
			indexDFF(file);
		} /*else {
			char* errmsg = new char[strlen(file.getPath()->toString()) + 128];
			sprintf(errmsg, "Unknown resource type for file %s", file.getPath()->toString());
			EngineException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}*/
	}
}


void ResourceIndex::indexTXD(TXDArchive* txd, const File& file)
{
	for (int32_t i = 0 ; i < txd->getTextureCount() ; i++) {
		TXDTexture* tex = txd->nextTexture();
		const char* name = tex->getDiffuseName();

		char* lName = new char[strlen(name)+1];
		strtolower(lName, name);

		delete tex;

		TextureEntry* entry = new TextureEntry;
		entry->file = new File(file);
		entry->txdIndex = i;

		textureMutex.lock();
		textureIndex.insert(pair<const char*, TextureEntry*>(lName, entry));
		textureMutex.unlock();
	}
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
	meshIndex.insert(pair<const char*, MeshEntry*>(meshName, entry));
	meshMutex.unlock();
}


bool ResourceIndex::gotoTexture(const char* name, TXDArchive*& txd, TXDTexture*& texture)
{
	textureMutex.lock();

	TextureMap::iterator it = textureIndex.find(name);

	if (it == textureIndex.end()) {
		textureMutex.unlock();
		return false;
	}

	TextureEntry* texEntry = it->second;

	int txdIndex = texEntry->txdIndex;

	txd = new TXDArchive(*texEntry->file);

	textureMutex.unlock();

	for (int32_t i = 0 ; i < txdIndex ; i++) delete txd->nextTexture();
	texture = txd->nextTexture();

	return true;
}


bool ResourceIndex::getTextureHeader(const char* name, TXDTexture*& texture)
{
	TXDArchive* txd = NULL;
	bool retval = gotoTexture(name, txd, texture);

	if (txd) {
		delete txd;
	}

	return retval;
}


bool ResourceIndex::getTexture(const char* name, TXDTexture*& texture, uint8_t*& data)
{
	TXDArchive* txd = NULL;
	bool retval = gotoTexture(name, txd, texture);

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


