highp vec4 ShadeFragment();


uniform sampler2D DepthTex;
uniform vec2 TexDimensions;


void main()
{
	highp float depth = texture2D(DepthTex, vec2(gl_FragCoord.x / TexDimensions.x, gl_FragCoord.y / TexDimensions.y)).r;
	
	if (gl_FragDepth <= depth) {
		discard;
	}

	vec4 color = ShadeFragment();
	
	gl_FragColor = vec4(color.rgb*color.a, color.a) * 0.004;
	gl_FragDepth = vec4(0.004);
}
