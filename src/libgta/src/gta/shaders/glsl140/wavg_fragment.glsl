#version 140

vec4 ShadeFragment();


void main()
{
	vec4 color = ShadeFragment();
	gl_FragData[0] = vec4(color.rgb*color.a, color.a);
	gl_FragData[1] = vec4(1.0);
}
