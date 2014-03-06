/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef COLLISIONBOX_H_
#define COLLISIONBOX_H_

#include <gta/config.h>
#include <gtaformats/col/COLBox.h>


class CollisionBox
{
public:
	CollisionBox(const Vector3& min, const Vector3& max) : min(min), max(max) {}
	CollisionBox(const CollisionBox& other) : min(other.min), max(other.max) {}
	CollisionBox(const COLBox& box) : min(box.getMinimum()), max(box.getMaximum()) {}
	Vector3& getMinimum() { return min; }
	const Vector3& getMinimum() const { return min; }
	Vector3& getMaximum() { return max; }
	const Vector3& getMaximum() const { return max; }
	void setMinimum(const Vector3& min) { this->min = min; }
	void setMaximum(const Vector3& max) { this->max = max; }

private:
	Vector3 min;
	Vector3 max;
};

#endif /* COLLISIONBOX_H_ */
