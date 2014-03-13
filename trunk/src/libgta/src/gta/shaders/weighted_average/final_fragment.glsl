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

#ifdef GTAGL_3_1_SUPPORTED
uniform sampler2DRect ColorTex;
uniform sampler2DRect DepthComplexityTex;
#else
uniform sampler2D ColorTex;
uniform sampler2D DepthComplexityTex;
#endif


#ifndef GTAGL_3_1_SUPPORTED
HIGHP VARYING vec2 FragTexCoord;
#endif


void main()
{
#ifdef GTAGL_3_1_SUPPORTED
	vec2 tc = gl_FragCoord.xy;
#else
	vec2 tc = FragTexCoord;
#endif

	vec4 color = gtaglTexture2D(ColorTex, tc);
	float depthComplexity = gtaglTexture2D(DepthComplexityTex, tc).r;
	
#ifndef GTAGL_3_1_SUPPORTED
	color *= 250.0;
	depthComplexity *= 250.0;
#endif
	
	if (depthComplexity == 0.0) {
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}
	
	vec3 avgColor = color.rgb / color.a;
	float avgAlpha = color.a / depthComplexity;
	
	float t = pow(1.0 - avgAlpha + 0.00001, depthComplexity);
	
	gl_FragColor.rgb = avgColor * (1.0 - t);
	gl_FragColor.a = t;
}
