#version 140

uniform mat4 MVPMatrix;

in vec4 Vertex;
in vec4 Color;
in vec2 TexCoord;

out vec4 FragColor;
out vec2 FragTexCoord;


void main()
{
	gl_Position = MVPMatrix * Vertex;
	FragColor = Color;
	FragTexCoord = TexCoord;
}
