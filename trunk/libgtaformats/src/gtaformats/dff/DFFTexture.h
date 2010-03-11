/*
 * DFFTexture.h
 *
 *  Created on: 12.02.2010
 *      Author: alemariusnexus
 */

#ifndef DFFTEXTURE_H_
#define DFFTEXTURE_H_

#include "../gf_config.h"

class DFFTexture {
private:
	friend class DFFLoader;

public:
	DFFTexture() : diffuseName(NULL), alphaName(NULL) {}
	~DFFTexture();
	int16_t getFilterModeFlags() const { return filterModeFlags; }
	const char* getDiffuseName() const { return diffuseName; }
	const char* getAlphaName() const { return alphaName; }

private:
	int16_t filterModeFlags;
	char* diffuseName;
	char* alphaName;
};

#endif /* DFFTEXTURE_H_ */
