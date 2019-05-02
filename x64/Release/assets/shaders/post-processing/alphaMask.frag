#version 400

uniform sampler2D FilterMask;
uniform sampler2D AlphaMask;

uniform int WinWidth;
uniform int WinHeight;

in vec2 TexCoord;

void main() 
{
	vec2 winCoords = vec2(gl_FragCoord.x / WinWidth, gl_FragCoord.y / WinHeight);
	
	gl_FragColor = texture2D(FilterMask, winCoords);

	gl_FragColor.a *= texture2D(AlphaMask, winCoords).a;
	
	if (gl_FragColor.a < 0.01)
	{
		discard;
	}
}