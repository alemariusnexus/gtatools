/*
 * IFPRotTransScaleFrame.h
 *
 *  Created on: 26.05.2012
 *      Author: alemariusnexus
 */

#ifndef IFPROTTRANSSCALEFRAME_H_
#define IFPROTTRANSSCALEFRAME_H_

#include "IFPRotTransFrame.h"


class IFPRotTransScaleFrame : public IFPRotTransFrame
{
public:
	IFPRotTransScaleFrame(const Vector3& trans, const Quaternion& rot, const Vector3& scale, float time)
			: IFPRotTransFrame(trans, rot, time), scale(scale) {}
	void setScale(const Vector3& s) { scale = s; }
	void setScale(float x, float y, float z) { setScale(Vector3(x, y, z)); }
	Vector3 getScale() const { return scale; }

protected:
	IFPRotTransScaleFrame() : IFPRotTransFrame() {}

private:
	Vector3 scale;

private:
	friend class IFPLoader;
};

#endif /* IFPROTTRANSSCALEFRAME_H_ */
