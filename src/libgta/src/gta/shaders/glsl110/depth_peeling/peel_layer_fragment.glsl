vec4 ShadeFragment();


uniform sampler2D PeelDepthTex;
uniform sampler2D OpaqueDepthTex;
uniform ivec2 TexDimensions;


void main()
{
	vec2 texCoord = vec2(gl_FragCoord.x / float(TexDimensions.x), gl_FragCoord.y / float(TexDimensions.y));

	float opaqueDepth = texture2D(OpaqueDepthTex, texCoord).r;
	
	if (gl_FragCoord.z >= opaqueDepth+0.00002) {
		discard;
	}
	
	float peelDepth = texture2D(PeelDepthTex, texCoord).r;
	
	if (gl_FragCoord.z <= peelDepth+0.00002) {
		discard;
	}
	
	gl_FragColor = ShadeFragment();
}
