void FragmentPostProcessing(inout vec4 frag)
{
	//frag *= vec4(0.5, 1.0, 1.0, 1.0);
	float val = gl_FragCoord.z;
	frag = vec4(val, val, val, 1.0);
}
