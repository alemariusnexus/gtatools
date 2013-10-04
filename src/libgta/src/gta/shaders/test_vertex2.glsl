void VertexPostProcessing(inout vec4 vertex)
{
}

void VertexPreProcessing(inout vec4 vertex)
{
	vertex = vertex + vec4(0.0, 0.0, 30.0, 0.0);
}
