/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef SPOTLIGHTSOURCE_H_
#define SPOTLIGHTSOURCE_H_

#include "LightSource.h"


class SpotLightSource : public LightSource
{
public:
	SpotLightSource()
	{
		setPosition(Vector3::Zero);
		setDirection(Vector3::UnitZ);
		setShininess(1.0f);
		setAttenuation(1.0f, 0.0f, 0.0f);
		setCutoffAngleCosine(0.70710678f); // 45 degrees
		setSpotlightExponent(1.0f);
	}
	SpotLightSource(const SpotLightSource& other) : LightSource(other) {}

	virtual LightType getLightType() const { return SpotLight; }
	/*Vector3& getPosition() { return lightData.position; }
	const Vector3& getPosition() const { return lightData.position; }*/
	void setPosition(const Vector3& pos) { lightData.position = pos; pseudoModelMat = Matrix4::translation(pos); }
	void setDirection(const Vector3& direction) { lightData.direction = direction; }
	Vector3& getDirection() { return lightData.direction; }
	const Vector3& getDirection() const { return lightData.direction; }
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
	void setCutoffAngleCosine(float coc) { lightData.cutoffAngleCos = coc; }
	float getCutoffAngleCosine() const { return lightData.cutoffAngleCos; }
	void setSpotlightExponent(float exp) { lightData.spotlightExponent = exp; }
	float getSpotlightExponent() const { return lightData.spotlightExponent; }
	virtual SceneObject* clone() const { return new SpotLightSource(*this); }
	virtual void setModelMatrix(const Matrix4& matrix)
			{ const float* mm = matrix.toArray(); setPosition(Vector3(mm[12], mm[13], mm[14])); }
};

#endif /* SPOTLIGHTSOURCE_H_ */
