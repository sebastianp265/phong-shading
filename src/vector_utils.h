#ifndef PHONG_SHADING_VECTOR_UTILS_H
#define PHONG_SHADING_VECTOR_UTILS_H

#include <SFML/Graphics.hpp>
#include <cmath>

float dot(const sf::Vector3f &v1, const sf::Vector3f &v2);
void normalize(sf::Vector3f &v);

#endif //PHONG_SHADING_VECTOR_UTILS_H
