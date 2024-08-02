cbuffer Constants : register(b0)
{
    float4x4 ViewProjectionMatrix;
}

struct VertexInput
{
    float3 Position : POSITION;
};

struct VertexOutput
{
    float4 PositionClipSpace : SV_POSITION;
    float3 PositionLocalSpace : LOCAL_SPACE_POSITION;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.PositionClipSpace = mul(ViewProjectionMatrix, float4(input.Position.xyz, 1.0f));
    output.PositionLocalSpace = input.Position;
    return output;
}