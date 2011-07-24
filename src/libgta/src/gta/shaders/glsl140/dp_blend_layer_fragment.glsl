#version 140

uniform sampler2DRect LayerTex;

void main()
{
	gl_FragColor = texture(LayerTex, gl_FragCoord.xy);
	gl_FragColor = vec4(gl_FragColor.rgb*gl_FragColor.a, gl_FragColor.a);
}
