/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef GF_CONFIG_H_
#define GF_CONFIG_H_

#ifdef _MSC_VER
#	pragma warning(disable : 4996)
#endif

#include "stdint.h"


#define PI 3.141593f



#cmakedefine GF_USE_SQUISH

#cmakedefine EXCEPTION_POSITION_INFO

#define GTATOOLS_VERSION "${GTATOOLS_VERSION_MAJOR}.${GTATOOLS_VERSION_MINOR}.${GTATOOLS_VERSION_PATCH}${GTATOOLS_VERSION_SUFFIX}"
#define GTATOOLS_VERSION_MAJOR ${GTATOOLS_VERSION_MAJOR}
#define GTATOOLS_VERSION_MINOR ${GTATOOLS_VERSION_MINOR}
#define GTATOOLS_VERSION_PATCH ${GTATOOLS_VERSION_PATCH}
#define GTATOOLS_VERSION_SUFFIX "${GTATOOLS_VERSION_SUFFIX}"

#endif /* GF_CONFIG_H_ */
