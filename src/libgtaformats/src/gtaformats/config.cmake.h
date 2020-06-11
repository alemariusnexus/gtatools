/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef GF_CONFIG_H_
#define GF_CONFIG_H_

#include <nxcommon/config.h>


#cmakedefine GTAFORMATS_ENABLE_PVRTEXLIB
#cmakedefine GTAFORMATS_ENABLE_SQUISH

#cmakedefine _CMAKE_GTATOOLS_HAVE_GIT_REFSPEC
#cmakedefine _CMAKE_GTATOOLS_HAVE_GIT_SHA1

#ifdef NXCOMMON_BIG_ENDIAN
#define GTAFORMATS_BIG_ENDIAN
#else
#define GTAFORMATS_LITTLE_ENDIAN
#endif

#define GTATOOLS_VERSION_MAJOR ${GTATOOLS_VERSION_MAJOR}
#define GTATOOLS_VERSION_MINOR ${GTATOOLS_VERSION_MINOR}
#define GTATOOLS_VERSION_SUFFIX "${GTATOOLS_VERSION_SUFFIX}"

#ifdef _CMAKE_GTATOOLS_HAVE_GIT_REFSPEC
#define GTATOOLS_GIT_REFSPEC "${GTATOOLS_GIT_REFSPEC}"
#endif

#ifdef _CMAKE_GTATOOLS_HAVE_GIT_SHA1
#define GTATOOLS_GIT_SHA1 "${GTATOOLS_GIT_SHA1}"
#define GTATOOLS_GIT_SHA1_SHORT "${GTATOOLS_GIT_SHA1_SHORT}"
#endif

#define GTATOOLS_VERSION "${GTATOOLS_VERSION}"

#endif /* GF_CONFIG_H_ */
