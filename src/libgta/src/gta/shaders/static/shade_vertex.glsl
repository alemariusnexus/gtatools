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

uniform mat4 MMatrix;
uniform mat4 VMatrix;
uniform mat4 VPMatrix;
uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform mat4 NormalMVMatrix;

uniform bool HasVertexColors;
//uniform bool IsTextured;
//uniform vec4 MaterialColor;
uniform vec4 MaterialAmbientReflection;
uniform vec4 MaterialDiffuseReflection;
uniform vec4 MaterialSpecularReflection;

uniform bool IsTextured[NUM_SUBMESHES];
uniform vec4 MaterialColor[NUM_SUBMESHES];

uniform int SubmeshOffset;

ATTRIBUTE vec4 Vertex;
ATTRIBUTE vec3 Normal;
ATTRIBUTE vec2 TexCoord;
ATTRIBUTE vec4 Color;
ATTRIBUTE int SubmeshIndex;

VARYING vec2 FragTexCoord;
VARYING vec4 FragColor;
flat VARYING int FragSubmeshIndex;



void CalculateVertexLighting(inout vec4 color, vec3 eVertex, vec3 eNormal);



void ShadeVertex()
{
	int actualSubmeshIndex = SubmeshIndex - SubmeshOffset;
	
	FragSubmeshIndex = actualSubmeshIndex;

	vec4 vertex = Vertex;
	DispatchVertexPreProcessing(vertex);
	
#ifdef SHADER_PLUGIN_HOOK_VERTEXPOSTMODELTRANSFORMATIONPROCESSING_USED
	vec4 mVertex = MMatrix * vertex;
	DispatchVertexPostModelTransformationProcessing(mVertex);
	vec3 eVertex = vec3(VMatrix * mVertex);
	gl_Position = VPMatrix * mVertex;
#else
	vec3 eVertex = vec3(MVMatrix * vertex);
	gl_Position = MVPMatrix * vertex;
#endif

    FragTexCoord = TexCoord;
    
    vec3 eNormal = normalize(vec3(NormalMVMatrix * vec4(Normal, 1.0)));
    
    FragColor = MaterialColor[actualSubmeshIndex] * Color;
    
    CalculateVertexLighting(FragColor, eVertex, eNormal);
    
    DispatchVertexPostProcessing(gl_Position);
}
