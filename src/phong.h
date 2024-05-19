#ifndef PHONG_SHADING_PHONG_H
#define PHONG_SHADING_PHONG_H

#include "SFML/Graphics.hpp"
#include "vector_utils.h"

struct Light {
    sf::Vector3f ambient;
    sf::Vector3f diffuse;
    sf::Vector3f specular;
};

struct Material {
    sf::Vector3f ambient;
    sf::Vector3f diffuse;
    sf::Vector3f specular;
    float shininess{};
};

sf::Vector3f productElementWise(const sf::Vector3f &a, const sf::Vector3f &b);
// https://en.wikipedia.org/wiki/Phong_reflection_model
sf::Vector3f calculatePhongLighting(
        const sf::Vector3f &towardLightSource, // L
        const sf::Vector3f &surfaceNormal, // N
        const sf::Vector3f &reflectedRayDirection, // R
        const sf::Vector3f &towardCamera, // V
        const Material &material,
        const Light &light
);

#endif //PHONG_SHADING_PHONG_H
