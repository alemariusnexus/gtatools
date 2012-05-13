/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#ifndef POINTLIGHTSOURCE_H_
#define POINTLIGHTSOURCE_H_

#include "LightSource.h"


class PointLightSource : public LightSource
{
public:
	PointLightSource()
	{
		lightData.cutoffAngleCos = -2.0f;
		setPosition(Vector3::Zero);
		setShininess(1.0f);
		setAttenuation(1.0f, 0.0f, 0.0f);
	}

	virtual LightType getLightType() const { return PointLight; }
	void setPosition(const Vector3& pos) { lightData.position = pos; }
	Vector3& getPosition() { return lightData.position; }
	const Vector3& getPosition() const { return lightData.position; }
	float getShininess() const { return lightData.shininess; }
	void setShininess(float s) { lightData.shininess = s; }
	void setConstantAttenuation(float a) { lightData.constAttenuation = a; }
	float getConstantAttenuation() const { return lightData.constAttenuation; }
	void setLinearAttenuation(float a) { lightData.linearAttenuation = a; }
	float getLinearAttenuation() const { return lightData.linearAttenuation; }
	void setQuadraticAttenuation(float a) { lightData.quadAttenuation = a; }
	float getQuadraticAttenuation() const { return lightData.quadAttenuation; }
	void setAttenuation(float c, float l, float q)
			{ lightData.constAttenuation=c; lightData.linearAttenuation=l; lightData.quadAttenuation=q; }
};

#endif /* POINTLIGHTSOURCE_H_ */
