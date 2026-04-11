//glsl version 4.5
#version 450

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec4 vertPos;

//output write
layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform  SceneData{
    vec4 fogColor;// w is for exponent
    vec4 fogDistances;//x for min, y for max, zw unused.
    vec4 ambientColor;
    vec4 sunlightDirection;//w for sun power
    vec4 sunlightColor;
} sceneData;

layout(set = 2, binding = 0) uniform sampler2D tex1;

void main()
{
    vec3 color = texture(tex1, texCoord).xyz;
    vec3 N = normalize(inNormal);
    vec3 L = normalize(sceneData.sunlightDirection.xyz);
    float lambert = max(dot(N, L), 0.0);
    float specular = 0.0;
    if (lambert > 0.0)
    {
        vec3 R = reflect(-L, N);
        vec3 V = normalize(-(vertPos.xyz));
        float specAngle = max(dot(R, V), 0.0);
        specular = pow(specAngle, 1.0);// 32 is the shininess
    }
	float Ka = 1.f;
	float Kd = 1.f;
	float Ks = 1.f;
    outFragColor = vec4(Ka * sceneData.ambientColor.xyz +
						Kd * lambert * color +
						Ks * specular * sceneData.sunlightColor.xyz, 1.0);
}