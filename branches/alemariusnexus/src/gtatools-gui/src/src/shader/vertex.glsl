#version 130

uniform mat4 MVPMatrix;
in vec4 Vertex;
in vec2 TexCoord;

out vec2 FragTexCoord;

void main()
{
    FragTexCoord = TexCoord;
    gl_Position = MVPMatrix * Vertex;
}
