#include "vector_utils.h"

float dot(const sf::Vector3f &v1, const sf::Vector3f &v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

void normalize(sf::Vector3f &v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length != 0) {
        v.x /= length;
        v.y /= length;
        v.z /= length;
    }
}