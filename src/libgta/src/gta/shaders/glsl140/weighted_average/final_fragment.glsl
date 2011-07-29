#version 140

uniform sampler2DRect ColorTex;
uniform sampler2DRect DepthComplexityTex;


void main()
{
	vec4 color = texture(ColorTex, gl_FragCoord.xy);
	float depthComplexity = texture(DepthComplexityTex, gl_FragCoord.xy).r;
	
	if (depthComplexity == 0.0) {
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}
	
	vec3 avgColor = color.rgb / color.a;
	float avgAlpha = color.a / depthComplexity;
	
	float t = pow(1.0 - avgAlpha + 0.00001, depthComplexity);
	
	gl_FragColor.rgb = avgColor * (1.0 - t);
	gl_FragColor.a = t;
}
