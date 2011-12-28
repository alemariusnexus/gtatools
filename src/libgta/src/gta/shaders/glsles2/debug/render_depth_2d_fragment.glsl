precision highp float;

uniform highp sampler2D DepthTex;

varying vec2 TexCoord;


vec4 SplitFloat(in float f)
{
	float c1 = 255.0 * f;
	float fc1 = fract(c1);
	float c2 = 255.0 * fc1;
	float fc2 = fract(c2);
	float c3 = 255.0 * fc2;
	float fc3 = fract(c3);
	float c4 = 255.0 * fc3;
	return vec4(f, fc1, fc2, fc3);
}


void main()
{
	float depth = texture2D(DepthTex, TexCoord).r;
	gl_FragColor = SplitFloat(depth);
	//gl_FragColor = vec4(0.5, 0.0, 0.0, 0.0);
}
