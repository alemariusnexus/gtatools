uniform mat4 MVPMatrix;

attribute vec4 Vertex;
attribute vec4 Color;
attribute vec2 TexCoord;

varying vec4 FragColor;
varying vec2 FragTexCoord;


void main()
{
	gl_Position = MVPMatrix * Vertex;
	FragColor = Color;
	FragTexCoord = TexCoord;
}
