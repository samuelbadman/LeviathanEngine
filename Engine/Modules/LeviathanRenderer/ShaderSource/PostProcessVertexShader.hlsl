struct VertexInput
{
    float2 Position : POSITION;
    float2 UV : UV;
};

struct VertexOutput
{
    float4 PositionClipSpace : SV_POSITION;
    float2 TexCoord : TEX_COORD;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.PositionClipSpace = float4(input.Position.xy, 0.0f, 1.0f);
    output.TexCoord = input.UV;
    return output;
}