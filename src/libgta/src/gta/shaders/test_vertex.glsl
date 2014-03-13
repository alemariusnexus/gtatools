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

#define PI 3.1415927


uniform int Time;



void VertexPostProcessing(inout vec4 vertex)
{
}

void VertexPreProcessing(inout vec4 vertex)
{
}

void VertexPostModelTransformationProcessing(inout vec4 vertex)
{
	int period = 5000;
	float wavelen = 100.0;
	float amplitude = 10.0;

	float turbulence = sin((vertex.x/wavelen + float(Time%period)/float(period)) * 2*PI) * amplitude;

	vertex.z += turbulence;
}
