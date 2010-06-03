/*
 * OpenGLResourceManager.cpp
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#include "OpenGLResourceManager.h"
#include "EngineException.h"
#include <utility>
#include <iostream>
#include <cstdlib>
#include "../util/util.h"

using std::pair;
using std::cerr;
using std::endl;


OpenGLResourceManager::OpenGLResourceManager(bool cacheTextureData)
		: cacheTextureData(cacheTextureData)
{
}


OpenGLResourceManager::~OpenGLResourceManager()
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


GLuint OpenGLResourceManager::generateTexture(TXDTexture* texture, uint8_t* data)
{
	GLenum format;

	switch (textureFormat) {
	case Native:
		cerr << "OpenGLResourceManager currently does not support native texture formats!" << endl;
		exit(0);
		break;

	case R8G8B8A8:
		format = GL_RGBA;
		break;
	}

	int16_t width = texture->getWidth();
	int16_t height = texture->getHeight();

	GLuint texID = 0;
	glGenTextures(1, &texID);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
			0, format, GL_UNSIGNED_BYTE, data);

	return texID;
}


void OpenGLResourceManager::cacheTexture(TXDTexture* texture, uint8_t* data)
{
	char* lName = new char[strlen(texture->getDiffuseName())+1];
	strtolower(lName, texture->getDiffuseName());

	GLuint texID = generateTexture(texture, data);
	textureCache.insert(pair<const char*, GLuint>(lName, texID));
}


void OpenGLResourceManager::uncacheTexture(TextureCacheMap::iterator it)
{
	delete[] it->first;
	GLuint texID = it->second;
	glDeleteTextures(1, &texID);
	textureCache.erase(it);
}


bool OpenGLResourceManager::uncacheTexture(const char* name)
{
	TextureCacheMap::iterator it = textureCache.find(name);

	if (it != textureCache.end()) {
		uncacheTexture(it);
		return true;
	}

	return false;
}


bool OpenGLResourceManager::bindTexture(const char* name)
{
	TextureCacheMap::iterator it = textureCache.find(name);

	if (it == textureCache.end()) {
		TXDTexture* tex = NULL;
		uint8_t* data = NULL;

		bool found = getTexture(name, tex, data);

		if (!found) {
			return false;
		}

		generateTexture(tex, data);
	} else {
		glBindTexture(GL_TEXTURE_2D, it->second);
	}

	return true;
}


void OpenGLResourceManager::cacheTexture(const char* name)
{
	TXDTexture* tex = NULL;
	uint8_t* data = NULL;

	if (cacheTextureData) {
		if (!ResourceIndex::getTexture(name, tex, data)) {
			char errmsg[128];
			sprintf(errmsg, "Texture %s not found", name);
			throw EngineException(errmsg, __FILE__, __LINE__);
		}
	} else {
		if (!ResourceIndex::getTextureHeader(name ,tex)) {
			char errmsg[128];
			sprintf(errmsg, "Texture %s not found", name);
			throw EngineException(errmsg, __FILE__, __LINE__);
		}
	}

	cacheTexture(tex, data);
}


void OpenGLResourceManager::beginCacheMesh(const char* name)
{
	char* lName = new char[strlen(name)+1];
	strtolower(lName, name);

	GLuint meshID = glGenLists(1);
	meshCache.insert(pair<const char*, GLuint>(lName, meshID));
	glNewList(meshID, GL_COMPILE);
}


void OpenGLResourceManager::endCacheMesh()
{
	glEndList();
}


void OpenGLResourceManager::uncacheMesh(MeshCacheMap::iterator it)
{
	delete it->first;
	GLuint meshID = it->second;
	glDeleteLists(meshID, 1);
}


void OpenGLResourceManager::uncacheMesh(const char* name)
{
	MeshCacheMap::iterator it = meshCache.find(name);

	if (it != meshCache.end()) {
		uncacheMesh(it);
	}
}


void OpenGLResourceManager::renderMesh(const char* name)
{
	MeshCacheMap::iterator it = meshCache.find(name);

	if (it == meshCache.end()) {
		throw EngineException("Mesh not found", __FILE__, __LINE__);
	}

	GLuint meshID = it->second;

	glCallList(meshID);
}


bool OpenGLResourceManager::isMeshCached(const char* name)
{
	return meshCache.find(name) != meshCache.end();
}

