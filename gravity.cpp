#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include<vector>

// Constants
const float timeStep = 1.0f / 60.0f;
const float bounceDamping = 0.8f;
const float minBounceVelocity = 5.0f;

struct PhysicsCircle {
    float mass;
    sf::Vector2f velocity;
    sf::CircleShape shape;

    PhysicsCircle(float m, float radius, float startX, float startY, sf::Texture* tex) {
        mass = m;
        velocity = sf::Vector2f(0.f, 0.f);

        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(startX, startY);

        // Load texture
        if (tex) {
            shape.setTexture(tex);
        }
        else {
            std::cout << "Null texture passed!\n";
            shape.setFillColor(sf::Color::Red);
        }
    }

    void applyForce(const sf::Vector2f& force) {
        sf::Vector2f acceleration = force / mass;
        velocity += acceleration * timeStep;
    }

    void update() {
        shape.move(velocity * timeStep);
    }

    float getBottom() const {
        return shape.getPosition().y + shape.getRadius();
    }

    void setBottom(float y) {
        shape.setPosition(shape.getPosition().x, y - shape.getRadius());
    }
};

int main() {
    // Get desktop mode for fullscreen resolution
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    // Create fullscreen window
    sf::RenderWindow window(desktop, "2D Bouncing Ball", sf::Style::Fullscreen);

    // Create the ground
    float groundY = desktop.height - 50.0f;
    sf::RectangleShape ground(sf::Vector2f(static_cast<float>(desktop.width), 10.f));
    ground.setPosition(0.f, groundY);
    ground.setFillColor(sf::Color::Green);

    //Data for the bodies
    const float gravities[10] = {
        274.0f,  // Sun
        3.7f,    // Mercury
        8.87f,   // Venus
        9.81f,   // Earth
        3.71f,   // Mars
        24.79f,  // Jupiter
        10.44f,  // Saturn
        8.69f,   // Uranus
        11.15f,  // Neptune
        1.62f    // Moon
    };
    const float masses[10] = {
       1.989e30f,
       3.3e23f,
       4.87e24f,
       5.97e24f,
       6.42e23f,
       1.9e27f,
       5.68e26f,
       8.68e25f,
       1.02e26f,
       7.35e22f
    };
    const float radii[10] = {
        150.f,  // Sun
        10.f,  // Mercury
        20.f,  // Venus
        22.f,  // Earth
        18.f,  // Mars
        70.f,  // Jupiter
        70.f,  // Saturn
        45.f,  // Uranus
        50.f,  // Neptune
        8.f   // Moon
    };
    std::string textureFiles[10] = {
       "sun.jpg",
       "mercury.jpg",
       "venus.jpg",
       "earth.jpg",
       "mars.jpg",
       "jupiter.jpg",
       "saturn.jpg",
       "uranus.jpg",
       "neptune.jpg",
       "moon.jpg"
    };

    std::vector<sf::Texture> textures(10);
    for (int i = 0; i < 10; ++i) {
        if (!textures[i].loadFromFile(textureFiles[i])) {
            std::cout << "Failed to load texture: " << textureFiles[i] << "\n";
        }
    }

    std::vector<PhysicsCircle> ball;
    std::vector<sf::Vector2f> initialPosition;
    //spacing between the bodies
    float startX = 100.f;
    float spacing = 200.f;
    for (int i = 0; i < 10; ++i) {
        float x = startX + i * spacing;
        float y = 100.0f;
        initialPosition.push_back(sf::Vector2f(x, y));
        ball.emplace_back(masses[i], radii[i], x, y, &textures[i]);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Allow Space key to rerun the gravity fall
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                for (int i = 0; i < 10; i++) {
                    ball[i].velocity = sf::Vector2f(0.f, 0.f);
                    ball[i].shape.setPosition(initialPosition[i]);
                }
            }
            // Allow ESC key to exit fullscreen
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        //Update each bodies
        for (int i = 0; i < 10; ++i) {
            // scale gravity down so balls fall at visible speed
            float scaledGravity = gravities[i] * 0.05f;

            sf::Vector2f gravityForce(0.f, ball[i].mass * scaledGravity);
            ball[i].applyForce(gravityForce);
            ball[i].update();

            // Ground collision and bounce
            if (ball[i].getBottom() >= groundY) {
                ball[i].setBottom(groundY);

                if (std::abs(ball[i].velocity.y) > minBounceVelocity) {
                    ball[i].velocity.y = -ball[i].velocity.y * bounceDamping;
                }
                else {
                    ball[i].velocity.y = 0;
                }
            }
        }

        // Draw
        window.clear(sf::Color::Black);
        window.draw(ground);
        for (const auto& bal : ball) {
            window.draw(bal.shape);
        }
        window.display();
    }

    return 0;
}
