#include <SFML/Graphics.hpp>
#include <iostream>
#include "phong.h"
#include "consts.h"
#include "drawing.h"

sf::Event event{};

void handleEvents(sf::RenderWindow &window,
                  sf::Vector3f &lightSourcePosition,
                  std::initializer_list<Material>::iterator &it,
                  const std::initializer_list<Material> &materials) {
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::W:
                    lightSourcePosition.y -= POSITION_DELTA;
                    break;
                case sf::Keyboard::S:
                    lightSourcePosition.y += POSITION_DELTA;
                    break;
                case sf::Keyboard::D:
                    lightSourcePosition.x += POSITION_DELTA;
                    break;
                case sf::Keyboard::A:
                    lightSourcePosition.x -= POSITION_DELTA;
                    break;
                case sf::Keyboard::Q:
                    lightSourcePosition.z += POSITION_DELTA;
                    break;
                case sf::Keyboard::E:
                    lightSourcePosition.z -= POSITION_DELTA;
                    break;
                case sf::Keyboard::Right:
                    if (it != std::prev(materials.end())) {
                        ++it;
                    }
                    break;
                case sf::Keyboard::Left:
                    if (it != materials.begin()) {
                        --it;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

int main() {
    sf::Vector3f lightSourcePosition(0.f, 0.f, -2.f);
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "project");

    Light light{{1., 1., 1.},
                {1., 1., 1.},
                {1., 1., 1.}};

    Material polishedGold{
            {0.24725f, 0.2245f, 0.0645f},
            {0.34615f, 0.3143f,0.0903f},
            {0.797357f, 0.723991f,0.208006f},
            83.2
    };
    Material materialWall{
            {0.2f, 0.2f, 0.2f},
            {0.5f, 0.5f, 0.5f},
            {0.1f, 0.1f, 0.1f},
            10.0f
    };
    Material materialPlastic{
            {0.0f, 0.0f, 0.0f},
            {0.01f, 0.01f, 0.01f},
            {0.5f, 0.5f, 0.5f},
            32.0f
    };
    Material materialWood{
            {0.2f, 0.1f, 0.05f},
            {0.4f, 0.2f, 0.1f},
            {0.1f, 0.1f, 0.1f},
            10.0f
    };

    auto materials = {polishedGold, materialWall, materialPlastic, materialWood};
    auto materialsIt = materials.begin();

    sf::RenderTexture renderTexture;

    renderTexture.create(WIDTH, HEIGHT);

    sf::Clock clock;

    while (window.isOpen()) {
        clock.restart();
        handleEvents(window, lightSourcePosition, materialsIt, materials);
        draw(window, renderTexture, lightSourcePosition, *materialsIt, light);

        std::cout << 1 / clock.getElapsedTime().asSeconds() << "FPS" << std::endl;
    }

    return 0;
}
