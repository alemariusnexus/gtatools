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

highp vec4 ShadeFragment();


uniform sampler2D DepthTex;
uniform vec2 TexDimensions;


void main()
{
	highp float depth = texture2D(DepthTex, vec2(gl_FragCoord.x / TexDimensions.x, gl_FragCoord.y / TexDimensions.y)).r;
	
	if (gl_FragDepth <= depth) {
		discard;
	}

	vec4 color = ShadeFragment();
	
	gl_FragColor = vec4(color.rgb*color.a, color.a) * 0.004;
	gl_FragDepth = vec4(0.004);
}
