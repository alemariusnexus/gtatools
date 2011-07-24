#version 140

vec4 ShadeFragment();


uniform sampler2DRect DepthTex;


void main()
{
	float depth = texture(DepthTex, gl_FragCoord.xy).r;
	
	if (gl_FragCoord.z <= depth) {
		discard;
	}
	
	gl_FragColor = ShadeFragment();
}
