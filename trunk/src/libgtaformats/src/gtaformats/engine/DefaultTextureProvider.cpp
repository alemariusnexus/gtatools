/*
 * DefaultTextureProvider.cpp
 *
 *  Created on: 14.03.2010
 *      Author: alemariusnexus
 */

#include "DefaultTextureProvider.h"
#include "../gf_filetype.h"
#include "../gtatxd.h"
#include "../gtaimg.h"
#include <utility>
#include <fstream>

using std::pair;
using std::ifstream;


void DefaultTextureProvider::addResource(const char* file)
{
	char* fileCpy = new char[strlen(file)+1];
	strcpy(fileCpy, file);

	GFFileType type = GFGuessFileType(fileCpy);

	if (type == GF_TYPE_TXD) {
		ifstream stream(fileCpy, ifstream::in | ifstream::binary);
		TXDArchive txd(&stream);

		for (int16_t i = 0 ; i < txd.getTextureCount() ; i++) {
			TXDTexture* tex = txd.nextTexture();
			TextureEntry* entry = new TextureEntry;
			entry->file = fileCpy;
			entry->texIndex = i;
			entry->imgEntryIndex = -1;
			char* texName = new char[strlen(tex->getDiffuseName())+1];
			strcpy(texName, tex->getDiffuseName());
			textureMap.insert(pair<const char*, const TextureEntry*>(texName, entry));
			delete tex;
		}
	} else if (type == GF_TYPE_DIR  ||  type == GF_TYPE_IMG) {
		IMGArchive img(fileCpy);
		IMGEntry** entries = img.getEntries();

		for (int32_t i = 0 ; i < img.getEntryCount() ; i++) {
			IMGEntry* entry = entries[i];

			if (strcmp(entry->name + strlen(entry->name)-4, ".txd") == 0) {
				istream* stream = img.gotoEntry(entry);

				TXDArchive txd(stream);

				for (int16_t j = 0 ; j < txd.getTextureCount() ; j++) {
					TXDTexture* tex = txd.nextTexture();
					TextureEntry* entry = new TextureEntry;
					entry->file = fileCpy;
					entry->texIndex = j;
					entry->imgEntryIndex = i;
					char* texName = new char[strlen(tex->getDiffuseName())+1];
					strcpy(texName, tex->getDiffuseName());
					textureMap.insert(pair<const char*, TextureEntry*>(texName, entry));
					delete tex;
				}
			}
		}
	}
}


bool DefaultTextureProvider::findTexture(	const char* texName, TXDTexture*& texture, uint8_t*& data,
													int16_t& width, int16_t& height						)
{
	TextureMap::const_iterator it = textureMap.find(texName);

	if (it == textureMap.end()) {
		return false;
	}

	const TextureEntry* texEntry = it->second;

	if (texEntry->imgEntryIndex == -1) {
		ifstream stream(texEntry->file, ifstream::in | ifstream::binary);
		TXDArchive txd(&stream);

		for (int16_t i = 0 ; i < texEntry->texIndex ; i++)
			txd.nextTexture();

		TXDTexture* tex = txd.nextTexture();
		width = tex->getWidth();
		height = tex->getHeight();
		data = txd.readTextureData(tex);
		texture = tex;
		//data = new uint8_t[width * height * 4];
		//tex->convert(data, rawData, MIRROR_NONE, 4, 0, 1, 2, 3);
		//delete[] rawData;
	} else {
		IMGArchive img(texEntry->file);

		IMGEntry* entry = img.getEntries()[texEntry->imgEntryIndex];
		istream* stream = img.gotoEntry(entry);

		TXDArchive txd(stream);

		for (int16_t i = 0 ; i < texEntry->texIndex ; i++)
			txd.nextTexture();

		TXDTexture* tex = txd.nextTexture();
		width = tex->getWidth();
		height = tex->getHeight();
		data = txd.readTextureData(tex);
		texture = tex;
		//data = new uint8_t[width * height * 4];
		//tex->convert(data, rawData, MIRROR_NONE, 4, 0, 1, 2, 3);
		//delete[] rawData;
	}

	return true;
}



