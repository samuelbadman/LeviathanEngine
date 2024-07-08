// Renderer constants.
#define MAX_DIRECTIONAL_LIGHT_COUNT 10
#define MAX_POINT_LIGHT_COUNT 10
#define MAX_SPOT_LIGHT_COUNT 10

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

// Shader definitions.
#define PI 3.14159265359

// Pixel shader input.
struct PixelInput
{
    float4 PositionClipSpace : SV_POSITION;
    float3 PositionViewSpace : POSITION_VIEW_SPACE;
    float3 PositionTangentSpace : POSITION_TANGENT_SPACE;
    float3 VertexNormalViewSpace : VERTEX_NORMAL_VIEW_SPACE;
    float2 TexCoord : TEXTURE_COORD;
    float3 Radiance : RADIANCE;
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

float Attenuation(float distance)
{
    return 1.0f / Square(distance);
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
    float3 halfDirectionViewSpace = normalize(surfaceToViewDirection + surfaceToLightDirection);
        
    float nDotH = saturate(dot(surfaceNormal, halfDirectionViewSpace));
    float vDotH = saturate(dot(surfaceToViewDirection, halfDirectionViewSpace));
    float nDotL = saturate(dot(surfaceNormal, surfaceToLightDirection));

    float3 f = SchlickFresnel(vDotH, baseColor, metallic);
    float3 kS = f;
    float3 kD = 1.0f - kS;
        
    float3 diffuse = kD * Lambert(baseColor);
        
    float d = TrowbridgeReitzGGX(roughness, halfDirectionViewSpace, surfaceNormal);
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
    
    float3 totalColor = float3(0.0f, 0.0f, 0.0f);

    float3 surfaceToViewDirectionTangentSpace = normalize(-input.PositionTangentSpace);
    float nDotV = saturate(dot(surfaceNormal, surfaceToViewDirectionTangentSpace));

    // Directional light.
    float3 surfaceToLightDirectionTangentSpace = -input.LightDirectionTangentSpace;
    totalColor += CalculateLighting(surfaceToLightDirectionTangentSpace, surfaceToViewDirectionTangentSpace, surfaceNormal, nDotV, input.Radiance, baseColor, roughness, metallic);
    
    // Point light.
    //for (int j = 0; j < PointLightCount; ++j)
    //{
    //    float3 surfaceToLightVectorViewSpace = PointLights[j].PositionViewSpace - input.PositionViewSpace;
    //    float attenuation = Attenuation(length(surfaceToLightVectorViewSpace));
    //    float3 radiance = attenuation * PointLights[j].Radiance;
    //    totalColor += CalculateLighting(normalize(surfaceToLightVectorViewSpace), surfaceToViewDirectionViewSpace, surfaceNormal, nDotV, radiance, baseColor, roughness, metallic);
    //}
    
    // Spot light.
    //for (int k = 0; k < SpotLightCount; ++k)
    //{
    //    float3 surfaceToLightVectorViewSpace = SpotLights[k].PositionViewSpace - input.PositionViewSpace;
    //    float3 surfaceToLightDirectionViewSpace = normalize(surfaceToLightVectorViewSpace);
    //    float theta = saturate(dot(-surfaceToLightDirectionViewSpace, SpotLights[k].DirectionViewSpace));
    //    float epsilon = SpotLights[k].CosineInnerConeAngle - SpotLights[k].CosineOuterConeAngle;
    //    float intensity = smoothstep(0.0f, 1.0f, saturate((theta - SpotLights[k].CosineOuterConeAngle) / epsilon));
    //    float attenuation = Attenuation(length(surfaceToLightVectorViewSpace));
    //    float3 radiance = attenuation * intensity * SpotLights[k].Radiance;
    //    totalColor += CalculateLighting(surfaceToLightDirectionViewSpace, surfaceToViewDirectionViewSpace, surfaceNormal, nDotV, radiance, baseColor, roughness, metallic);
    //}
    
    // HDR tone mapping.
    totalColor = totalColor / (totalColor + float3(1.0f, 1.0f, 1.0f));

    // Gamma correction.
    float gammaExponent = 1.0f / 2.2f;
    float4 finalColor = float4(pow(totalColor, float3(gammaExponent, gammaExponent, gammaExponent)), 1.0f);

    return finalColor;
}