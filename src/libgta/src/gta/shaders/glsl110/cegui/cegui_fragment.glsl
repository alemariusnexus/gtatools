uniform bool Textured;
uniform sampler2D Texture;

varying vec4 FragColor;
varying vec2 FragTexCoord;


void main()
{
	if (Textured) {
		gl_FragColor = texture2D(Texture, FragTexCoord) * FragColor;
	} else {
		gl_FragColor = FragColor;
	}
}
