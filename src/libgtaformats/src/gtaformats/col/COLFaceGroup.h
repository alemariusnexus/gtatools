/*
 * COLFaceGroup.h
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#ifndef COLFACEGROUP_H_
#define COLFACEGROUP_H_

#include "../util/math/Vector3.h"



class COLFaceGroup {
	friend class COLoader;

public:
	COLFaceGroup(const Vector3& min, const Vector3& max, uint16_t startFace, uint16_t endFace)
			: min(min), max(max), startFace(startFace), endFace(endFace) {}
	COLFaceGroup(const COLFaceGroup& other)
			: min(other.min), max(other.max), startFace(other.startFace), endFace(other.endFace) {}
	COLFaceGroup() : startFace(0), endFace(0) {}
	const Vector3& getMinimum() const { return min; }
	Vector3& getMinimum() { return min; }
	const Vector3& getMaximum() const { return max; }
	Vector3& getMaximum() { return max; }
	uint16_t getStartFace() const { return startFace; }
	uint16_t getEndFace() const { return endFace; }
	uint16_t getFaceCount() const { return endFace-startFace; }
	void setMinimum(const Vector3& min) { this->min = min; }
	void setMaximum(const Vector3& max) { this->max = max; }
	void setStartFace(uint16_t face) { this->startFace = face; }
	void setEndFace(uint16_t face) { this->endFace = face; }

private:
	Vector3 min;
	Vector3 max;
	uint16_t startFace;
	uint16_t endFace;
};

#endif /* COLFACEGROUP_H_ */
