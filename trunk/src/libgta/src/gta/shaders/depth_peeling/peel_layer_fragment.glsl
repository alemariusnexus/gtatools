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

HIGHP vec4 ShadeFragment();


#ifdef GTAGL_3_1_SUPPORTED
uniform sampler2DRect PeelDepthTex;
uniform sampler2DRect OpaqueDepthTex;
#else
uniform sampler2D PeelDepthTex;
uniform sampler2D OpaqueDepthTex;
#endif


#ifndef GTAGL_3_1_SUPPORTED
uniform ivec2 TexDimensions;
#endif



#ifdef GTAGL_3_1_SUPPORTED
#define DEPTH_THRESHOLD 0.0000000003
#else
#define DEPTH_THRESHOLD 0.00002
#endif


void main()
{
#ifdef GTAGL_3_1_SUPPORTED
	HIGHP vec2 tc = gl_FragCoord.xy;
#else
	HIGHP vec2 tc = vec2(gl_FragCoord.x / float(TexDimensions.x), gl_FragCoord.y / float(TexDimensions.y));
#endif

	HIGHP float opaqueDepth = gtaglTexture2D(OpaqueDepthTex, tc).r;
	
	if (gl_FragCoord.z >= opaqueDepth + DEPTH_THRESHOLD) {
		discard;
	}

	HIGHP float peelDepth = gtaglTexture2D(PeelDepthTex, tc).r;
	
	if (gl_FragCoord.z <= peelDepth + DEPTH_THRESHOLD) {
		discard;
	}
	
	gl_FragColor = ShadeFragment();
}
