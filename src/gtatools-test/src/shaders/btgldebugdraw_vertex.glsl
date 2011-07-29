#version 120

uniform mat4 MVPMatrix;

attribute vec4 Vertex;
attribute vec4 Color;

varying vec4 FragColor;


void main()
{
	gl_Position = MVPMatrix*Vertex;
	//gl_Position = Vertex;
	FragColor = Color;
}
