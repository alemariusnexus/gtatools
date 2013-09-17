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

#ifndef IFPROTTRANSFRAME_H_
#define IFPROTTRANSFRAME_H_

#include "IFPRotFrame.h"
#include "../util/math/Vector3.h"



class IFPRotTransFrame : public IFPRotFrame {
public:
	IFPRotTransFrame(const Vector3& trans, const Quaternion& rot, float time)
			: IFPRotFrame(rot, time), trans(trans) {}
	void setTranslation(const Vector3& trans) { this->trans = trans; }
	void setTranslation(float x, float y, float z) { setTranslation(Vector3(x, y, z)); }
	Vector3 getTranslation() const { return trans; }

protected:
	IFPRotTransFrame() : IFPRotFrame() {}

private:
	Vector3 trans;

private:
	friend class IFPLoader;
};

#endif /* IFPROTTRANSFRAME_H_ */
