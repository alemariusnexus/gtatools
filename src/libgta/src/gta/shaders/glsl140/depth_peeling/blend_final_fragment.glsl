#version 140

uniform sampler2DRect CombinedLayerTex;

void main()
{
	gl_FragColor = texture(CombinedLayerTex, gl_FragCoord.xy);
}
