cbuffer ObjectBuffer : register(b0)
{
    float4x4 WorldViewMatrix;
    float4x4 WorldViewProjectionMatrix;
    float4x4 NormalMatrix;
};

struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : UV;
    float3 Tangent : TANGENT;
};

struct VertexOutput
{
    float4 PositionClipSpace : SV_POSITION;
    float2 TexCoord : TEXTURE_COORD;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.PositionClipSpace = mul(WorldViewProjectionMatrix, float4(input.Position.xyz, 1.0f));
    output.TexCoord = input.UV;
    return output;
}