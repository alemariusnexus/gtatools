/*
	Copyright 2010 David Lerch

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
