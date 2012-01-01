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

uniform bool Textured;
uniform vec4 MaterialColor;
uniform bool VertexColors;
uniform sampler1D BoneMatrices;
uniform int Bone;

uniform mat4 MVPMatrix;
uniform mat4 ModelMatrix;

in vec4 Vertex;
in vec4 Normal;
in vec2 TexCoord;
in vec4 Color;

in vec4 BoneIndices;
in vec4 BoneWeights;

out vec2 FragTexCoord;
out vec4 FragColor;


void ShadeVertex()
{
	int size = textureSize(BoneMatrices, 0);
	
	mat4 animMat;
	
	if (Bone != -1) {
		vec4 b11 = texture(BoneMatrices, (Bone*4) / float(size));
		vec4 b12 = texture(BoneMatrices, (Bone*4 + 1) / float(size));
		vec4 b13 = texture(BoneMatrices, (Bone*4 + 2) / float(size));
		vec4 b14 = texture(BoneMatrices, (Bone*4 + 3) / float(size));
	
		animMat = mat4(b11, b12, b13, b14);
	} else {
		vec4 b11 = texture(BoneMatrices, (BoneIndices.x*4) / float(size));
		vec4 b12 = texture(BoneMatrices, (BoneIndices.x*4 + 1) / float(size));
		vec4 b13 = texture(BoneMatrices, (BoneIndices.x*4 + 2) / float(size));
		vec4 b14 = texture(BoneMatrices, (BoneIndices.x*4 + 3) / float(size));
		
		mat4 b1 = mat4(b11, b12, b13, b14);
		
		animMat = b1;
	}

    FragTexCoord = TexCoord;
    gl_Position = MVPMatrix * animMat * Vertex;
    //gl_Position = MVPMatrix * Vertex;
    
    if (VertexColors) {
    	FragColor = Color;
    }
}
