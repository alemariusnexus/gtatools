/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#include <gf_config.h>
#include <cstdlib>

class DFFTexture {
private:
	friend class DFFLoader;

public:
	DFFTexture(char* diffuseName, char* alphaName = NULL, int16_t filterModeFlags = 0)
			: diffuseName(diffuseName), alphaName(alphaName), filterModeFlags(filterModeFlags) {}
	DFFTexture(const DFFTexture& other);
	~DFFTexture();
	int16_t getFilterModeFlags() const { return filterModeFlags; }
	const char* getDiffuseName() const { return diffuseName; }
	const char* getAlphaName() const { return alphaName; }
	void setFilterModeFlags(int16_t flags) { filterModeFlags = flags; }
	void setDiffuseName(char* name) { diffuseName = name; }
	void setDiffuseName(const char* name);
	void setAlphaName(char* name) { alphaName = name; }
	void setAlphaName(const char* name);

private:
	int16_t filterModeFlags;
	char* diffuseName;
	char* alphaName;
};

#endif /* DFFTEXTURE_H_ */
