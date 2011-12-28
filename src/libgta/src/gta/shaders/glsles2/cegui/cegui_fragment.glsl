uniform bool Textured;
uniform sampler2D Texture;

varying highp vec4 FragColor;
varying highp vec2 FragTexCoord;


void main()
{
	if (Textured) {
		gl_FragColor = texture2D(Texture, FragTexCoord) * FragColor;
	} else {
		gl_FragColor = FragColor;
	}
}
