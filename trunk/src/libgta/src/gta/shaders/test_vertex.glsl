#define PI 3.1415927


uniform int Time;



void VertexPostProcessing(inout vec4 vertex)
{
}

void VertexPreProcessing(inout vec4 vertex)
{
}

void VertexPostModelTransformationProcessing(inout vec4 vertex)
{
	int period = 5000;
	float wavelen = 100.0;
	float amplitude = 10.0;

	float turbulence = sin((vertex.x/wavelen + float(Time%period)/float(period)) * 2*PI) * amplitude;

	vertex.z += turbulence;
}
