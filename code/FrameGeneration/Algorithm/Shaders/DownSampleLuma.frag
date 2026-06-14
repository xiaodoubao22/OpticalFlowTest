#version 450 core
in vec2 TexCoord;
out uint outLuma;
uniform usampler2D oriTex;
void main()
{
    uvec4 c = textureGather(oriTex, TexCoord);
    uint sum = c.x + c.y + c.z + c.w;
    outLuma = (sum + 2u) / 4u;
}