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

#include "Color4.h"



void Color4::blend(const Color4& other, float otherRgbFactor, float thisRgbFactor, float otherAFactor, float thisAFactor)
{
	vec = Vector4 (
			otherRgbFactor*other.getRed() + thisRgbFactor*getRed(),
			otherRgbFactor*other.getGreen() + thisRgbFactor*getGreen(),
			otherRgbFactor*other.getBlue() + thisRgbFactor*getBlue(),
			otherAFactor*other.getAlpha() + thisAFactor*getAlpha()
	);
}
