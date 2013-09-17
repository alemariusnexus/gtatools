/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef DFFMATERIAL_H_
#define DFFMATERIAL_H_

#include <gtaformats/config.h>
#include "DFFTexture.h"
#include <vector>

using std::vector;


class DFFGeometry;


/**	\brief A class which defines material properties for a DFFGeometryPart.
 *
 * 	This includes textures and a generic color.
 *
 * 	@see DFFGeometryPart
 * 	@see DFFTexture
 */
class DFFMaterial {
public:
	typedef vector<DFFTexture*>::iterator TextureIterator;
	typedef vector<DFFTexture*>::const_iterator ConstTextureIterator;

private:
	friend class DFFLoader;
	friend class DFFGeometry;

public:
	DFFMaterial() : geometry(NULL) {}
	DFFMaterial(const DFFMaterial& other);
	~DFFMaterial();
	void getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const
			{ r=color[0];g=color[1];b=color[2];a=color[3]; }
	uint8_t* getColor() { return color; }
	uint32_t getTextureCount() const { return textures.size(); }
	TextureIterator getTextureBegin() { return textures.begin(); }
	TextureIterator getTextureEnd() { return textures.end(); }
	ConstTextureIterator getTextureBegin() const { return textures.begin(); }
	ConstTextureIterator getTextureEnd() const { return textures.end(); }
	DFFTexture* getTexture(uint32_t index);
	const DFFTexture* getTexture(uint32_t index) const;
	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			{ color[0] = r; color[1] = g; color[2] = b; color[3] = a; }
	void addTexture(DFFTexture* texture) { textures.push_back(texture); }
	void removeTexture(DFFTexture* texture);
	void removeTexture(unsigned int index) { removeTexture(textures[index]); }
	void removeTextures();

private:
	void reparent(DFFGeometry* geom);

private:
	uint8_t color[4];
	DFFGeometry* geometry;
	vector<DFFTexture*> textures;
};

#endif /* DFFMATERIAL_H_ */
