/*
 * DFFMaterial.h
 *
 *  Created on: 12.02.2010
 *      Author: alemariusnexus
 */

#ifndef DFFMATERIAL_H_
#define DFFMATERIAL_H_

#include "../config.h"
#include "DFFTexture.h"


class DFFMaterial {
private:
	friend class DffLoader;

public:
	~DFFMaterial();
	void getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) { r=color[0];g=color[1];b=color[2];a=color[3]; }
	int32_t getTextureCount() { return textureCount; }
	DFFTexture** getTextures() { return textures; }

private:
	uint8_t color[4];
	int32_t textureCount;
	DFFTexture** textures;
};

#endif /* DFFMATERIAL_H_ */
