/*
 * ObjectDefinition.h
 *
 *  Created on: 29.08.2010
 *      Author: alemariusnexus
 */

#ifndef OBJECTDEFINITION_H_
#define OBJECTDEFINITION_H_

class ObjectDefinition {
public:
	virtual void draw() = 0;
	void setDrawDistance(float dd) { drawDist = dd; }
	float getDrawDistance() { return drawDist; }
	void setBounds(float x, float y, float z, float r) { bounds[0] = x; bounds[1] = y; bounds[2] = z; bounds[3] = r; }
	float* getBounds() { return bounds; }

private:
	float drawDist;
	float bounds[4];
};

#endif /* OBJECTDEFINITION_H_ */
