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
    float3 PositionViewSpace : POSITION_VIEW_SPACE;
    float3 VertexNormalViewSpace : VERTEX_NORMAL_VIEW_SPACE;
    float2 TexCoord : TEXTURE_COORD;
    float3x3 TBNMatrix : TBN_MATRIX;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.PositionClipSpace = mul(WorldViewProjectionMatrix, float4(input.Position, 1.0f));
    output.PositionViewSpace = mul(WorldViewMatrix, float4(input.Position, 1.0f)).xyz;
    output.VertexNormalViewSpace = normalize(mul(NormalMatrix, float4(input.Normal, 0.0f)).xyz);
    output.TexCoord = input.UV;

    float3 tangentViewSpace = normalize(mul(WorldViewMatrix, float4(input.Tangent, 0.0f)).xyz);
    float3 bitangentViewSpace = normalize(cross(output.VertexNormalViewSpace, tangentViewSpace));
    float3x3 tbnMatrix = float3x3(tangentViewSpace, bitangentViewSpace, output.VertexNormalViewSpace);
    float3x3 inverseTBNMatrix = transpose(tbnMatrix);
    
    output.TBNMatrix = tbnMatrix;

    return output;
}