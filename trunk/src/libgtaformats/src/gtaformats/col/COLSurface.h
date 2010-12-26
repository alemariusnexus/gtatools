/*
 * COLSurface.h
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#ifndef COLSURFACE_H_
#define COLSURFACE_H_

#include "../config.h"


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
