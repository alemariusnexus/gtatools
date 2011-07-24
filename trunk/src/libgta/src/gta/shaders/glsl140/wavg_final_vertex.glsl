#version 140

in vec2 Vertex;


void main()
{
	gl_Position = vec4(Vertex, 1.0, 1.0);
}
