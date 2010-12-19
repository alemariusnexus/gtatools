/*
 * ManagedTextureSource.h
 *
 *  Created on: 07.11.2010
 *      Author: alemariusnexus
 */

#ifndef MANAGEDTEXTURESOURCE_H_
#define MANAGEDTEXTURESOURCE_H_

#include "TextureSource.h"
#include "Engine.h"


class ManagedTextureSource : public TextureSource {
public:
	ManagedTextureSource(hash_t txdHash) : txdHash(txdHash) {}
	ManagedTextureSource(const char* txdName) : txdHash(Hash(txdName)) {}
	ManagedTextureSource(const ManagedTextureSource& other) : txdHash(other.txdHash) {}
	void setTXD(hash_t txdHash) { this->txdHash = txdHash; }
	void setTXD(const char* txdName) { setTXD(Hash(txdName)); }
	hash_t getTXD() const { return txdHash; }
	virtual GLuint getTexture(hash_t txdHash);

private:
	hash_t txdHash;
};

#endif /* MANAGEDTEXTURESOURCE_H_ */
