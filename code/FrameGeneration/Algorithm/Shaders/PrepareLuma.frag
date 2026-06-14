#version 450 core
in vec2 TexCoord;
out uint outLuma;
uniform sampler2D oriTex;
void main()
{
    outLuma = uint(255.0 * dot(texture(oriTex, TexCoord).xyz, vec3(0.2126, 0.7152, 0.0722)));
}
