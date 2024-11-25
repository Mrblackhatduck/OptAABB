#version 430 core

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D DepthImage;
void main()
{
	// Shadow map debug
//	float depthValue = texture(DepthImage, TexCoords).r;
//	FragColor = vec4(vec3(depthValue),1);
	vec4 col = texture(DepthImage,TexCoords);
	FragColor = col;
}