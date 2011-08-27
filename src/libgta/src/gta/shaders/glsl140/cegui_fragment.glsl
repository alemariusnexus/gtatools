uniform bool Textured;
uniform sampler2D Texture;

in vec4 FragColor;
in vec2 FragTexCoord;


void main()
{
	if (Textured) {
		gl_FragColor = texture(Texture, FragTexCoord) * FragColor;
	} else {
		gl_FragColor = FragColor;
	}
}
