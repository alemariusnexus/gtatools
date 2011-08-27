uniform sampler2D CombinedLayerTex;

varying highp vec2 FragTexCoord;

void main()
{
	gl_FragColor = texture2D(CombinedLayerTex, FragTexCoord);
}
