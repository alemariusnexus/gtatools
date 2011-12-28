attribute vec2 Vertex;

varying vec2 FragTexCoord;

void main()
{
	gl_Position = vec4(Vertex, 0.0, 1.0);
	FragTexCoord = (Vertex+1.0)*0.5;
}
