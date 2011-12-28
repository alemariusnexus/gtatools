#version 140

uniform bool Textured;
uniform vec4 MaterialColor;
uniform bool VertexColors;

uniform sampler2D Texture;

in vec2 FragTexCoord;
in vec4 FragColor;

vec4 ShadeFragment()
{
    if (Textured) {
        return texture(Texture, FragTexCoord);
    } else {
    	if (VertexColors) {
    		return FragColor;
    	} else {
    		return MaterialColor;
    	}
    }
}
