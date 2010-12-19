/*
 * ManagedTextureSource.cpp
 *
 *  Created on: 07.11.2010
 *      Author: alemariusnexus
 */

#include "ManagedTextureSource.h"
#include "Engine.h"
#include "ResourceManager.h"



GLuint ManagedTextureSource::getTexture(hash_t texHash)
{
	ResourceManager* resMgr = Engine::getInstance()->getResourceManager();
	GLuint tex = resMgr->getTexture(TextureIndex(txdHash, texHash));
	return tex;
}
