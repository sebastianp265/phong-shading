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

// https://en.wikipedia.org/wiki/Phong_reflection_model
sf::Color calculatePhongLighting(
        const sf::Vector3f &towardLightSource, // L
        const sf::Vector3f &surfaceNormal, // N
        const sf::Vector3f &reflectedRayDirection, // R
        const sf::Vector3f &towardCamera, // V
        const sf::Color &objectColor,
        const sf::Color &lightColor
) {
    sf::Color totalIllumination{0, 0, 0};

    const float ka = 0.0;
    const sf::Color &ia = objectColor;

    const sf::Color illuminationAmbient{
            static_cast<sf::Uint8>(ka * ia.r),
            static_cast<sf::Uint8>(ka * ia.g),
            static_cast<sf::Uint8>(ka * ia.b)
    };

    totalIllumination += illuminationAmbient;

    const float kd = 0.01;
    const sf::Color &id = objectColor;

    const float lightDirNormalDotProduct = std::max(0.0f, dot(towardLightSource, surfaceNormal));

    const sf::Color illuminationDiffuse{
            static_cast<sf::Uint8>(kd * lightDirNormalDotProduct * id.r),
            static_cast<sf::Uint8>(kd * lightDirNormalDotProduct * id.g),
            static_cast<sf::Uint8>(kd * lightDirNormalDotProduct * id.b)
    };

    totalIllumination += illuminationDiffuse;
    if (lightDirNormalDotProduct > 0.0) {
        const float shininess = 27.8974;
        const float ks = 0.5;

        const sf::Color &is = lightColor;

        const float reflectedRayDirTowardCameraDotProduct =
                std::max(0.0f, dot(reflectedRayDirection, towardCamera));

        const float reflectedRayDirTowardCameraDotProductToPowShininess =
                std::pow(reflectedRayDirTowardCameraDotProduct, shininess);

        const sf::Color illuminationSpecular = {
                static_cast<sf::Uint8>(ks * reflectedRayDirTowardCameraDotProductToPowShininess * is.r),
                static_cast<sf::Uint8>(ks * reflectedRayDirTowardCameraDotProductToPowShininess * is.g),
                static_cast<sf::Uint8>(ks * reflectedRayDirTowardCameraDotProductToPowShininess * is.b)
        };

        totalIllumination += illuminationSpecular;
    }

    return totalIllumination;
}

constexpr int WIDTH = 600;
constexpr int HEIGHT = 600;

constexpr float BALL_DIAMETER = 0.8f;
constexpr float BALL_RADIUS = BALL_DIAMETER / 2.f;
constexpr float POSITION_DELTA = 0.1f;

const sf::Vector3f CAMERA_POSITION(0.0f, 0.0f, -1.0f);

sf::Event event{};

void handleEvents(sf::RenderWindow &window, sf::Vector3f &lightSourcePosition) {
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
                default:
                    break;
            }
        }
    }
}

sf::Sprite shadedBall;
sf::VertexArray vertices(sf::Points, WIDTH * HEIGHT);

void draw(sf::RenderWindow &window,
          sf::RenderTexture &renderTexture,
          const sf::Vector3f &lightSourcePosition,
          const sf::Color &objectColor,
          const sf::Color &lightColor) {
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
                sf::Color shadingColor = calculatePhongLighting(towardLightSource,
                                                                surfaceNormal,
                                                                reflectedRayDirection,
                                                                towardCamera,
                                                                objectColor,
                                                                lightColor);

                int index = y * WIDTH + x;
                vertices[index].position = sf::Vector2f(x, y);
                vertices[index].color = shadingColor;
            }
        }
    }

    renderTexture.clear();
    renderTexture.draw(vertices);
    renderTexture.display();
    shadedBall.setTexture(renderTexture.getTexture());
    window.draw(shadedBall);
    window.display();
}

int main() {
    sf::Vector3f lightSourcePosition(0.f, 0.f, -1.f);
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "project");

    sf::Color lightColor(255, 255, 255);
    sf::Color objectColor(0xF5, 0xBF, 0x03);

    sf::RenderTexture renderTexture;

    renderTexture.create(WIDTH, HEIGHT);

    sf::Clock clock;

    sf::RectangleShape pixel(sf::Vector2f(1, 1));

    while (window.isOpen()) {
        clock.restart();
        handleEvents(window, lightSourcePosition);
        draw(window, renderTexture, lightSourcePosition, objectColor, lightColor);

        std::cout << 1 / clock.getElapsedTime().asSeconds() << "FPS" << std::endl;
    }

    return 0;
}
