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
		delete it->second;
	}

	textureIndex.clear();


	MeshMap::iterator mit;

	for (mit = meshIndex.begin() ; mit != meshIndex.end() ; mit++) {
		delete[] mit->first;
		delete[] mit->second;
	}

	meshIndex.clear();
}


void ResourceIndex::addResource(const char* filename)
{
	if (isDirectory(filename)) {

	} else {
		GFFileType type = GFGuessFileType(filename);

		if (type == GF_TYPE_TXD) {
			TXDArchive txd(filename);
			indexTXD(&txd, filename);
		} else if (type == GF_TYPE_IMG  ||  type == GF_TYPE_DIR) {
			IMGArchive img(filename);
			IMGEntry** entries = img.getEntries();

			for (int32_t i = 0 ; i < img.getEntryCount() ; i++) {
				IMGEntry* entry = entries[i];
				GFFileType entryType = GFGuessFileType(entry->name);

				if (entryType == GF_TYPE_TXD) {
					InputStream* stream = img.gotoEntry(entry);
					TXDArchive txd(stream);
					indexTXD(&txd, filename, i);
				} else if (entryType == GF_TYPE_DFF) {
					indexDFF(filename, entry->name, i);
				}
			}
		} else if (type == GF_TYPE_DFF) {
			indexDFF(filename, filename);
		} else {
			char errmsg[2048];
			sprintf(errmsg, "Unknown resource type for file %s", filename);
			throw EngineException(errmsg, __FILE__, __LINE__);
		}
	}
}


void ResourceIndex::indexTXD(TXDArchive* txd, const char* filename, int32_t imgIndex)
{
	for (int32_t i = 0 ; i < txd->getTextureCount() ; i++) {
		TXDTexture* tex = txd->nextTexture();
		const char* name = tex->getDiffuseName();

		char* lName = new char[strlen(name)+1];
		strtolower(lName, name);

		delete tex;

		TextureEntry* entry = new TextureEntry;
		entry->filename = filename;
		entry->imgIndex = imgIndex;
		entry->txdIndex = i;

		textureIndex.insert(pair<const char*, TextureEntry*>(lName, entry));
	}
}


void ResourceIndex::indexDFF(const char* filename, const char* dffName, int32_t imgIndex)
{
	const char* fnameBegin = strrchr(dffName, '/');

	if (fnameBegin == NULL) {
		fnameBegin = dffName;
	}
	if (strlen(fnameBegin) <= 4) {
		char errmsg[2048];
		sprintf(errmsg, "Invalid mesh file name, must be *.dff: %s", fnameBegin);
		throw EngineException(errmsg, __FILE__, __LINE__);
	}

	char* meshName = new char[strlen(fnameBegin)+1];
	strtolower(meshName, fnameBegin);
	meshName[strlen(meshName)-4] = '\0';

	MeshEntry* entry = new MeshEntry;
	entry->filename = filename;
	entry->imgIndex = imgIndex;

	meshIndex.insert(pair<const char*, MeshEntry*>(meshName, entry));
}


bool ResourceIndex::gotoTexture(	const char* name, TXDArchive*& txd, TXDTexture*& texture,
									IMGArchive*& img	)
{
	TextureMap::iterator it = textureIndex.find(name);

	if (it == textureIndex.end()) {
		return false;
	}

	TextureEntry* texEntry = it->second;

	if (texEntry->imgIndex == -1) {
		txd = new TXDArchive(texEntry->filename);

		for (int32_t i = 0 ; i < texEntry->txdIndex ; i++) delete txd->nextTexture();

		texture = txd->nextTexture();
	} else {
		img = new IMGArchive(texEntry->filename);
		IMGEntry* entry = img->getEntries()[texEntry->imgIndex];
		InputStream* stream = img->gotoEntry(entry);

		txd = new TXDArchive(stream);

		for (int32_t i = 0 ; i < texEntry->txdIndex ; i++) delete txd->nextTexture();

		texture = txd->nextTexture();
	}

	return true;
}


bool ResourceIndex::getTextureHeader(const char* name, TXDTexture*& texture)
{
	IMGArchive* img = NULL;
	TXDArchive* txd = NULL;
	bool retval = gotoTexture(name, txd, texture, img);

	if (txd) {
		delete txd;
	}
	if (img) {
		delete img;
	}

	return retval;
}


bool ResourceIndex::getTexture(const char* name, TXDTexture*& texture, uint8_t*& data)
{
	IMGArchive* img = NULL;
	TXDArchive* txd = NULL;
	bool retval = gotoTexture(name, txd, texture, img);

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
	if (img) {
		delete img;
	}

	return retval;
}


bool ResourceIndex::getMesh(const char* name, DFFMesh*& mesh)
{
	MeshMap::iterator it = meshIndex.find(name);

	if (it == meshIndex.end()) {
		return false;
	}

	MeshEntry* meshEntry = it->second;

	DFFLoader dff;

	if (meshEntry->imgIndex == -1) {
		mesh = dff.loadMesh(meshEntry->filename);
	} else {
		IMGArchive img(meshEntry->filename);
		IMGEntry* entry = img.getEntries()[meshEntry->imgIndex];
		InputStream* stream = img.gotoEntry(entry);
		mesh = dff.loadMesh(stream);
	}

	return true;
}


