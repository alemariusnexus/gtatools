/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#ifndef IFPFRAME_H_
#define IFPFRAME_H_

#include <gtaformats/config.h>
#include "../util/math/Quaternion.h"



class IFPFrame {
public:
	IFPFrame(const Quaternion& rot, float time) : rot(rot), time(time) {}
	Quaternion getRotation() const { return rot; }
	float getTime() const { return time; }
	void setRotation(const Quaternion& rot) { this->rot = rot; }
	void setTime(float time) { this->time = time; }

protected:
	IFPFrame() {}

private:
	Quaternion rot;
	float time;

private:
	friend class IFPLoader;
};

#endif /* IFPFRAME_H_ */
