#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:
    Player();
    void reset();
    void update();
    void jump(float force = -12.0f);

    sf::Sprite sprite;
    sf::Vector2f velocity;
    bool isAlive;
    bool isGrounded;
    float gravity;

private:
    sf::Texture texture;
};