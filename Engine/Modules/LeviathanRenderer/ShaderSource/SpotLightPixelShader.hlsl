#include "Lighting.hlsl"

cbuffer SpotLightBuffer : register(b0)
{
    float3 Radiance;
    float3 LightPositionViewSpace;
    float3 LightDirectionViewSpace;
    float CosineInnerConeAngle;
    float CosineOuterConeAngle;
}

// Pixel shader input.
struct PixelInput
{
    float4 PositionClipSpace : SV_POSITION;
    float3 PositionViewSpace : POSITION_VIEW_SPACE;
    float3 PositionTangentSpace : POSITION_TANGENT_SPACE;
    float3 VertexNormalViewSpace : VERTEX_NORMAL_VIEW_SPACE;
    float2 TexCoord : TEXTURE_COORD;
    float3 SurfaceToLightVectorTangentSpace : SURFACE_TO_LIGHT_VECTOR_TANGENT_SPACE;
    float3 LightDirectionTangentSpace : LIGHT_DIRECTION_TANGENT_SPACE;
};

// Texture2D SRV table.
Texture2D Texture2DSRVTable[TEXTURE2D_SRV_TABLE_LENGTH] : register(t0);

// Sampler state table.
SamplerState TextureSamplerTable[TEXTURE_SAMPLER_TABLE_LENGTH] : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
     // Final color = Cook Torrance BRDF * Light intensity * nDotL
        
     // Cook Torrance BRDF = (kD * fLambert) + (kS * fCookTorrance)
     // kD + kS = 1
     // fLambert = base color / pi
     // fCookTorrance = d * f * g / 4 * dot(surface normal, surface to light direction) * dot(surface normal, surface to view direction)
     // d (Normal distribution function): GGX by Trowbridge & Reitz.
     // g (Geometry function): Schlick-GGX by Schlick & Beckmann using Smith's method.
     // f (Fresnel function): Schlick approximation.
        
     // Light intensity = light radiance
     // nDotL = dot(surface normal, surface to light direction)
    
    float3 baseColor = Texture2DSRVTable[COLOR_TEXTURE2D_SRV_TABLE_INDEX].Sample(TextureSamplerTable[COLOR_TEXTURE_SAMPLER_TABLE_INDEX], input.TexCoord.xy).rgb;
    float roughness = Texture2DSRVTable[ROUGHNESS_TEXTURE2D_SRV_TABLE_INDEX].Sample(TextureSamplerTable[ROUGHNESS_TEXTURE_SAMPLER_TABLE_INDEX], input.TexCoord.xy).r;
    float metallic = Texture2DSRVTable[METALLIC_TEXTURE2D_SRV_TABLE_INDEX].Sample(TextureSamplerTable[METALLIC_TEXTURE_SAMPLER_TABLE_INDEX], input.TexCoord.xy).r;
    float3 surfaceNormal = normalize(Texture2DSRVTable[NORMAL_TEXTURE2D_SRV_TABLE_INDEX].Sample(TextureSamplerTable[NORMAL_TEXTURE_SAMPLER_TABLE_INDEX], input.TexCoord.xy).xyz * 2.0f - 1.0f);
    
    float3 surfaceToViewDirectionTangentSpace = normalize(-input.PositionTangentSpace);
    float nDotV = saturate(dot(surfaceNormal, surfaceToViewDirectionTangentSpace));
    
    // Spot light.
    float3 surfaceToLightDirectionTangentSpace = normalize(input.SurfaceToLightVectorTangentSpace);
    float theta = saturate(dot(-surfaceToLightDirectionTangentSpace, input.LightDirectionTangentSpace));
    float epsilon = CosineInnerConeAngle - CosineOuterConeAngle;
    float intensity = smoothstep(0.0f, 1.0f, saturate((theta - CosineOuterConeAngle) / epsilon));
    float attenuation = Attenuation(length(input.SurfaceToLightVectorTangentSpace));
    float3 radiance = attenuation * intensity * Radiance;
    float3 color = CalculateLighting(normalize(input.SurfaceToLightVectorTangentSpace), surfaceToViewDirectionTangentSpace, surfaceNormal, nDotV, radiance, baseColor, roughness, metallic);

    return float4(color.rgb, 1.0f);
}