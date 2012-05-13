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

#ifndef LIGHTSOURCE_H_
#define LIGHTSOURCE_H_

#include <gtaformats/util/math/Vector3.h>
#include <gtaformats/util/math/Vector4.h>



class LightSource
{
public:
	enum LightType
	{
		DirectionalLight,
		PointLight,
		SpotLight
	};

public:
	LightSource() : enabled(true)
	{
		lightData.ambient = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		lightData.diffuse = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		lightData.specular = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	void setEnabled(bool enabled) { this->enabled = enabled; }
	bool isEnabled() const { return enabled; }
	void setAmbientColor(const Vector4& a) { lightData.ambient = a; }
	void setDiffuseColor(const Vector4& d) { lightData.diffuse = d; }
	void setSpecularColor(const Vector4& s) { lightData.specular = s; }
	Vector4& getAmbientColor() { return lightData.ambient; }
	const Vector4& getAmbientColor() const { return lightData.ambient; }
	Vector4& getDiffuseColor() { return lightData.diffuse; }
	const Vector4& getDiffuseColor() const { return lightData.diffuse; }
	Vector4& getSpecularColor() { return lightData.specular; }
	const Vector4& getSpecularColor() const { return lightData.specular; }
	virtual LightType getLightType() const = 0;

protected:
	// This struct must match struct LightSource in GLSL
	struct {
		Vector4 ambient;
		Vector4 diffuse;
		Vector4 specular;
		Vector3 position;
		Vector3 direction;
		float cutoffAngleCos;
		float shininess;
		float spotlightExponent;
		float constAttenuation;
		float linearAttenuation;
		float quadAttenuation;
	} lightData;

private:
	bool enabled;
};

#endif /* LIGHTSOURCE_H_ */
