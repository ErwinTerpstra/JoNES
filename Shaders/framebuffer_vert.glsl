layout (location = 0) in vec3 position;

uniform float screenRatio;
uniform float textureRatio;

out vec2 fragUV;

void main()
{
	fragUV.x = 0.5 + (position.x * 0.5 * (screenRatio / textureRatio));
    fragUV.y = 0.5 - (position.y * 0.5);

    gl_Position = vec4(position.xyz, 1);
};