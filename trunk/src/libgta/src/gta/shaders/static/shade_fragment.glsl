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

uniform bool Textured;
uniform HIGHP vec4 MaterialColor;
uniform bool VertexColors;

uniform sampler2D Texture;

VARYING HIGHP vec2 FragTexCoord;
VARYING HIGHP vec4 FragColor;

HIGHP vec4 ShadeFragment()
{
	/*float v = 0.0;
    
    for (int i = 0 ; i < 2000 ; i++) {
    	v += 0.1 + (v/2.0);
    }
    
    v = 1.0 / v;
    
    vec4 vc = vec4(v, 0.0, 0.0, 0.0);*/

	if (Textured) {
    	return FragColor * gtaglTexture2D(Texture, FragTexCoord) /*+ vc*/;
    	//return FragColor;
    } else {
    	return FragColor /*+ vc*/;
    }
}
