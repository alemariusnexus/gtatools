highp vec4 ShadeFragment();


uniform sampler2D DepthTex;
uniform ivec2 TexDimensions;


void main()
{
	highp float depth = texture2D(DepthTex, vec2(gl_FragCoord.x / float(TexDimensions.x), gl_FragCoord.y / float(TexDimensions.y))).r;
	
	if (gl_FragCoord.z <= depth+0.00002) {
		discard;
	}
	
	gl_FragColor = ShadeFragment();
}
