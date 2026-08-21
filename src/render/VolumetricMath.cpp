#include "VolumetricMath.h"
#include <cmath>
#include <algorithm>

namespace nf {

float VolumetricMath::BeerLambert(float extinctionCoeff, float density, float stepLength) {
    return std::exp(-extinctionCoeff * density * stepLength);
}

float VolumetricMath::HenyeyGreenstein(float cosTheta, float g) {
    g = std::clamp(g, -0.99f, 0.99f);
    cosTheta = std::clamp(cosTheta, -1.0f, 1.0f);
    float num = 1.0f - g * g;
    float denom = 4.0f * 3.141592653589793f * std::pow(1.0f + g * g - 2.0f * g * cosTheta, 1.5f);
    return (denom > 1e-6f) ? (num / denom) : 0.0f;
}

static float Hash3D(int x, int y, int z) {
    int n = x + y * 57 + z * 131;
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float VolumetricMath::SimplexNoise3D(const glm::vec3& p) {
    int ix = static_cast<int>(std::floor(p.x));
    int iy = static_cast<int>(std::floor(p.y));
    int iz = static_cast<int>(std::floor(p.z));

    float fx = p.x - static_cast<float>(ix);
    float fy = p.y - static_cast<float>(iy);
    float fz = p.z - static_cast<float>(iz);

    // Smoothstep
    float ux = fx * fx * (3.0f - 2.0f * fx);
    float uy = fy * fy * (3.0f - 2.0f * fy);
    float uz = fz * fz * (3.0f - 2.0f * fz);

    float n000 = Hash3D(ix, iy, iz);
    float n100 = Hash3D(ix + 1, iy, iz);
    float n010 = Hash3D(ix, iy + 1, iz);
    float n110 = Hash3D(ix + 1, iy + 1, iz);
    float n001 = Hash3D(ix, iy, iz + 1);
    float n101 = Hash3D(ix + 1, iy, iz + 1);
    float n011 = Hash3D(ix, iy + 1, iz + 1);
    float n111 = Hash3D(ix + 1, iy + 1, iz + 1);

    float nx00 = n000 + ux * (n100 - n000);
    float nx10 = n010 + ux * (n110 - n010);
    float nx01 = n001 + ux * (n101 - n001);
    float nx11 = n011 + ux * (n111 - n011);

    float nxy0 = nx00 + uy * (nx10 - nx00);
    float nxy1 = nx01 + uy * (nx11 - nx01);

    return nxy0 + uz * (nxy1 - nxy0);
}

float VolumetricMath::WorleyNoise3D(const glm::vec3& p) {
    int ix = static_cast<int>(std::floor(p.x));
    int iy = static_cast<int>(std::floor(p.y));
    int iz = static_cast<int>(std::floor(p.z));

    float minDist = 1.0f;
    for (int z = -1; z <= 1; ++z) {
        for (int y = -1; y <= 1; ++y) {
            for (int x = -1; x <= 1; ++x) {
                int cx = ix + x;
                int cy = iy + y;
                int cz = iz + z;
                glm::vec3 cellPos(
                    static_cast<float>(cx) + (Hash3D(cx, cy, cz) * 0.5f + 0.5f),
                    static_cast<float>(cy) + (Hash3D(cx + 17, cy, cz) * 0.5f + 0.5f),
                    static_cast<float>(cz) + (Hash3D(cx, cy + 31, cz) * 0.5f + 0.5f)
                );
                float dist = glm::length(cellPos - p);
                minDist = std::min(minDist, dist);
            }
        }
    }
    return minDist;
}

float VolumetricMath::SampleDensity(const glm::vec3& p, float timeSeconds, float windSpeed) {
    glm::vec3 offset(timeSeconds * windSpeed * 0.1f, 0.0f, timeSeconds * windSpeed * 0.05f);
    glm::vec3 samplePos = p * 0.2f + offset;

    float simplex = SimplexNoise3D(samplePos) * 0.5f + 0.5f;
    float worley = 1.0f - WorleyNoise3D(samplePos * 2.0f);

    float density = simplex * worley;
    // Bounding height gradient
    float heightFade = std::clamp((p.y - 1.0f) / 4.0f, 0.0f, 1.0f) * std::clamp((10.0f - p.y) / 4.0f, 0.0f, 1.0f);
    return std::max(0.0f, density * heightFade);
}

glm::vec4 VolumetricMath::RayMarchClouds(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    const glm::vec3& sunDir,
    const glm::vec4& sunColor,
    float densityScale,
    float g,
    int stepCount,
    float timeSeconds,
    float windSpeed
) {
    float stepLength = 10.0f / static_cast<float>(std::max(stepCount, 1));
    float cosTheta = glm::dot(glm::normalize(rayDir), glm::normalize(sunDir));
    float phase = HenyeyGreenstein(cosTheta, g);

    glm::vec3 accumulatedColor(0.0f);
    float transmittance = 1.0f;
    const float extinctionCoeff = 0.5f;

    for (int i = 0; i < stepCount; ++i) {
        float t = static_cast<float>(i) * stepLength + 1.0f;
        glm::vec3 pos = rayOrigin + rayDir * t;
        float density = SampleDensity(pos, timeSeconds, windSpeed) * densityScale;

        if (density > 0.001f) {
            float stepTransmittance = BeerLambert(extinctionCoeff, density, stepLength);
            float shadowMarchTransmittance = BeerLambert(extinctionCoeff, density * 0.5f, 1.0f);

            glm::vec3 lightScattering = glm::vec3(sunColor) * phase * shadowMarchTransmittance;
            glm::vec3 stepColor = lightScattering * (1.0f - stepTransmittance);

            accumulatedColor += stepColor * transmittance;
            transmittance *= stepTransmittance;

            if (transmittance < 0.01f) break;
        }
    }

    return glm::vec4(accumulatedColor, 1.0f - transmittance);
}

} // namespace nf
