#ifndef PHONG_SHADING_DRAWING_H
#define PHONG_SHADING_DRAWING_H

#include <SFML/Graphics.hpp>
#include "consts.h"
#include "phong.h"

void capColorVector(sf::Vector3f &vector);
void draw(sf::RenderWindow &window,
          sf::RenderTexture &renderTexture,
          const sf::Vector3f &lightSourcePosition,
          const Material &material,
          const Light &light);

#endif //PHONG_SHADING_DRAWING_H
