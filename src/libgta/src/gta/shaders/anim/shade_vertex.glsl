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
uniform int Bone;

#ifdef GTAGL_BONE_MATRIX_UNIFORM_ARRAY_100
uniform mat4 BoneMatrices[100];
#else
uniform sampler2D BoneMatrices;
#endif

#if !defined(GTAGL_3_1_SUPPORTED)  &&  !defined(GTAGL_BONE_MATRIX_UNIFORM_ARRAY_100)
uniform int TexSize;
#endif

uniform mat4 MVPMatrix;
uniform mat4 ModelMatrix;

ATTRIBUTE vec4 Vertex;
ATTRIBUTE vec4 Normal;
ATTRIBUTE vec2 TexCoord;
ATTRIBUTE vec4 Color;

ATTRIBUTE vec4 BoneIndices;
ATTRIBUTE vec4 BoneWeights;

VARYING vec2 FragTexCoord;
VARYING vec4 FragColor;


void ShadeVertex()
{
#ifndef GTAGL_BONE_MATRIX_UNIFORM_ARRAY_100
#ifdef GTAGL_3_1_SUPPORTED
	int size = textureSize(BoneMatrices, 0).x;
#else
	int size = TexSize;
#endif
#endif
	
	if (Bone != -1) {
		// We have an unskinned animation, so there's only one affecting bone
	
#ifdef GTAGL_BONE_MATRIX_UNIFORM_ARRAY_100
		mat4 animMat = BoneMatrices[Bone];
#else
		vec4 b11 = gtaglTexture2D(BoneMatrices, vec2(float(Bone*4) / float(size), 0.5));
		vec4 b12 = gtaglTexture2D(BoneMatrices, vec2(float(Bone*4 + 1) / float(size), 0.5));
		vec4 b13 = gtaglTexture2D(BoneMatrices, vec2(float(Bone*4 + 2) / float(size), 0.5));
		vec4 b14 = gtaglTexture2D(BoneMatrices, vec2(float(Bone*4 + 3) / float(size), 0.5));
		
		mat4 animMat = mat4(b11, b12, b13, b14);
#endif
		
		gl_Position = MVPMatrix * animMat * Vertex;
	} else {
		// This animation uses skinning, so we have to compute the animation matrix by weighting up to four
		// different bone matrices.

#ifdef GTAGL_BONE_MATRIX_UNIFORM_ARRAY_100
		mat4 b1 = BoneMatrices[int(BoneIndices.x)];
		mat4 b2 = BoneMatrices[int(BoneIndices.y)];
		mat4 b3 = BoneMatrices[int(BoneIndices.z)];
		mat4 b4 = BoneMatrices[int(BoneIndices.w)];
#else
		vec4 b11 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.x*4.0) / float(size), 0.0));
		vec4 b12 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.x*4.0 + 1.0) / float(size), 0.0));
		vec4 b13 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.x*4.0 + 2.0) / float(size), 0.0));
		vec4 b14 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.x*4.0 + 3.0) / float(size), 0.0));
		
		vec4 b21 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.y*4.0) / float(size), 0.0));
		vec4 b22 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.y*4.0 + 1.0) / float(size), 0.0));
		vec4 b23 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.y*4.0 + 2.0) / float(size), 0.0));
		vec4 b24 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.y*4.0 + 3.0) / float(size), 0.0));
		
		vec4 b31 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.z*4.0) / float(size), 0.0));
		vec4 b32 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.z*4.0 + 1.0) / float(size), 0.0));
		vec4 b33 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.z*4.0 + 2.0) / float(size), 0.0));
		vec4 b34 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.z*4.0 + 3.0) / float(size), 0.0));
		
		vec4 b41 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.w*4.0) / float(size), 0.0));
		vec4 b42 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.w*4.0 + 1.0) / float(size), 0.0));
		vec4 b43 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.w*4.0 + 2.0) / float(size), 0.0));
		vec4 b44 = gtaglTexture2D(BoneMatrices, vec2((BoneIndices.w*4.0 + 3.0) / float(size), 0.0));
		
		mat4 b1 = mat4(b11, b12, b13, b14);
		mat4 b2 = mat4(b21, b22, b23, b24);
		mat4 b3 = mat4(b31, b32, b33, b34);
		mat4 b4 = mat4(b41, b42, b43, b44);
#endif
		
		gl_Position = b1*BoneWeights.x*Vertex + b2*BoneWeights.y*Vertex + b3*BoneWeights.z*Vertex + b4*BoneWeights.w*Vertex;
		gl_Position = MVPMatrix * gl_Position;
	}

    FragTexCoord = TexCoord;
    
    if (VertexColors) {
    	FragColor = Color;
    }
}
