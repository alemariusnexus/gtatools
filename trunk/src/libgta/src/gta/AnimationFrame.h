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

#ifndef ANIMATIONFRAME_H_
#define ANIMATIONFRAME_H_

#include <gta/config.h>
#include <nxcommon/math/Vector3.h>
#include <nxcommon/math/Quaternion.h>
#include <gtaformats/ifp/IFPRotFrame.h>
#include <gtaformats/ifp/IFPRotTransFrame.h>
#include <gtaformats/ifp/IFPRotTransScaleFrame.h>


class AnimationFrame {
public:
	AnimationFrame(float start, const Quaternion& rot, const Vector3& trans = Vector3::Zero,
			const Vector3& scale = Vector3::One);
	AnimationFrame(const AnimationFrame& other);
	AnimationFrame(const IFPRotFrame* frame);
	AnimationFrame(const IFPRotTransFrame* frame);
	AnimationFrame(const IFPRotTransScaleFrame* frame);
	float getStart() const { return start; }
	Quaternion getRotation() const { return rot; }
	Vector3 getTranslation() const { return trans; }
	Vector3 getScale() const { return scale; }

private:
	float start;
	Quaternion rot;
	Vector3 trans;
	Vector3 scale;
};

#endif /* ANIMATIONFRAME_H_ */
