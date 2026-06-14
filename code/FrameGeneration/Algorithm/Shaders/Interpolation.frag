#version 450 core

in vec2 TexCoord;
out vec4 outColor;

layout(binding = 0) uniform sampler2D prevColor;
layout(binding = 1) uniform sampler2D nextColor;
layout(binding = 2) uniform isampler2D opticalFlow;

uniform ivec2 uInputLumaResolution;
uniform ivec2 uOpticalFlowResolution;

void main()
{
    if (TexCoord.x < 0.005) {
        outColor = vec4(0.0, 1.0, 0.0, 1.0);
        return;
    }

	vec2 uv = TexCoord;
	uv.y  = 1.0 - uv.y;

    vec2 ofCoord = uv * vec2(uInputLumaResolution) / vec2(uOpticalFlowResolution * ivec2(8u));

    ivec2 ofRaw = texture(opticalFlow, ofCoord).xy;
    vec2 ofValue = vec2(ofRaw) / vec2(uInputLumaResolution);
	
	float fac = 0.5;
    vec2 uvPrev = uv + fac * ofValue;
    vec2 uvNext = uv - fac * ofValue;

    vec3 colorPrev = texture(prevColor, uvPrev).rgb;
    vec3 colorNext = texture(nextColor, uvNext).rgb;

    outColor = vec4(mix(colorPrev, colorNext, 0.5), 1.0);
}