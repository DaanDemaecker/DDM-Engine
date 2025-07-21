#version 450

layout(location = 0) in vec2 fragUV;

layout(location = 0) out float outColor;

layout(binding = 0) uniform sampler2D ssaoInput;

const int range = 2;

// Logic by Brian Will
// https://www.youtube.com/watch?v=7hxrPKoELpo
void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));

	float result = 0.0;

	for(int x = -range; x <= range; ++x)
	{
		for(int y = -range; y <= range; ++y)
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(ssaoInput, fragUV + offset).r;
		}
	}
	outColor = result / ((range * 2 + 1) * (range * 2 + 1));
}