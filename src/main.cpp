#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

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

sf::Vector3f productElementWise(const sf::Vector3f &a, const sf::Vector3f &b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

void capColorVector(sf::Vector3f &vector) {
    vector.x = std::max(0.0f, std::min(1.0f, vector.x));
    vector.y = std::max(0.0f, std::min(1.0f, vector.y));
    vector.z = std::max(0.0f, std::min(1.0f, vector.z));
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

constexpr int WIDTH = 600;
constexpr int HEIGHT = 600;

constexpr float BALL_DIAMETER = 0.8f;
constexpr float BALL_RADIUS = BALL_DIAMETER / 2.f;
constexpr float POSITION_DELTA = 0.1f;

const sf::Vector3f CAMERA_POSITION(0.0f, 0.0f, -1.0f);

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

sf::Sprite shadedBall;
sf::VertexArray vertices(sf::Points, WIDTH * HEIGHT);
sf::Color BG_COLOR{0xdb, 0xdc, 0xdc};

void draw(sf::RenderWindow &window,
          sf::RenderTexture &renderTexture,
          const sf::Vector3f &lightSourcePosition,
          const Material &material,
          const Light &light) {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            float nx = 2.f * x / WIDTH - 1.f;
            float ny = 2.f * y / HEIGHT - 1.f;
            float distanceSquaredXY = nx * nx + ny * ny;
            if (distanceSquaredXY <= BALL_RADIUS * BALL_RADIUS) {
                float nz = -std::sqrt(BALL_RADIUS * BALL_RADIUS - distanceSquaredXY);

                sf::Vector3f pixelPosition{nx, ny, nz};
                sf::Vector3f surfaceNormal = pixelPosition;

                sf::Vector3f towardLightSource = lightSourcePosition - pixelPosition;
                sf::Vector3f towardCamera = CAMERA_POSITION - pixelPosition;

                normalize(towardLightSource);
                normalize(surfaceNormal);
                normalize(towardCamera);
                sf::Vector3f reflectedRayDirection =
                        2 * dot(towardLightSource, surfaceNormal) * surfaceNormal - towardLightSource;

                normalize(reflectedRayDirection);
                sf::Vector3f shadingColor = calculatePhongLighting(towardLightSource,
                                                                   surfaceNormal,
                                                                   reflectedRayDirection,
                                                                   towardCamera,
                                                                   material,
                                                                   light);
                capColorVector(shadingColor);

                int index = y * WIDTH + x;
                vertices[index].position = sf::Vector2f(x, y);
                vertices[index].color = sf::Color(
                        shadingColor.x * 255.f,
                        shadingColor.y * 255.f,
                        shadingColor.z * 255.f
                );
            }
        }
    }

    renderTexture.clear(BG_COLOR);
    renderTexture.draw(vertices);
    renderTexture.display();
    shadedBall.setTexture(renderTexture.getTexture());
    window.draw(shadedBall);
    window.display();
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
            {0.05f, 0.05f, 0.05f},
            {0.6f, 0.6f, 0.6f},
            {0.6f, 0.6f, 0.6f},
            32.0f
    };
    Material materialPlastic{
            {0.0f, 0.0f, 0.0f},
            {0.01f, 0.01f, 0.01f},
            {0.5f, 0.5f, 0.5f},
            32.0f
    };
    Material materialWood{
            {0.05f, 0.05f, 0.0f},
            {0.4f, 0.25f, 0.1f},
            {0.04f, 0.04f, 0.04f},
            10.0f
    };

    auto materials = {polishedGold, materialWall, materialPlastic, materialWood};
    auto materialsIt = materials.begin();

    sf::RenderTexture renderTexture;

    renderTexture.create(WIDTH, HEIGHT);

    sf::Clock clock;

    sf::RectangleShape pixel(sf::Vector2f(1, 1));

    while (window.isOpen()) {
        clock.restart();
        handleEvents(window, lightSourcePosition, materialsIt, materials);
        draw(window, renderTexture, lightSourcePosition, *materialsIt, light);

        std::cout << 1 / clock.getElapsedTime().asSeconds() << "FPS" << std::endl;
    }

    return 0;
}
