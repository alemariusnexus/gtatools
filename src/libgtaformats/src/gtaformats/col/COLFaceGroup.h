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
	COLFaceGroup(const Vector3& vmin, const Vector3& vmax, uint16_t startFace, uint16_t endFace)
			: vmin(vmin), vmax(vmax), startFace(startFace), endFace(endFace) {}
	COLFaceGroup(const COLFaceGroup& other)
			: vmin(other.vmin), vmax(other.vmax), startFace(other.startFace), endFace(other.endFace) {}
	COLFaceGroup() : startFace(0), endFace(0) {}
	const Vector3& getMinimum() const { return vmin; }
	Vector3& getMinimum() { return vmin; }
	const Vector3& getMaximum() const { return vmax; }
	Vector3& getMaximum() { return vmax; }
	uint16_t getStartFace() const { return startFace; }
	uint16_t getEndFace() const { return endFace; }
	uint16_t getFaceCount() const { return endFace-startFace; }
	void setMinimum(const Vector3& vmin) { this->vmin = vmin; }
	void setMaximum(const Vector3& vmax) { this->vmax = vmax; }
	void setStartFace(uint16_t face) { this->startFace = face; }
	void setEndFace(uint16_t face) { this->endFace = face; }

private:
	Vector3 vmin;
	Vector3 vmax;
	uint16_t startFace;
	uint16_t endFace;
};

#endif /* COLFACEGROUP_H_ */
