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

#ifndef IDESTATEMENT_H_
#define IDESTATEMENT_H_

#include <gtaformats/config.h>


enum IDEType
{
	IDETypeStaticObject = 1 << 0,
	IDETypeTimedObject = 1 << 1,
	IDETypeAnimation = 1 << 2,
	IDETypePedestrian = 1 << 3,
	IDETypeWeapon = 1 << 4,

	IDETypeGroupEntity = IDETypeStaticObject | IDETypeTimedObject | IDETypeAnimation | IDETypePedestrian
			| IDETypeWeapon,
	IDETypeGroupAll = IDETypeStaticObject | IDETypeTimedObject | IDETypeAnimation | IDETypePedestrian
			| IDETypeWeapon
};


typedef char idetype_t;


class IDEStatement {
public:
	virtual ~IDEStatement() {}
	virtual idetype_t getType() const = 0;
};

#endif /* IDESTATEMENT_H_ */
