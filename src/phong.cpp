#include "phong.h"

sf::Vector3f productElementWise(const sf::Vector3f &a, const sf::Vector3f &b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

// https://en.wikipedia.org/wiki/Phong_reflection_model
sf::Vector3f calculatePhongLighting(
        const sf::Vector3f &towardLightSource, // L
        const sf::Vector3f &surfaceNormal, // N
        const sf::Vector3f &reflectedRayDirection, // R
        const sf::Vector3f &towardCamera, // V
        const Material &material,
        const Light &light
) {
    sf::Vector3f totalIllumination{0, 0, 0};

    totalIllumination += productElementWise(material.ambient, light.ambient);

    const float lightDirNormalDotProduct = dot(towardLightSource, surfaceNormal);
    if (lightDirNormalDotProduct <= 0) {
        return totalIllumination;
    }
    totalIllumination += lightDirNormalDotProduct * productElementWise(material.diffuse, light.diffuse);

    const float reflectedRayDirTowardCameraDotProduct = dot(reflectedRayDirection, towardCamera);
    if (reflectedRayDirTowardCameraDotProduct <= 0) {
        return totalIllumination;
    }
    const float reflectedRayDirTowardCameraDotProductToPowShininess =
            std::pow(reflectedRayDirTowardCameraDotProduct, material.shininess);
    totalIllumination += reflectedRayDirTowardCameraDotProductToPowShininess * productElementWise(material.specular,
                                                                                                  light.specular);
    return totalIllumination;
}
