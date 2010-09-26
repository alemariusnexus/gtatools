#version 120

uniform bool Textured;
uniform sampler2D Texture;

varying vec2 FragTexCoord;

void main()
{
    if (Textured) {
        gl_FragColor = texture2D(Texture, FragTexCoord);
    } else {
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
}
