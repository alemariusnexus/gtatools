/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef AMBIENTLIGHTSOURCE_H_
#define AMBIENTLIGHTSOURCE_H_

#include "LightSource.h"


class AmbientLightSource : public LightSource
{
public:
	AmbientLightSource() {}
	AmbientLightSource(const AmbientLightSource& other) : LightSource(other) {}

	virtual LightType getLightType() const { return AmbientLight; }
	virtual SceneObject* clone() const { return new AmbientLightSource(*this); }
	virtual float getStreamingDistance() const { return 0.0f; }
	virtual void setModelMatrix(const Matrix4& matrix) {}
};

#endif /* AMBIENTLIGHTSOURCE_H_ */
