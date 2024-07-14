struct PixelInput
{
    float4 PositionClipSpace : SV_POSITION;
    float2 TexCoord : TEX_COORD;
};

Texture2D SceneTexture : register(t0);
SamplerState SceneTextureSampler : register(s0);

float3 HDRToneMap(float3 color)
{
    return color / (color + float3(1.0f, 1.0f, 1.0f));
}

float3 GammaCorrect(float3 color)
{
    const float gammaExponent = 1.0f / 2.2f;
    return float3(pow(color, float3(gammaExponent, gammaExponent, gammaExponent)));
}

float4 main(PixelInput input) : SV_TARGET
{
    float3 sceneTextureColor = SceneTexture.Sample(SceneTextureSampler, input.TexCoord).rgb;

    float3 finalColor = float3(0.0f, 0.0f, 0.0f);

    // HDR tone mapping.
    finalColor = HDRToneMap(sceneTextureColor);

    // Gamma correction.
    finalColor = GammaCorrect(finalColor);

    return float4(finalColor.rgb, 1.0f);
}