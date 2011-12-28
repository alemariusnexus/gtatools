#version 140

vec4 ShadeFragment();


uniform sampler2DRect PeelDepthTex;
uniform sampler2DRect OpaqueDepthTex;


void main()
{
	float opaqueDepth = texture(OpaqueDepthTex, gl_FragCoord.xy).r;
	
	if (gl_FragCoord.z >= opaqueDepth + 0.0000000003) {
		discard;
	}

	float peelDepth = texture(PeelDepthTex, gl_FragCoord.xy).r;
	
	if (gl_FragCoord.z <= peelDepth + 0.0000000003) {
		discard;
	}
	
	gl_FragColor = ShadeFragment();
}
