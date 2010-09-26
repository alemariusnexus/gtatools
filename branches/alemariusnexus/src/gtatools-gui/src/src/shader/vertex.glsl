#version 120

uniform mat4 MVPMatrix;
attribute vec4 Vertex;
attribute vec2 TexCoord;

varying vec2 FragTexCoord;

void main()
{
    FragTexCoord = TexCoord;
    gl_Position = MVPMatrix * Vertex;
}
