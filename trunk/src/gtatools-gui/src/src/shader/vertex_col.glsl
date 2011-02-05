#version 120

uniform mat4 MVPMatrix;

attribute vec4 Vertex;
attribute vec4 Color;

flat varying vec4 FragColor;

void main()
{
	FragColor = Color;
	gl_Position = MVPMatrix * Vertex;
}
