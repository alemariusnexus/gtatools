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

#ifndef DFFTEXTURE_H_
#define DFFTEXTURE_H_

#include <gtaformats/config.h>
#include "../util/CString.h"
#include <cstdlib>

class DFFTexture {
private:
	friend class DFFLoader;

public:
	DFFTexture(const CString& diffuseName, const CString& alphaName = NULL, uint16_t filterModeFlags = 0)
			: diffuseName(diffuseName), alphaName(alphaName), filterModeFlags(filterModeFlags) {}
	DFFTexture(const DFFTexture& other);
	~DFFTexture();
	uint16_t getFilterModeFlags() const { return filterModeFlags; }
	CString getDiffuseName() const { return diffuseName; }
	CString getAlphaName() const { return alphaName; }
	void setFilterModeFlags(uint16_t flags) { filterModeFlags = flags; }
	void setDiffuseName(const CString& name) { diffuseName = name; }
	void setAlphaName(const CString& name) { alphaName = name; }

private:
	CString diffuseName;
	CString alphaName;
	uint16_t filterModeFlags;
};

#endif /* DFFTEXTURE_H_ */
