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


uniform sampler2D PeelDepthTex;
uniform sampler2D OpaqueDepthTex;
uniform ivec2 TexDimensions;


void main()
{
	highp vec2 texCoord = vec2(gl_FragCoord.x / float(TexDimensions.x), gl_FragCoord.y / float(TexDimensions.y));

	highp float opaqueDepth = texture2D(OpaqueDepthTex, texCoord).r;
	
	if (gl_FragCoord.z >= opaqueDepth+0.00002) {
		//discard;
	}
	
	highp float peelDepth = texture2D(PeelDepthTex, texCoord).r;
	
	if (gl_FragCoord.z <= peelDepth+0.002) {
		//discard;
	}
	
	if (peelDepth == 0.0) {
		gl_FragColor = vec4(texCoord.x, texCoord.y, 0.0, 1.0);
	} else {
		gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	}
	
	//gl_FragColor = ShadeFragment();
}
