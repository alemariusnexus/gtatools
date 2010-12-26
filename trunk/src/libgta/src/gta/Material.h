/*
 * Material.h
 *
 *  Created on: 22.10.2010
 *      Author: alemariusnexus
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "config.h"
#include "Engine.h"
#include <gtaformats/dff/DFFMaterial.h>



class Material {
public:
	Material(hash_t texture);
	Material();
	Material(const DFFMaterial& dffMat);
	hash_t getTextureHash() const { return texture; }
	void setTextureHash(hash_t texHash) { texture = texHash; }
	bool isTextured() const { return textured; }
	void setTextured(bool texed) { textured = texed; }

	void getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const
	{
		r = color[0]; g = color[1]; b = color[2]; a = color[3];
	}
	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		color[0] = r; color[1] = g; color[2] = b; color[3] = a;
	}

	int guessSize() const { return sizeof(Material); }

private:
	bool textured;
	hash_t texture;
	uint8_t color[4];
};

#endif /* MATERIAL_H_ */
