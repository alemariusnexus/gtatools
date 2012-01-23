/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

// This file is automatically prepended to every GLSL source file

#ifndef _GLOBAL_GLSL_DEFINED
#define _GLOBAL_GLSL_DEFINED

#if (GTAGL_VERSION_MAJOR > 3 || (GTAGL_VERSION_MAJOR == 3  &&  GTAGL_VERSION_MINOR >= 1))
#define GTAGL_3_1_SUPPORTED
#endif


#ifdef GTAGL_3_1_SUPPORTED
#define gtaglTexture2D texture
#else
#define gtaglTexture2D texture2D
#endif

#endif
