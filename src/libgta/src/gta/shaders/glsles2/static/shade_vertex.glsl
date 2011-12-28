uniform bool Textured;
uniform vec4 MaterialColor;
uniform bool VertexColors;

uniform mat4 MVPMatrix;
uniform mat4 ModelMatrix;

attribute vec4 Vertex;
attribute vec4 Normal;
attribute vec2 TexCoord;
attribute vec4 Color;

varying vec2 FragTexCoord;
varying vec4 FragColor;


void ShadeVertex()
{
    FragTexCoord = TexCoord;
    gl_Position = MVPMatrix * Vertex;
    
    if (VertexColors) {
    	FragColor = Color;
    }
}
