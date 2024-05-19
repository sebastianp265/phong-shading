#ifndef PHONG_SHADING_CONSTS_H
#define PHONG_SHADING_CONSTS_H
#include <SFML/Graphics.hpp>

constexpr int WIDTH = 600;
constexpr int HEIGHT = 600;

constexpr float BALL_DIAMETER = 0.8f;
constexpr float BALL_RADIUS = BALL_DIAMETER / 2.f;
constexpr float POSITION_DELTA = 0.1f;

const sf::Vector3f CAMERA_POSITION(0.0f, 0.0f, -1.0f);

#endif //PHONG_SHADING_CONSTS_H
