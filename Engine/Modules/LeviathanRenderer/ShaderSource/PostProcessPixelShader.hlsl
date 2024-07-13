struct PixelInput
{
    float4 PositionClipSpace : SV_POSITION;
    float2 TexCoord : TEX_COORD;
};

Texture2D SceneTexture : register(t0);
SamplerState SceneTextureSampler : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
    float3 sceneTextureColor = SceneTexture.Sample(SceneTextureSampler, input.TexCoord).rgb;
    return float4(sceneTextureColor.rgb, 1.0f);
}