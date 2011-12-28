attribute vec2 Vertex;

varying vec2 TexCoord;

void main()
{
	gl_Position = vec4(Vertex, 1.0, 1.0);
	TexCoord = (Vertex + 1.0) * 0.5;
}
