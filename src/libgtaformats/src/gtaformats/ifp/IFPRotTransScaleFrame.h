/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
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
