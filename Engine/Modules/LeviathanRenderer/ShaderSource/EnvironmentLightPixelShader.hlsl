struct PixelInput
{
    float4 PositionClipSpace : SV_POSITION;
    float3 PositionLocalSpace : LOCAL_SPACE_POSITION;
    float2 TexCoord : TEXTURE_COORD;
};

// Texture2D SRV table.
Texture2D Texture2DSRVTable[TEXTURE2D_SRV_TABLE_LENGTH] : register(t0);

// Texture cube SRV table
TextureCube TextureCubeSRVTable[TEXTURE_CUBE_SRV_TABLE_LENGTH] : register(t1);

// Sampler state table.
SamplerState TextureSamplerTable[TEXTURE_SAMPLER_TABLE_LENGTH] : register(s0);

// TODO: Remove
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
    const float3 radiance = TextureCubeSRVTable[ENVIRONMENT_TEXTURE_CUBE_SRV_TABLE_INDEX].Sample(TextureSamplerTable[ENVIRONMENT_TEXTURE_CUBE_SAMPLER_TABLE_INDEX], float3(1.0f, 0.0f, 0.0f)).rgb;
    //const float3 baseColor = Texture2DSRVTable[COLOR_TEXTURE2D_SRV_TABLE_INDEX].Sample(TextureSamplerTable[COLOR_TEXTURE_SAMPLER_TABLE_INDEX], input.TexCoord.xy).rgb;

    // TODO: Remove
    const float3 baseColor = Texture2DSRVTable[COLOR_TEXTURE2D_SRV_TABLE_INDEX].Sample(TextureSamplerTable[COLOR_TEXTURE_SAMPLER_TABLE_INDEX], 
        SampleSphericalMap(normalize(input.PositionLocalSpace))).rgb;
    
    return float4((baseColor /** radiance*/).xyz, 1.0f);
}