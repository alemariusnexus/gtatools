uniform bool Textured;
uniform highp vec4 MaterialColor;
uniform bool VertexColors;

uniform sampler2D Texture;

varying highp vec2 FragTexCoord;
varying highp vec4 FragColor;

highp vec4 ShadeFragment()
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
