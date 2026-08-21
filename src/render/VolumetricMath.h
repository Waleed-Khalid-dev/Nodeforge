#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace nf {

class VolumetricMath {
public:
    static float BeerLambert(float extinctionCoeff, float density, float stepLength);
    static float HenyeyGreenstein(float cosTheta, float g);
    static float SimplexNoise3D(const glm::vec3& p);
    static float WorleyNoise3D(const glm::vec3& p);
    static float SampleDensity(const glm::vec3& p, float timeSeconds, float windSpeed);

    static glm::vec4 RayMarchClouds(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDir,
        const glm::vec3& sunDir,
        const glm::vec4& sunColor,
        float densityScale,
        float g,
        int stepCount,
        float timeSeconds,
        float windSpeed
    );
};

} // namespace nf
