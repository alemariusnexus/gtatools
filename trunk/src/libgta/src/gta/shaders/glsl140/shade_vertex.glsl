#version 140

uniform bool Textured;
uniform vec4 MaterialColor;
uniform bool VertexColors;

uniform mat4 MVPMatrix;
uniform mat4 ModelMatrix;

in vec4 Vertex;
in vec4 Normal;
in vec2 TexCoord;
in vec4 Color;

out vec2 FragTexCoord;
out vec4 FragColor;


void ShadeVertex()
{
    FragTexCoord = TexCoord;
    gl_Position = MVPMatrix * Vertex;
    
    if (VertexColors) {
    	FragColor = Color;
    }
}
