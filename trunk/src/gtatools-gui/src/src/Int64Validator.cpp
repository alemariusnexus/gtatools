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

#include "Int64Validator.h"
#include <cstdio>



Int64Validator::State Int64Validator::validate(QString& input, int& pos) const
{
	if (input.isEmpty()) {
		return Intermediate;
	}
	if (input.startsWith('-')  &&  vmin > 0) {
		return Invalid;
	}
	if (input.startsWith('+')  &&  vmax < 0) {
		return Invalid;
	}
	if (input == "-"  ||  input == "+") {
		return Intermediate;
	}

	bool ok;
	int64_t value = input.toLongLong(&ok);

	if (!ok) {
		uint64_t uvalue = input.toULongLong(&ok);

		if (!ok) {
			return Invalid;
		} else {
			return (uvalue > vmax  &&  -uvalue < vmin) ? Invalid : Intermediate;
		}

		return Invalid;
	} else {
		if (value >= vmin  &&  value <= vmax) {
			return Acceptable;
		}
	}

	if (value >= 0) {
		return (value > vmax  &&  -value < vmin) ? Invalid : Intermediate;
	} else {
		return value < vmin ? Invalid : Intermediate;
	}
}
