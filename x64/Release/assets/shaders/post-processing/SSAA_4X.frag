#version 400

uniform sampler2D Texture;
uniform vec2 samplesOffset[4];

in vec2 TexCoord;

void main() 
{
	vec3 color = vec3(0, 0, 0);
	for (int i = 0; i < 4; ++i)
	{
		color += texture2D(Texture, TexCoord + samplesOffset[i]).rgb;
	}
	color /= 4;
	
	gl_FragColor = vec4(color, 1.0);
}