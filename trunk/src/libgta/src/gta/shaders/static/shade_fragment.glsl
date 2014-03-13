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

uniform bool HasVertexColors;
//uniform bool IsTextured;
//uniform vec4 MaterialColor;
uniform vec4 MaterialAmbientReflection;
uniform vec4 MaterialDiffuseReflection;
uniform vec4 MaterialSpecularReflection;

uniform bool IsTextured[NUM_SUBMESHES];
uniform vec4 MaterialColor[NUM_SUBMESHES];

uniform sampler2D Texture[NUM_SUBMESHES];

VARYING HIGHP vec2 FragTexCoord;
VARYING HIGHP vec4 FragColor;
flat VARYING int FragSubmeshIndex;



HIGHP vec4 ShadeFragment()
{
    vec4 outColor;

	if (IsTextured[FragSubmeshIndex]) {
    	outColor = FragColor * gtaglTexture2D(Texture[FragSubmeshIndex], FragTexCoord);
    } else {
    	outColor = FragColor;
    }
    
    DispatchFragmentPostProcessing(outColor);
    
    return outColor;
}
