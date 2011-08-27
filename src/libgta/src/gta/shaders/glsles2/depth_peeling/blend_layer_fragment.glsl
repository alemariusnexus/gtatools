uniform sampler2D LayerTex;

varying highp vec2 FragTexCoord;

void main()
{
	gl_FragColor = texture2D(LayerTex, FragTexCoord);
	gl_FragColor = vec4(gl_FragColor.rgb*gl_FragColor.a, gl_FragColor.a);
}
