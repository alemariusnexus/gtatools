#version 140

in vec2 Vertex;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(Vertex, 1.0, 1.0);
	TexCoord = (Vertex + 1.0) * 0.5;
}
