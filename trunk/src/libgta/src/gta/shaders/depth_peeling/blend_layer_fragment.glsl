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

#ifdef GTAGL_3_1_SUPPORTED
uniform sampler2DRect LayerTex;
#else
uniform sampler2D LayerTex;
#endif


#ifndef GTAGL_3_1_SUPPORTED
VARYING HIGHP vec2 FragTexCoord;
#endif

void main()
{
#ifdef GTAGL_3_1_SUPPORTED
	HIGHP vec2 tc = gl_FragCoord.xy;
#else
	HIGHP vec2 tc = FragTexCoord;
#endif

	gl_FragColor = gtaglTexture2D(LayerTex, tc);
	gl_FragColor = vec4(gl_FragColor.rgb*gl_FragColor.a, gl_FragColor.a);
}
