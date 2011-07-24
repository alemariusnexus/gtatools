#version 140

vec4 ShadeFragment();


void main()
{
	gl_FragColor = ShadeFragment();
	
	if (gl_FragColor.a < 0.01) {
		discard;
	}
}
