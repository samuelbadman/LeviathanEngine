struct PixelInput
{
    float4 PositionClipSpace : SV_POSITION;
    float3 PositionLocalSpace : LOCAL_SPACE_POSITION;
};

Texture2D EquirectangularMap : register(t0);
SamplerState EquirectangularMapSampler : register(s0);

#define INV_A_TAN float2(0.1591f, 0.3183f)
float2 SampleSphericalMap(float3 inVector)
{
    float2 uv = float2(atan2(inVector.z, inVector.x), asin(inVector.y));
    uv *= INV_A_TAN;
    uv += 0.5f;
    return uv;
}

float4 main(PixelInput input) : SV_TARGET
{
    const float2 texCoord = SampleSphericalMap(normalize(input.PositionLocalSpace));
    const float3 color = EquirectangularMap.Sample(EquirectangularMapSampler, texCoord).rgb;
    return float4(color, 1.0f);
}