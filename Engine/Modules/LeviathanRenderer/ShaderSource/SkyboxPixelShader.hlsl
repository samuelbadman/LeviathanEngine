struct PixelInput
{
    float4 PositionClipSpace : SV_Position;
    float3 PositionLocalSpace : LOCAL_SPACE_POSITION;
};

TextureCube SkyboxCubemap : register(t0);
SamplerState SkyboxCubemapSampler : register(s0);

float4 main(PixelInput input) : SV_Target
{
    return float4(SkyboxCubemap.Sample(SkyboxCubemapSampler, input.PositionLocalSpace).rgb, 1.0f);
}