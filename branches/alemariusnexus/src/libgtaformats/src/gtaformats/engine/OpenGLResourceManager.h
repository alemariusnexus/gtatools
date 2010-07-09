/*
 * OpenGLResourceManager.h
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#ifndef OPENGLRESOURCEMANAGER_H_
#define OPENGLRESOURCEMANAGER_H_

#ifdef _WIN32
	#include <windows.h>
#endif

#include "../gf_config.h"
#include "../util/StringComparator.h"
#include "ResourceIndex.h"
#include "../txd/TXDTexture.h"
#include <GL/gl.h>
#include <GL/glu.h>



class OpenGLResourceManager : public ResourceIndex {
public:
	typedef map<const char*, GLuint, StringComparator> TextureCacheMap;
	typedef map<const char*, GLuint, StringComparator> MeshCacheMap;

public:
	OpenGLResourceManager(bool cacheTextureData = true);
	~OpenGLResourceManager();
	void cacheTexture(const char* name);
	bool uncacheTexture(const char* name);
	bool bindTexture(const char* name);
	void beginCacheMesh(const char* name);
	void endCacheMesh();
	void uncacheMesh(const char* name);
	void renderMesh(const char* name);
	bool isMeshCached(const char* name);

private:
	void cacheTexture(TXDTexture* texture, uint8_t* data);
	void uncacheTexture(TextureCacheMap::iterator it);
	void uncacheMesh(MeshCacheMap::iterator it);
	GLuint generateTexture(TXDTexture* texture, uint8_t* data);

private:
	bool cacheTextureData;
	TextureCacheMap textureCache;
	MeshCacheMap meshCache;
};

#endif /* OPENGLRESOURCEMANAGER_H_ */
