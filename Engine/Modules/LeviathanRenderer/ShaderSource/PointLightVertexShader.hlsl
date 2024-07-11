cbuffer ObjectBuffer : register(b0)
{
    float4x4 WorldViewMatrix;
    float4x4 WorldViewProjectionMatrix;
    float4x4 NormalMatrix;
};

cbuffer PointLightBuffer : register(b1)
{
    float3 Radiance;
    float3 LightPositionViewSpace;
}

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
    float3 PositionTangentSpace : POSITION_TANGENT_SPACE;
    float3 VertexNormalViewSpace : VERTEX_NORMAL_VIEW_SPACE;
    float2 TexCoord : TEXTURE_COORD;
    float3 SurfaceToLightVectorTangentSpace : SURFACE_TO_LIGHT_VECTOR_TANGENT_SPACE;
};

VertexOutput main(VertexInput input)
{
    const float4 inputPosition4 = float4(input.Position, 1.0f);

    VertexOutput output;
    output.PositionClipSpace = mul(WorldViewProjectionMatrix, inputPosition4);
    output.PositionViewSpace = mul(WorldViewMatrix, inputPosition4).xyz;
    output.VertexNormalViewSpace = normalize(mul(NormalMatrix, float4(input.Normal, 0.0f)).xyz);
    output.TexCoord = input.UV;

    float3 tangentViewSpace = normalize(mul(WorldViewMatrix, float4(input.Tangent, 0.0f)).xyz);
    // Re-orthogonalize tangent with respect to normal.
    tangentViewSpace = normalize(tangentViewSpace - dot(tangentViewSpace, output.VertexNormalViewSpace) * output.VertexNormalViewSpace);
    const float3 bitangentViewSpace = normalize(cross(output.VertexNormalViewSpace, tangentViewSpace));
    const float3x3 inverseTBNMatrix = transpose(float3x3(tangentViewSpace, bitangentViewSpace, output.VertexNormalViewSpace));
    
    output.PositionTangentSpace = mul(output.PositionViewSpace, inverseTBNMatrix).xyz;
    output.SurfaceToLightVectorTangentSpace = mul(LightPositionViewSpace - output.PositionViewSpace, inverseTBNMatrix).xyz;
    
    return output;
}