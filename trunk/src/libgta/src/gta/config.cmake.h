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

#ifndef CONFIG_CMAKE_H_
#define CONFIG_CMAKE_H_

#include <gtaformats/config.h>

#ifdef GTAFORMATS_ENABLE_SQUISH
#define GTA_ENABLE_SQUISH
#else
#undef GTA_ENABLE_SQUISH
#endif

#ifdef GTAFORMATS_ENABLE_PVRTEXLIB
#define GTA_ENABLE_PVRTEXLIB
#else
#undef GTA_ENABLE_PVRTEXLIB
#endif

#cmakedefine GTA_USE_OPENGL_ES
#cmakedefine GTA_VISIBILITY_PVS

#endif /* CONFIG_CMAKE_H_ */
