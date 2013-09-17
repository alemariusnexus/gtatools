/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "UInt64Validator.h"
#include <cstdio>



UInt64Validator::State UInt64Validator::validate(QString& input, int& pos) const
{
	if (input.isEmpty()) {
		return Intermediate;
	}
	if (input.startsWith('-')) {
		return Invalid;
	}
	if (input == "+"  ||  input == "0x") {
		return Intermediate;
	}

	bool ok;
	uint64_t value = input.toULongLong(&ok, 0);

	if (!ok) {
		return Invalid;
	} else {
		if (value >= vmin  &&  value <= vmax) {
			return Acceptable;
		}
	}

	return Invalid;
}
