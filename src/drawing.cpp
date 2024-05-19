#include "drawing.h"

void capColorVector(sf::Vector3f &vector) {
    vector.x = std::max(0.0f, std::min(1.0f, vector.x));
    vector.y = std::max(0.0f, std::min(1.0f, vector.y));
    vector.z = std::max(0.0f, std::min(1.0f, vector.z));
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