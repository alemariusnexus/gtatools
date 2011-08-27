uniform sampler2D ColorTex;
uniform sampler2D DepthComplexityTex;

varying highp vec2 FragTexCoord;


void main()
{
	vec4 color = texture2D(ColorTex, FragTexCoord) * 250.0;
	float depthComplexity = texture2D(DepthComplexityTex, FragTexCoord).r * 250.0;
	
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
