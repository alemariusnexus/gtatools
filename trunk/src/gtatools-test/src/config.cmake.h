/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-test.

	gtatools-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <gta/config.h>

#define GTASA_PATH "${GT_GTASA_PATH}"
#define WINDOW_WIDTH ${GT_WINDOW_WIDTH}
#define WINDOW_HEIGHT ${GT_WINDOW_HEIGHT}
#define WINDOW_POS_X ${GT_WINDOW_POS_X}
#define WINDOW_POS_Y ${GT_WINDOW_POS_Y}
#cmakedefine GT_WINDOW_FULLSCREEN
#define DRAW_DIST_MULTIPLIER ${GT_DRAW_DIST_MULTIPLIER}

#ifdef GTA_USE_OPENGL_ES
#define GT_USE_OPENGL_ES
#else
#undef GT_USE_OPENGL_ES
#endif

#define WINDOW_BASE_TITLE "GTATools Test Program"

#endif /* CONFIG_H_ */
