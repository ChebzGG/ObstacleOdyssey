#include "Player.h"

Player::Player() {
    if (!texture.loadFromFile("assets/images/player.png")) {
        std::cerr << "Failed to load player texture\n";
    }
    sprite.setTexture(texture);
    sprite.setScale(0.8f, 0.8f);
    reset();
}

void Player::reset() {
    sprite.setPosition(100, 300);
    velocity = { 0, 0 };
    isAlive = true;
    gravity = 0.5f;
    isGrounded = false;
}

void Player::update() {
    if (!isAlive) return;
    velocity.y += gravity;
    if (velocity.y > 15) velocity.y = 15;
    sprite.move(velocity);
}

void Player::jump(float force) {
    if (isGrounded) {
        velocity.y = force;
        isGrounded = false;
    }
}