/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "ResourceManager.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTexture.h>
#include <gtaformats/img/IMGArchive.h>
#include <gtaformats/util/strutil.h>
#include <gtaformats/dff/DFFLoader.h>
#include <gtaformats/ide/IDEReader.h>
#include <gtaformats/ide/IDEStatement.h>
#include <gtaformats/ide/IDEStaticObject.h>
#include <gtaformats/col/COLLoader.h>
#include <utility>
#include "EngineException.h"
#include "COLMeshConverter.h"
#include <iostream>

using std::pair;
using std::locale;
using std::cerr;
using std::endl;




ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
	textureCache.clear();

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

	meshCache.clear();

	MeshMap::iterator mit;

	for (mit = meshes.begin() ; mit != meshes.end() ; mit++) {
		MeshEntry* entry = mit->second;
		delete entry->file;
		delete entry;
	}
}


void ResourceManager::addResource(const File& file, void (*callback)())
{
	if (file.isDirectory()) {
		FileIterator* it = file.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			addResource(*child, callback);
			delete child;
		}

		delete it;

		if (callback)
			callback();
	} else if (file.isArchiveFile()) {
		IMGArchive img(file);
		const IMGEntry* entries = img.getEntries();

		for (int32_t i = 0 ; i < img.getEntryCount() ; i++) {
			const IMGEntry* entry = &entries[i];

			FilePath* path = new FilePath(*file.getPath(), entry->name);
			File child(path, true);

			istream* stream = img.gotoEntry(entry);
			addResource(child, stream, callback);
			delete stream;
		}
	} else {
		istream* stream = NULL;

		FileContentType type = file.guessContentType();

		if (type == CONTENT_TYPE_TXD/*  ||  type == CONTENT_TYPE_COL*/) {
			stream = file.openInputStream(istream::binary);
		} else if (type == CONTENT_TYPE_IDE) {
			stream = file.openInputStream();
		}

		addResource(file, stream, callback);

		if (stream) {
			delete stream;
		}
	}
}


void ResourceManager::addResource(const File& file, istream* stream, void (*callback)())
{
	FileContentType type = file.guessContentType();

	if (type == CONTENT_TYPE_TXD) {
		try {
			TXDArchive txd(stream);
			char* lTxdName = new char[strlen(file.getPath()->getFileName())+1];
			strtolower(lTxdName, file.getPath()->getFileName());
			*strchr(lTxdName, '.') = '\0';

			hash_t txdHash = Hash(lTxdName);

			delete[] lTxdName;

			TXDEntry* txdEntry = new TXDEntry;
			txdEntry->parent = NULL;
			txdEntry->file = new File(file);

			for (int16_t i = 0 ; i < txd.getTextureCount() ; i++) {
				TXDTexture* tex = txd.nextTexture();

				char* lName = new char[strlen(tex->getDiffuseName())+1];
				strtolower(lName, tex->getDiffuseName());

				hash_t texHash = Hash(lName);

				delete[] lName;

				TextureEntry* entry = new TextureEntry;
				entry->textureIndex = i;
				txdEntry->textures.insert(pair<hash_t, TextureEntry*>(texHash, entry));
				delete tex;
			}

			textureMutex.lock();
			textures.insert(pair<hash_t, TXDEntry*>(txdHash, txdEntry));
			textureMutex.unlock();
		} catch (TXDException ex) {
			cerr << "WARNING: Unable to index TXD file " << file.getPath()->toString() << ": " << ex.what()
					<< endl;
		} catch (...) {
			cerr << "WARNING: Unable to index TXD file (unknown exception) " << file.getPath()->toString()
					<< endl;
		}
	} else if (type == CONTENT_TYPE_DFF) {
		/*char name[20];
		char lname[20];

		InputStream::streampos colStart = stream->tell();

		COLLoader col;
		while (col.loadModelName(stream, name)) {
			strtolower(lname, name);
			MeshEntry* entry = new MeshEntry;
			entry->file = new File(file);
			entry->colStart = colStart;
			colStart = stream->tell();
			meshMutex.lock();
			meshes.insert(pair<hash_t, MeshEntry*>(Hash(lname), entry));
			meshMutex.unlock();
		}*/

		char* lMeshName = new char[strlen(file.getPath()->getFileName())+1];
		strtolower(lMeshName, file.getPath()->getFileName());
		*strchr(lMeshName, '.') = '\0';
		MeshEntry* entry = new MeshEntry;
		entry->file = new File(file);
		meshMutex.lock();
		meshes.insert(pair<hash_t, MeshEntry*>(Hash(lMeshName), entry));
		meshMutex.unlock();
		delete[] lMeshName;
	} else if (type == CONTENT_TYPE_IDE) {
		IDEReader ide(stream, false);
		IDEStatement* stmt;

		while ((stmt = ide.readStatement())  !=  NULL) {
			switch (stmt->getType()) {
			case IDE_TYPE_STATIC_OBJECT:
				IDEStaticObject* sobj = (IDEStaticObject*) stmt;
				ItemDefinition* def = new ItemDefinition(*sobj);
				defineItem(sobj->getId(), def);
				break;
			}

			delete stmt;
		}
	} /*else if (type == CONTENT_TYPE_COL) {
		COLLoader col;
		COLModel* model;

		//while ((model = col.loadModel(stream))  !=  NULL) {
		while (true) {
			InputStream::streampos offset = stream->tell();
			model = col.loadModel(stream);
			if (!model) {
				break;
			}
			hash_t colHash = Hash(model->getName());
			delete model;
			COLEntry* entry = new COLEntry;
			entry->file = new File(file);
			entry->offset = offset;
			cols[colHash] = entry;
		}
	}*/

	if (callback)
		callback();
}


bool ResourceManager::getTexture(const TextureIndex& index, TXDTexture*& tex)
{
	TXDEntry* txdEntry;
	TextureEntry* texEntry = findTexture(index, txdEntry);

	if (!tex) {
		return false;
	}

	uint8_t* nptr;
	readTexture(texEntry, txdEntry, tex, nptr, false);

	return true;
}


bool ResourceManager::getTexture(const TextureIndex& index, TXDTexture*& tex, uint8_t*& data)
{
	TXDEntry* txdEntry;
	TextureEntry* texEntry = findTexture(index, txdEntry);

	if (!texEntry) {
		return false;
	}

	readTexture(texEntry, txdEntry, tex, data, true);

	return true;
}


bool ResourceManager::cacheTexture(const TextureIndex& index)
{
	TXDEntry* txdEntry;
	TextureEntry* texEntry = findTexture(index, txdEntry);

	if (!texEntry) {
		return false;
	}

	cacheTexture(txdEntry, texEntry);
	return true;
}


bool ResourceManager::cacheTextures(hash_t txdName)
{
	textureMutex.lock();
	TextureMap::iterator it = textures.find(txdName);

	if (it == textures.end()) {
		textureMutex.unlock();
		return false;
	}

	TXDEntry* entry = it->second;
	textureMutex.unlock();
	cacheTexture(entry, NULL);
	return true;
}


void ResourceManager::uncacheTexture(const TextureIndex& index)
{
	TXDEntry* txdEntry;
	TextureEntry* texEntry = findTexture(index, txdEntry);

	if (texEntry) {
		uncacheTexture(texEntry);
	}
}


void ResourceManager::uncacheTextures(hash_t txdName)
{
	textureMutex.lock();
	TextureMap::iterator it = textures.find(txdName);

	if (it != textures.end()) {
		textureMutex.unlock();
		uncacheTextures(it->second);
	} else {
		textureMutex.unlock();
	}
}


GLuint ResourceManager::getTexture(const TextureIndex& index)
{
	TXDEntry* txdEntry;
	TextureEntry* texEntry = findTexture(index, txdEntry);

	if (!texEntry) {
		return 0;
	}

	textureCacheMutex.lock();
	TextureCacheEntry* entry = textureCache[texEntry];

	if (!entry) {
		texCacheMisses++;
		textureCacheMutex.unlock();
		if (cacheTexture(index)) {
			return bindTexture(index);
		}

		return 0;
	}

	texCacheHits++;

	textureCacheMutex.unlock();
	return entry->texture;
}


GLuint ResourceManager::bindTexture(const TextureIndex& index)
{
	GLuint tex = getTexture(index);

	if (tex != 0) {
		glBindTexture(GL_TEXTURE_2D, tex);
	}

	return tex;
}


ResourceManager::TextureEntry* ResourceManager::findTexture(const TextureIndex& index, TXDEntry*& txdEntry)
{
	textureMutex.lock();
	TextureMap::iterator it = textures.find(index.getTXDHash());

	if (it == textures.end()) {
		textureMutex.unlock();
		return NULL;
	}

	txdEntry = it->second;
	TXDEntry* txd = it->second;
	TextureEntryMap::iterator eit = txd->textures.find(index.getTextureHash());

	if (eit == txd->textures.end()) {
		textureMutex.unlock();
		return NULL;
	}

	TextureEntry* entry = eit->second;
	textureMutex.unlock();

	return entry;
}


bool ResourceManager::findTextureArchive(hash_t texName, hash_t* txdName)
{
	TextureMap::iterator it;

	textureMutex.lock();
	for (it = textures.begin() ; it != textures.end() ; it++) {
		TXDEntry* txd = it->second;
		TextureEntryMap::iterator tit = txd->textures.find(texName);

		if (tit != txd->textures.end()) {
			*txdName = it->first;
			textureMutex.unlock();
			return true;
		}
	}

	textureMutex.unlock();
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

		GLint magFilter, minFilter;

		switch (tex->getFilterFlags()) {
		case TXD_FILTER_LINEAR:
			magFilter = GL_LINEAR;
			minFilter = GL_LINEAR;
			break;

		case TXD_FILTER_NEAREST:
			magFilter = GL_NEAREST;
			minFilter = GL_NEAREST;
			break;

		case TXD_FILTER_MIP_NEAREST:
			magFilter = GL_NEAREST;
			minFilter = GL_NEAREST_MIPMAP_NEAREST;
			break;

		case TXD_FILTER_MIP_LINEAR:
			magFilter = GL_NEAREST;
			minFilter = GL_NEAREST_MIPMAP_LINEAR;
			break;

		case TXD_FILTER_LINEAR_MIP_NEAREST:
			magFilter = GL_LINEAR;
			minFilter = GL_LINEAR_MIPMAP_NEAREST;
			break;

		case TXD_FILTER_LINEAR_MIP_LINEAR:
			magFilter = GL_LINEAR;
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
			break;

		case TXD_FILTER_NONE:
			// Don't know what it is
			magFilter = GL_LINEAR;
			minFilter = GL_LINEAR;
			break;

		default:
			// Unknown filter, use linear
			magFilter = GL_LINEAR;
			minFilter = GL_LINEAR;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, vWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, tex->getMipmapCount()-1);

		bool mipmapsIncluded = (tex->getRasterFormatExtension() & TXD_FORMAT_EXT_MIPMAP) != 0;
		bool mipmapsAuto = (tex->getRasterFormatExtension() & TXD_FORMAT_EXT_AUTO_MIPMAP) != 0;
		int numIncludedMipmaps = mipmapsIncluded ? tex->getMipmapCount() : 1;

		if (mipmapsAuto  &&  !glewIsSupported("GL_VERSION_3_0")) {
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		}

		int16_t w = tex->getWidth();
		int16_t h = tex->getHeight();

		TXDCompression compr = tex->getCompression();
		int size = 0;

		if ((compr == DXT1  ||  compr == DXT3)  &&  glewIsSupported("GL_EXT_texture_compression_s3tc")) {
			if (compr == DXT1) {
				//glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, w, h, 0, (w*h)/2, data);
				//size += (w*h)/2;

				uint8_t* dataStart = data;

				int16_t mipW = w;
				int16_t mipH = h;

				for (int i = 0 ; i < numIncludedMipmaps ; i++) {
					if (mipW < 4  ||  mipH < 4) {
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i-1);
						break;
					}

					glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
							mipW, mipH, 0, (mipW*mipH)/2, data);
					size += (mipW*mipH)/2;
					data += (mipW*mipH)/2;
					mipW /= 2;
					mipH /= 2;
				}

				delete[] dataStart;
			} else {
				//glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, w, h, 0, w*h, data);

				uint8_t* dataStart = data;

				int16_t mipW = w;
				int16_t mipH = h;

				for (int i = 0 ; i < numIncludedMipmaps ; i++) {
					if (mipW < 4  ||  mipH < 4) {
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i-1);
						break;
					}

					glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
							mipW, mipH, 0, mipW*mipH, data);
					size += mipW*mipH;
					data += mipW*mipH;
					mipW /= 2;
					mipH /= 2;
				}

				delete[] dataStart;
			}
		} else {
			uint8_t* dataStart = data;

			int16_t mipW = w;
			int16_t mipH = h;

			for (int i = 0 ; i < numIncludedMipmaps ; i++) {
				if (mipW < 1  ||  mipH < 1) {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i-1);
					break;
				}

				uint8_t* pixels = new uint8_t[mipW*mipH*4];
				tex->convert(pixels, data, i, MIRROR_NONE, 4, 0, 1, 2, 3);

				glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, mipW, mipH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
				size += mipW*mipH*4;
				data += mipW*mipH*4;

				delete[] pixels;

				mipW /= 2;
				mipH /= 2;
			}

			delete[] dataStart;
		}

		if (	(tex->getRasterFormatExtension() & TXD_FORMAT_EXT_AUTO_MIPMAP) != 0
				&&  glewIsSupported("GL_VERSION_3_0")
		) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		delete tex;

		TextureCacheEntry* entry = new TextureCacheEntry;
		entry->texture = glTex;
		textureCacheMutex.lock();

		if (!textureCache.insert(texEntry, entry, size)) {
			/*char* errmsg = new char[128];
			sprintf(errmsg, "Unable to cache texture of size %d: Too big for the cache (capacity: %d)!",
					size, textureCache.getCapacity());
			EngineException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;*/
		}

		textureCacheMutex.unlock();
	} else {
		TextureEntryMap::iterator it;

		for (it = txdEntry->textures.begin() ; it != txdEntry->textures.end() ; it++) {
			cacheTexture(txdEntry, it->second);
		}
	}
}


void ResourceManager::uncacheTexture(TextureEntry* texEntry)
{
	textureCacheMutex.lock();
	textureCache.remove(texEntry);
	textureCacheMutex.unlock();
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
	textureCacheMutex.lock();
	bool res = textureCache[texEntry] != NULL;
	textureCacheMutex.unlock();
	return res;
}


bool ResourceManager::getMesh(hash_t name, Mesh*& mesh)
{
	MeshCacheEntry* entry = meshCache[name];

	if (entry) {
		mesh = entry->mesh;
		meshCacheHits++;
		return true;
	}

	meshCacheMisses++;

	if (!readMesh(name, mesh)) {
		return false;
	}

	cacheMesh(name, mesh);

	return true;
}


bool ResourceManager::cacheMesh(hash_t name)
{
	Mesh* mesh;
	readMesh(name, mesh);
	return cacheMesh(name, mesh);
}


bool ResourceManager::cacheMesh(hash_t name, Mesh* mesh)
{
	MeshCacheEntry* entry = new MeshCacheEntry;
	entry->mesh = mesh;

	int size = mesh->guessSize();
	if (!meshCache.insert(name, entry, size)) {
		char* errmsg = new char[128];
		sprintf(errmsg, "Unable to cache mesh of size %d: Too big for the cache (capacity: %d)!",
				size, meshCache.getCapacity());
		EngineException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

    return true;
	//meshCache[name] = entry;
}


void ResourceManager::uncacheMesh(hash_t name)
{
	meshCache.remove(name);
	/*MeshCacheMap::iterator it = meshCache.find(name);

	if (it == meshCache.end()) {
		return;
	}

	MeshCacheEntry* entry = it->second;
	delete entry->mesh;
	delete entry;
	meshCache.erase(it);*/
}


bool ResourceManager::readMesh(hash_t name, Mesh*& mesh)
{
	MeshMap::iterator it = meshes.find(name);

	//printf("Reading in %d\n", meshes.size());

	if (it == meshes.end()) {
		return false;
	}

	File* file = it->second->file;

	DFFLoader dff;
	DFFMesh* dffMesh = dff.loadMesh(*file);
	if (dffMesh->getGeometryCount() <= 0) {
		printf("Geometry count <= 0!\n");
		exit(1337);
	}
	DFFGeometry* geom = dffMesh->getGeometry((uint32_t) 0);

	mesh = new Mesh(*geom);

	delete dffMesh;

	/*COLLoader col;
	InputStream* stream = file->openStream(STREAM_BINARY);
	stream->seek(it->second->colStart, InputStream::STREAM_SEEK_BEGIN);
    COLModel* model = col.loadModel(stream);
    delete stream;
    COLMeshConverter conv;
    mesh = conv.convert(*model);
    delete model;*/

	return true;
}


void ResourceManager::defineItem(int32_t id, ItemDefinition* item)
{
	items[id] = item;
}


ItemDefinition* ResourceManager::getItemDefinition(int32_t id)
{
	ItemDefinitionMap::iterator it = items.find(id);

	if (it == items.end()) {
		return NULL;
	}

	return it->second;
}


/*bool ResourceManager::getCollisionModel(hash_t colHash, COLModel*& model)
{
	COLMap::iterator it = cols.find(colHash);

	if (it == cols.end()) {
		return false;
	}

	COLEntry* entry = it->second;
	COLLoader col;
	InputStream* stream = entry->file->openStream(STREAM_BINARY);
	stream->seek(entry->offset, InputStream::STREAM_SEEK_BEGIN);
	model = col.loadModel(stream);
	delete stream;
	return true;
}


bool ResourceManager::getCollisionModel(hash_t colHash, Mesh*& mesh)
{

}*/
