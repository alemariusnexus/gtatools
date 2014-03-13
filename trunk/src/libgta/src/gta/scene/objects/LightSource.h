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

#ifndef LIGHTSOURCE_H_
#define LIGHTSOURCE_H_

#include "../parts/SceneObject.h"
#include "../StreamingManager.h"
#include <nxcommon/math/Vector3.h>
#include <nxcommon/math/Vector4.h>



class LightSource : public SceneObject
{
public:
	enum LightType
	{
		DirectionalLight,
		PointLight,
		SpotLight,
		AmbientLight
	};

public:
	LightSource() : enabled(true), shadowsEnabled(true)
	{
		lightData.ambient = Vector3(0.0f, 0.0f, 0.0f);
		lightData.diffuse = Vector3(0.0f, 0.0f, 0.0f);
		lightData.specular = Vector3(0.0f, 0.0f, 0.0f);
		sdist = 0.0f;
	}
	LightSource(const LightSource& other)
			: pseudoModelMat(other.pseudoModelMat), enabled(other.enabled), sdist(other.sdist),
			  shadowsEnabled(other.shadowsEnabled)
			{ memcpy(&lightData, &other.lightData, sizeof(lightData)); }

	void setEnabled(bool enabled) { this->enabled = enabled; }
	virtual bool isEnabled() const { return enabled; }
	void setAmbientColor(const Vector3& a) { lightData.ambient = a; }
	void setDiffuseColor(const Vector3& d) { lightData.diffuse = d; }
	void setSpecularColor(const Vector3& s) { lightData.specular = s; }
	Vector3& getAmbientColor() { return lightData.ambient; }
	const Vector3& getAmbientColor() const { return lightData.ambient; }
	Vector3& getDiffuseColor() { return lightData.diffuse; }
	const Vector3& getDiffuseColor() const { return lightData.diffuse; }
	Vector3& getSpecularColor() { return lightData.specular; }
	const Vector3& getSpecularColor() const { return lightData.specular; }
	virtual LightType getLightType() const = 0;
	virtual bool isShadowCastingEnabled() const { return shadowsEnabled; }
	virtual void setShadowCastingEnabled(bool e) { shadowsEnabled = e; }

	virtual typeflags_t getTypeFlags() const { return TypeFlagLight; }
	virtual uint32_t getStreamingBuckets() const { return StreamingManager::VisibleBucket; }
	virtual SceneObject* getLODParent() { return NULL; }
	virtual float getStreamingDistance() const { return sdist; }

	virtual Matrix4& getModelMatrix() { return pseudoModelMat; }
	virtual const Matrix4& getModelMatrix() const { return pseudoModelMat; }

	void setStreamingDistance(float sd) { sdist = sd; }

protected:
	struct {
		Vector3 ambient;
		Vector3 diffuse;
		Vector3 specular;
		Vector3 position;
		Vector3 direction;
		float cutoffAngleCos;
		float shininess;
		float spotlightExponent;
		float constAttenuation;
		float linearAttenuation;
		float quadAttenuation;
	} lightData;

	Matrix4 pseudoModelMat;

private:
	bool enabled;
	bool shadowsEnabled;
	float sdist;
};

#endif /* LIGHTSOURCE_H_ */
