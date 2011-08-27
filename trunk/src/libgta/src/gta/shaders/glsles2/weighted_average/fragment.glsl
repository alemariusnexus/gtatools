highp vec4 ShadeFragment();


void main()
{
	vec4 color = ShadeFragment();
	gl_FragData[0] = vec4(color.rgb*color.a, color.a) * 0.004;
	gl_FragData[1] = vec4(0.004);
}
