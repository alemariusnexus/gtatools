#version 140

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

uniform sampler2D DepthTex;

in vec2 TexCoord;


vec4 SplitFloat(in float f)
{
	float c1 = 255.0 * f;
	float fc1 = fract(c1);
	float c2 = 255.0 * fc1;
	float fc2 = fract(c2);
	float c3 = 255.0 * fc2;
	float fc3 = fract(c3);
	float c4 = 255.0 * fc3;
	return vec4(f, fc1, fc2, fc3);
}


void main()
{
	float depth = texture(DepthTex, TexCoord).r;
	gl_FragColor = SplitFloat(depth);
	//gl_FragColor = vec4(0.5, 0.0, 0.0, 0.0);
}
