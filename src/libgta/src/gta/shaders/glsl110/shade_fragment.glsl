uniform bool Textured;
uniform vec4 MaterialColor;
uniform bool VertexColors;

uniform sampler2D Texture;

varying vec2 FragTexCoord;
varying vec4 FragColor;

vec4 ShadeFragment()
{
    if (Textured) {
        return texture2D(Texture, FragTexCoord);
    } else {
    	if (VertexColors) {
    		return FragColor;
    	} else {
    		return MaterialColor;
    	}
    }
}
