highp vec4 ShadeFragment();


uniform sampler2D PeelDepthTex;
uniform sampler2D OpaqueDepthTex;
uniform ivec2 TexDimensions;


void main()
{
	highp vec2 texCoord = vec2(gl_FragCoord.x / float(TexDimensions.x), gl_FragCoord.y / float(TexDimensions.y));

	highp float opaqueDepth = texture2D(OpaqueDepthTex, texCoord).r;
	
	if (gl_FragCoord.z >= opaqueDepth+0.00002) {
		//discard;
	}
	
	highp float peelDepth = texture2D(PeelDepthTex, texCoord).r;
	
	if (gl_FragCoord.z <= peelDepth+0.002) {
		//discard;
	}
	
	if (peelDepth == 0.0) {
		gl_FragColor = vec4(texCoord.x, texCoord.y, 0.0, 1.0);
	} else {
		gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	}
	
	//gl_FragColor = ShadeFragment();
}
