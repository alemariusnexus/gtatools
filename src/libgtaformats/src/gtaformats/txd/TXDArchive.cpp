/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "TXDArchive.h"
#include "TXDException.h"
#include <utility>
#include "../util/strutil.h"
#include <algorithm>
#include "../gta.h"

using std::pair;
using std::find;




struct TextureNativeStruct
{
	int32_t platform;
	int16_t filterFlags;
	int8_t vWrap;
	int8_t uWrap;
	char diffuseName[32];
	char alphaName[32];
	int32_t rasterFormat;
	int32_t alphaOrCompression;
	int16_t width;
	int16_t height;
	int8_t bpp;
	int8_t mipmapCount;
	int8_t rasterType;
	int8_t compressionOrAlpha;
};



TXDArchive::TXDArchive()
		: texDict(new RWSection(RW_SECTION_TEXTUREDICTIONARY, RW_VERSION_GTASA))
{
	RWSection* txdSect = new RWSection(RW_SECTION_STRUCT, texDict);
	int16_t* txdSectData = new int16_t[2];
	txdSectData[0] = 0; // Texture count
	txdSectData[1] = 0; // Unknown
	txdSect->setData((uint8_t*) txdSectData, 4);
	RWSection* txdExtSect = new RWSection(RW_SECTION_EXTENSION, texDict);
	txdExtSect->setData(new uint8_t[0], 0); // Empty
}


TXDArchive::TXDArchive(istream* stream)
		: texDict(RWSection::readSection(stream))
{
	init(stream);
}


TXDArchive::TXDArchive(const File& file)
{
	istream* stream = file.openInputStream(istream::binary);
	texDict = RWSection::readSection(stream);
	init(stream);
	delete stream;
}


TXDArchive::~TXDArchive()
{
	delete texDict;

	for (TextureIterator it = texHeaders.begin() ; it != texHeaders.end() ; it++) {
		delete *it;
	}
}


void TXDArchive::init(istream* stream)
{
	if (!texDict) {
		throw TXDException("TXD file does not contain any valid section. Is it really TXD?",
				__FILE__, __LINE__);
	}
	if (texDict->getID() != RW_SECTION_TEXTUREDICTIONARY) {
		throw TXDException("TXD file doesn't start with RW_SECTION_TEXTUREDICTIONARY. Is it really TXD?",
				__FILE__, __LINE__);
	}

	RWSection::ChildIterator it = texDict->getChildBegin();

	while ((it = texDict->nextChild(RW_SECTION_TEXTURENATIVE, it))  !=  texDict->getChildEnd()) {
		RWSection* texNative = *it;
		RWSection* texNativeStruct = texNative->getChild(RW_SECTION_STRUCT);
		TextureNativeStruct& header = *((TextureNativeStruct*) texNativeStruct->getData());

		TXDCompression compr = NONE;
		bool alpha;

		if (header.platform == 0x325350) {
			// 0x325350 = "PS2"
			throw TXDException("PS2 format of TXD is currently not supported!");
		} else if (header.platform == 9) {
			char* fourCC = (char*) &header.alphaOrCompression;
			if (fourCC[0] == 'D'  &&  fourCC[1] == 'X'  &&  fourCC[2] == 'T') {
				if (fourCC[3] == '1') {
					compr = DXT1;
				} else if (fourCC[3] == '3') {
					compr = DXT3;
				}
			} else if (fourCC[0] == 'P'  &&  fourCC[1] == 'V'  &&  fourCC[2] == 'R') {
				if (fourCC[3] == '2') {
					compr = PVRTC2;
				} else if (fourCC[3] == '4') {
					compr = PVRTC4;
				}
			}

			alpha = (header.compressionOrAlpha == 9  ||  header.compressionOrAlpha == 1);
		} else {
			if (header.compressionOrAlpha == 1) {
				compr = DXT1;
			} else if (header.compressionOrAlpha == 3) {
				compr = DXT3;
			} else if (header.compressionOrAlpha == 50) {
				compr = PVRTC2;
			} else if (header.compressionOrAlpha == 51) {
				compr = PVRTC4;
			}

			alpha = (header.alphaOrCompression == 1);
		}

		TXDTextureHeader* tex = new TXDTextureHeader(header.diffuseName, header.rasterFormat, compr,
				header.width, header.height);
		tex->setAlphaChannel(alpha);
		tex->setAlphaName(header.alphaName);
		tex->setBytesPerPixel(header.bpp/8);
		tex->setFilterFlags(header.filterFlags);
		tex->setMipmapCount(header.mipmapCount);
		tex->setWrappingFlags(header.uWrap, header.vWrap);

		texHeaders.push_back(tex);

		texNativeMap.insert(pair<hash_t, RWSection*>(LowerHash(header.diffuseName), texNative));

		it++;
	}
}


uint8_t* TXDArchive::getTextureData(TXDTextureHeader* header)
{
	hash_t hash = LowerHash(header->getDiffuseName());
	TexNativeMap::iterator it = texNativeMap.find(hash);

	if (it == texNativeMap.end()) {
		return NULL;
	}

	RWSection* texNative = it->second;
	RWSection* texNativeStruct = texNative->getChild(RW_SECTION_STRUCT);

	uint8_t* texData = new uint8_t[header->computeDataSize()];
	uint8_t* texDataStart = texData;
	uint8_t* inData = texNativeStruct->getData()+88;

	if ((header->getRasterFormatExtension() & RasterFormatEXTPAL4)  !=  0) {
		memcpy(texData, inData, 16*4);
		texData += 16*4;
		inData += 16*4;
	} else if ((header->getRasterFormatExtension() & RasterFormatEXTPAL8)  !=  0) {
		memcpy(texData, inData, 256*4);
		texData += 256*4;
		inData += 256*4;
	}

	for (int i = 0 ; i < header->getMipmapCount() ; i++) {
		int32_t size = *((int32_t*) inData);
		memcpy(texData, inData+4, size);
		texData += size;
		inData += size+4;
	}

	return texDataStart;
}


void TXDArchive::applyTextureHeader(TXDTextureHeader* header)
{
	hash_t hash = LowerHash(header->getDiffuseName());
	TexNativeMap::iterator it = texNativeMap.find(hash);

	RWSection* texNative = it->second;
	RWSection* texNativeStruct = texNative->getChild(RW_SECTION_STRUCT);
	TextureNativeStruct& nativeHeader = *((TextureNativeStruct*) texNativeStruct->getData());

	nativeHeader.platform = 9;
	nativeHeader.filterFlags = header->getFilterFlags();
	nativeHeader.vWrap = header->getVWrapFlags();
	nativeHeader.uWrap = header->getUWrapFlags();
	strcpy(nativeHeader.diffuseName, header->getDiffuseName());
	strcpy(nativeHeader.alphaName, header->getAlphaName());
	nativeHeader.rasterFormat = header->getFullRasterFormat();
	nativeHeader.width = header->getWidth();
	nativeHeader.height = header->getHeight();
	nativeHeader.bpp = header->getBytesPerPixel() * 8;
	nativeHeader.mipmapCount = header->getMipmapCount();
	nativeHeader.rasterType = 0x4;

	if (nativeHeader.platform == 9) {
		char* dxtFourCC = (char*) &nativeHeader.alphaOrCompression;

		switch (header->getCompression()) {
		case DXT1:
			strncpy(dxtFourCC, "DXT1", 4);
			break;
		case DXT3:
			strncpy(dxtFourCC, "DXT3", 4);
			break;
		case PVRTC2:
			strncpy(dxtFourCC, "PVR2", 4);
			break;
		case PVRTC4:
			strncpy(dxtFourCC, "PVR4", 4);
			break;
		case NONE:
			if (header->hasAlphaChannel()) {
				*((int32_t*) dxtFourCC) = 21;
			} else {
				*((int32_t*) dxtFourCC) = 22;
			}
			break;
		}

		//nativeHeader.compressionOrAlpha = (header->hasAlphaChannel() ? 1 : 0);

		if (header->getCompression() == NONE) {
			nativeHeader.compressionOrAlpha = (header->hasAlphaChannel() ? 1 : 0);
		} else {
			nativeHeader.compressionOrAlpha = (header->hasAlphaChannel() ? 9 : 8);
		}
	} else {
		nativeHeader.alphaOrCompression = (header->hasAlphaChannel() ? 1 : 0);

		switch (header->getCompression()) {
		case DXT1:
			nativeHeader.compressionOrAlpha = 1;
			break;
		case DXT3:
			nativeHeader.compressionOrAlpha = 3;
			break;
		case PVRTC2:
			nativeHeader.compressionOrAlpha = 50;
			break;
		case PVRTC4:
			nativeHeader.compressionOrAlpha = 51;
			break;
		case NONE:
			nativeHeader.compressionOrAlpha = 0;
			break;
		}
	}
}


void TXDArchive::setTextureData(TXDTextureHeader* header, uint8_t* data)
{
	hash_t hash = LowerHash(header->getDiffuseName());
	TexNativeMap::iterator it = texNativeMap.find(hash);

	RWSection* texNative = it->second;
	RWSection* texNativeStruct = texNative->getChild(RW_SECTION_STRUCT);

	int32_t dataSize = header->computeDataSize() + 88 + 4*header->getMipmapCount();
	uint8_t* nativeData = new uint8_t[dataSize];

	memcpy(nativeData, texNativeStruct->getData(), 88);
	uint8_t* nativeDataStart = nativeData;

	nativeData += 88;

	for (int8_t i = 0 ; i < header->getMipmapCount() ; i++) {
		int32_t mipSize = header->computeMipmapDataSize(i);
		memcpy(nativeData, &mipSize, 4);
		memcpy(nativeData+4, data, mipSize);
		nativeData += mipSize+4;
		data += mipSize;
	}

	delete[] texNativeStruct->getData();
	texNativeStruct->setData(nativeDataStart, dataSize);
}


void TXDArchive::rename(TXDTextureHeader* header, const char* name)
{
	hash_t hash = LowerHash(header->getDiffuseName());
	TexNativeMap::iterator it = texNativeMap.find(hash);
	header->setDiffuseName(name);

	if (it != texNativeMap.end()) {
		RWSection* sect = it->second;
		texNativeMap.erase(it);
		texNativeMap.insert(pair<hash_t, RWSection*>(LowerHash(name), sect));
	}
}


void TXDArchive::addTexture(TXDTextureHeader* header, uint8_t* data)
{
	RWSection* texNativeSect = new RWSection(RW_SECTION_TEXTURENATIVE, 0);
	RWSection* texNativeStructSect = new RWSection(RW_SECTION_STRUCT, texNativeSect);
	RWSection* texNativeExtSect = new RWSection(RW_SECTION_EXTENSION, texNativeSect);
	texNativeExtSect->setData(new uint8_t[0], 0); // Empty

	// Values will be set by applyTextureHeader and data is added by setTextureData
	texNativeStructSect->setData(new uint8_t[88], 88);

	RWSection::ChildIterator it = texDict->getLastChildIterator(RW_SECTION_TEXTURENATIVE);

	if (it == texDict->getChildEnd()) {
		it = texDict->getChildBegin();
		it++;
	} else {
		it++;
	}

	texDict->insertChild(it, texNativeSect);
	texNativeSect->setVersion(texDict->getVersion());

	texHeaders.push_back(header);
	texNativeMap.insert(pair<hash_t, RWSection*>(LowerHash(header->getDiffuseName()), texNativeSect));
	applyTextureHeader(header);

	if (data) {
		setTextureData(header, data);
	}

	applyTextureCountChange();
}


void TXDArchive::removeTexture(TXDTextureHeader* header)
{
	hash_t hash = LowerHash(header->getDiffuseName());
	TexNativeMap::iterator it = texNativeMap.find(hash);

	RWSection* texNative = it->second;
	texDict->removeChild(texNative);
	delete texNative;
	texNativeMap.erase(it);

	for (TextureIterator it = texHeaders.begin() ; it != texHeaders.end() ; it++) {
		TXDTextureHeader* storedHeader = *it;

		if (LowerHash(storedHeader->getDiffuseName()) == hash) {
			delete storedHeader;
			break;
		}
	}

	applyTextureCountChange();
}


void TXDArchive::applyTextureCountChange()
{
	RWSection* txdStruct = texDict->getChild(RW_SECTION_STRUCT);
	*((int16_t*) txdStruct->getData()) = texHeaders.size();
}


void TXDArchive::write(ostream* stream)
{
	texDict->write(stream);
}


void TXDArchive::write(const File& file)
{
	texDict->write(file);
}


