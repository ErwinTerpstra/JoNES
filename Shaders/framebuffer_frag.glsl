in vec2 fragUV;

uniform sampler2D mainTexture;

layout (location = 0) out vec4 fragColor;

void main()
{
	if (fragUV.s >= 0.0 && fragUV.s <= 1.0)
    	fragColor = texture(mainTexture, fragUV.st);
    else
    	fragColor = vec4(0, 0, 0.3, 1);
};