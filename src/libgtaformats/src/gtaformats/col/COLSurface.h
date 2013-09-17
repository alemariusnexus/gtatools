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

#ifndef COLSURFACE_H_
#define COLSURFACE_H_

#include <gtaformats/config.h>


class COLSurface {
	friend class COLLoader;

public:
	COLSurface(uint8_t material, uint8_t flags, uint8_t brightness, uint8_t light)
			: material(material), flags(flags), brightness(brightness), light(light) {}
	COLSurface(const COLSurface& other)
			: material(other.material), flags(other.flags), brightness(other.brightness), light(other.light)
			{}
	COLSurface() : material(0), flags(0), brightness(0), light(0) {}
	uint8_t getMaterial() const { return material; }
	uint8_t getFlags() const { return flags; }
	uint8_t getBrightness() const { return brightness; }
	uint8_t getLight() const { return light; }
	void setMaterial(uint8_t material) { this->material = material; }
	void setFlags(uint8_t flags) { this->flags = flags; }
	void setBrightness(uint8_t brightness) { this->brightness = brightness; }
	void setLight(uint8_t light) { this->light = light; }

private:
	uint8_t material;
	uint8_t flags;
	uint8_t brightness;
	uint8_t light;
};

#endif /* COLSURFACE_H_ */
