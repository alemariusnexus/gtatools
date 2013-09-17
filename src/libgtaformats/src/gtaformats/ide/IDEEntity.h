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

#ifndef IDEENTITY_H_
#define IDEENTITY_H_

#include <gtaformats/config.h>
#include "IDEStatement.h"


class IDEEntity : public IDEStatement
{
public:
	IDEEntity(int32_t id) : id(id) {}
	int32_t getID() const { return id; }

private:
	int32_t id;
};

#endif /* IDEENTITY_H_ */
