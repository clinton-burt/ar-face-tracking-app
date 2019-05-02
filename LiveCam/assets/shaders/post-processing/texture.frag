#version 400

uniform sampler2D Texture;

in vec2 TexCoord;

void main()
{
	gl_FragColor = vec4(texture2D(Texture, TexCoord).rgb, 1);
}