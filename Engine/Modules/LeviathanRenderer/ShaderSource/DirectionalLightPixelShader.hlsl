#define TEXTURE2D_SRV_TABLE_LENGTH 5
#define TEXTURE_SAMPLER_TABLE_LENGTH 4

#define COLOR_TEXTURE2D_SRV_TABLE_INDEX 0
#define ROUGHNESS_TEXTURE2D_SRV_TABLE_INDEX 1
#define METALLIC_TEXTURE2D_SRV_TABLE_INDEX 2
#define NORMAL_TEXTURE2D_SRV_TABLE_INDEX 3

#define COLOR_TEXTURE_SAMPLER_TABLE_INDEX 0
#define ROUGHNESS_TEXTURE_SAMPLER_TABLE_INDEX 1
#define METALLIC_TEXTURE_SAMPLER_TABLE_INDEX 2
#define NORMAL_TEXTURE_SAMPLER_TABLE_INDEX 3

#define PI 3.14159265359

cbuffer DirectionalLightBuffer : register(b0)
{
    float3 Radiance;
    float3 LightDirectionViewSpace;
}

// Pixel shader input.
struct PixelInput
{
    float4 PositionClipSpace : SV_POSITION;
    float3 PositionViewSpace : POSITION_VIEW_SPACE;
    float3 PositionTangentSpace : POSITION_TANGENT_SPACE;
    float3 VertexNormalViewSpace : VERTEX_NORMAL_VIEW_SPACE;
    float2 TexCoord : TEXTURE_COORD;
    float3 LightDirectionTangentSpace : LIGHT_DIRECTION_TANGENT_SPACE;
};

// Texture2D SRV table.
Texture2D Texture2DSRVTable[TEXTURE2D_SRV_TABLE_LENGTH] : register(t0);

// Sampler state table.
SamplerState TextureSamplerTable[TEXTURE_SAMPLER_TABLE_LENGTH] : register(s0);

float Square(float x)
{
    return x * x;
}

float3 Lambert(float3 color)
{
    return color / PI;
}

float3 DiffuseBRDF(float kD, float3 lambert)
{
    return kD * lambert;
}

float TrowbridgeReitzGGX(float roughness, float3 halfVector, float3 surfaceNormal)
{
    float roughnessSquared = Square(roughness) * Square(roughness); // Roughness is raised to the power of 4 instead of 2 based on the experience of Disney and Epic Games.
    return roughnessSquared / (PI * Square((Square(dot(surfaceNormal, halfVector)) * (roughnessSquared - 1) + 1)));
}

float SchlickGGX(float nDotV, float k)
{
    return nDotV / (nDotV * (1.0f - k) + k);
}

float Smith(float nDotV, float nDotL, float roughness)
{
    float k = Square(roughness + 1.0f) / 8.0f;
    return SchlickGGX(nDotV, k) * SchlickGGX(nDotL, k);
}

float3 SchlickFresnel(float vDotH, float3 surfaceColor, float metallic)
{
    float3 f0 = float3(0.04f, 0.04f, 0.04f);
    f0 = lerp(f0, surfaceColor, metallic);
    return f0 + (1.0f - f0) * pow(saturate(1.0f - vDotH), 5.0f);
}

float3 CalculateLighting(float3 surfaceToLightDirection, float3 surfaceToViewDirection, float3 surfaceNormal, float nDotV, float3 radiance, float3 baseColor, float roughness, float metallic)
{
    float3 halfDirection = normalize(surfaceToViewDirection + surfaceToLightDirection);
        
    float nDotH = saturate(dot(surfaceNormal, halfDirection));
    float vDotH = saturate(dot(surfaceToViewDirection, halfDirection));
    float nDotL = saturate(dot(surfaceNormal, surfaceToLightDirection));

    float3 f = SchlickFresnel(vDotH, baseColor, metallic);
    float3 kS = f;
    float3 kD = 1.0f - kS;
        
    float3 diffuse = kD * Lambert(baseColor);
        
    float d = TrowbridgeReitzGGX(roughness, halfDirection, surfaceNormal);
    float g = Smith(nDotV, nDotL, roughness);
    float3 specular = (d * g * f) / ((4.0f * nDotL * nDotV) + 0.0001f);

    return (diffuse + specular) * radiance * nDotL;
}

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

    // Directional light.
    float3 surfaceToLightDirectionTangentSpace = -input.LightDirectionTangentSpace;
    float3 color = CalculateLighting(surfaceToLightDirectionTangentSpace, surfaceToViewDirectionTangentSpace, surfaceNormal, nDotV, Radiance, baseColor, roughness, metallic);

    return float4(color.rgb, 1.0f);
}