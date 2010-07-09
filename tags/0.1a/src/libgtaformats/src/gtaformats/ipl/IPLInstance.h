/*
 * IPLInstance.h
 *
 *  Created on: 18.04.2010
 *      Author: alemariusnexus
 */

#ifndef IPLINSTANCE_H_
#define IPLINSTANCE_H_

#include "../gf_config.h"
#include "IPLStatement.h"

#define IPL_TYPE_INSTANCE 0



class IPLInstance : public IPLStatement {
public:
	IPLInstance	(
			int32_t id, const char* modelName,
			float posX, float posY, float posZ,
			float rotX, float rotY, float rotZ, float rotW,
			float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
			int32_t interior = 0,
			int32_t lod = 0
	);
	~IPLInstance();

	virtual ipltype_t getType() const { return IPL_TYPE_INSTANCE; }
	int32_t getID() const { return id; }
	const char* getModelName() const { return modelName; }
	void getPosition(float& x, float& y, float& z) const { x = posX; y = posY; z = posZ; }
	void getRotation(float& x, float& y, float& z, float& w) const
			{ x = rotX; y = rotY; z = rotZ; w = rotW; }
	void getScale(float& x, float& y, float& z) const { x = scaleX; y = scaleY; z = scaleZ; }
	int32_t getInterior() const { return interior; }
	int32_t getLOD() const { return lod; }

private:
	int32_t id;
	char* modelName;
	float posX;
	float posY;
	float posZ;
	float rotX;
	float rotY;
	float rotZ;
	float rotW;
	float scaleX;
	float scaleY;
	float scaleZ;
	int32_t interior;
	int32_t lod;
};

#endif /* IPLINSTANCE_H_ */
