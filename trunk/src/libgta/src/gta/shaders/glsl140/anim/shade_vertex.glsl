#version 140

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
