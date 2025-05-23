// Level.cpp
#include "Level.h"
#include <fstream>
#include <iostream>
#include <cmath>

Level::Level(const std::string& levelFile, const std::string& musicFile)
    : levelPath(levelFile), musicPath(musicFile) {

    int levelNumber = 1;
    if (levelPath == "levels/level1.txt") levelNumber = 1;
    else if (levelPath == "levels/level2.txt") levelNumber = 2;
    else if (levelPath == "levels/level3.txt") levelNumber = 3;
    else if (levelPath == "levels/level4.txt") levelNumber = 4;
    else if (levelPath == "levels/level5.txt") levelNumber = 5;

    std::string bgPath = "assets/images/level" + std::to_string(levelNumber) + ".png";
    std::string mapPath = "levels/level" + std::to_string(levelNumber) + ".txt";
    std::string musicPathLocal = "assets/music/level" + std::to_string(levelNumber) + ".ogg";

    backgroundTexture.loadFromFile(bgPath);
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(1.0f, 1.0f);

    levelPath = mapPath;
    load();

    if (music.openFromFile(musicPathLocal)) {
        musicLoaded = true;
    }
    else {
        std::cerr << "Failed to load level music: " << musicPathLocal << std::endl;
    }

    iconTex.loadFromFile("assets/images/icon.png");
    icon.setTexture(iconTex);
    icon.setScale(1.0f, 1.0f);
    icon.setOrigin(icon.getLocalBounds().width / 2.f, icon.getLocalBounds().height / 2.f);
    icon.setPosition(200.f, 880.f);

    iconHitbox.setSize(sf::Vector2f(icon.getGlobalBounds().width, icon.getGlobalBounds().height));
    iconHitbox.setFillColor(sf::Color::Transparent);
    iconHitbox.setOrigin(iconHitbox.getSize() / 2.f);
    iconHitbox.setPosition(icon.getPosition());

    iconBlockHitbox.setSize(iconHitbox.getSize() / 5.f);
    iconBlockHitbox.setFillColor(sf::Color::Transparent);
    iconBlockHitbox.setOrigin(iconBlockHitbox.getSize() / 2.f);
    iconBlockHitbox.setPosition(icon.getPosition());

    iconDefaultY = static_cast<int>(icon.getPosition().y);
    iconGroundDefaultY = iconDefaultY;

    spikeTex.loadFromFile("assets/images/default spike.png");
    spike.setTexture(spikeTex);
    spike.setScale(0.5f, 0.5f);
    spike.setOrigin(spike.getLocalBounds().width / 2.f, spike.getLocalBounds().height / 2.f);

    shortSpikeTex.loadFromFile("assets/images/short spike.png");
    shortSpike.setTexture(shortSpikeTex);
    shortSpike.setScale(0.5f, 0.5f);
    shortSpike.setOrigin(shortSpike.getLocalBounds().width / 2.f, shortSpike.getLocalBounds().height / 2.f);

    blockTex.loadFromFile("assets/images/default block.png");
    block.setTexture(blockTex);
    block.setScale(0.48f, 0.48f);
    block.setOrigin(40.f, 61.f);

    endwallTex.loadFromFile("assets/images/endwall.png");
    endwall.setTexture(endwallTex);
    endwall.setOrigin(endwall.getLocalBounds().width, endwall.getLocalBounds().height);

    jumpPadTex.loadFromFile("assets/images/jump_pad.png");
    jumpPad.setTexture(jumpPadTex);
    jumpPad.setScale(0.5f, 0.5f);
    jumpPad.setOrigin(jumpPad.getLocalBounds().width / 2.f, jumpPad.getLocalBounds().height / 2.f);

    orbTex.loadFromFile("assets/images/orb.png");
    orb.setTexture(orbTex);
    orb.setScale(0.5f, 0.5f);
    orb.setOrigin(orb.getLocalBounds().width / 2.f, orb.getLocalBounds().height / 2.f);

    coinTex.loadFromFile("assets/images/coin.png");
    coin.setTexture(coinTex);
    coin.setScale(0.5f, 0.5f);
    coin.setOrigin(coin.getLocalBounds().width / 2.f, coin.getLocalBounds().height / 2.f);

    groundLine.setSize(sf::Vector2f(1920.f, 3.f));
    groundLine.setPosition(0.f, icon.getGlobalBounds().top + icon.getGlobalBounds().height);

    deathBuffer.loadFromFile("assets/sounds/death.ogg");
    death.setBuffer(deathBuffer);

    coinBuffer.loadFromFile("assets/sounds/coin.ogg");
    coinSound.setBuffer(coinBuffer);

    jumpBuffer.loadFromFile("assets/sounds/jump.ogg");
    jumpSound.setBuffer(jumpBuffer);
}

Level::~Level() {
    if (musicLoaded) {
        music.stop();
    }
}

void Level::load() {
    setLevel();
    setEndPosition();
    resetPlayer();
}

void Level::setLevel() {
    std::ifstream file(levelPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << levelPath << std::endl;
        return;
    }
    char input;
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            file >> input;
            level[i][j] = input;
        }
    }
    file.close();
}

void Level::setEndPosition() {
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            if (level[i][j] == 's' || level[i][j] == 'b' || level[i][j] == 'h' || level[i][j] == 'd' || level[i][j] == 'o') {
                endPos = i;
            }
        }
    }
}

void Level::update() {
    if (completed || failed) return;

    handleInput();
    handlePhysics();
    checkCollisions();

    if (icon.getPosition().x >= 1920 + 50) {
        completed = true;
        stopMusic();
    }
}

void Level::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        // Check for orb collision when jumping
        for (int i = 0; i < SIZE_X; i++) {
            for (int j = 0; j < SIZE_Y; j++) {
                if (level[i][j] == 'o') {
                    float drawX = (i + 1) * icon.getGlobalBounds().width - levelPos + icon.getPosition().x;
                    float drawY = iconGroundDefaultY - j * icon.getGlobalBounds().height;
                    orb.setPosition(drawX + 10, drawY);

                    if (orb.getGlobalBounds().intersects(iconHitbox.getGlobalBounds())) {
                        jumpCheck = true;
                        jumpCount = 0; // Reset jump for higher jump
                        jumpSound.play();
                        level[i][j] = '-'; // Remove orb after use
                    }
                }
            }
        }

        if (isGrounded) {
            jumpCheck = true;
        }
    }
}

void Level::handlePhysics() {
    if (jumpCheck && iconAlive) {
        if (jumpCount++ == 0) isGrounded = false;
        jumpHeight = 1.0f * (0 - jumpCount) * (jumpCount - 26);
    }
    else if (!isGrounded) {
        jumpCount++;
        jumpHeight = 0.8f * (0 - jumpCount) * jumpCount;
    }
    icon.setPosition(icon.getPosition().x, iconDefaultY - jumpHeight);
    iconHitbox.setPosition(icon.getPosition());
    iconBlockHitbox.setPosition(icon.getPosition());

    if (isGrounded) {
        jumpCheck = false;
        jumpCount = 0;
        icon.setRotation(0);
    }
    else {
        icon.rotate(180.0f / 26);
    }

    levelPos += levelSpeed;
}

void Level::checkCollisions() {
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            float drawX = (i + 1) * icon.getGlobalBounds().width - levelPos + icon.getPosition().x;
            float drawY = iconGroundDefaultY - j * icon.getGlobalBounds().height;

            if (level[i][j] == 's') {
                spike.setPosition(drawX + 10, drawY);
                if (spike.getGlobalBounds().intersects(iconHitbox.getGlobalBounds())) {
                    iconAlive = false;
                    failed = true;
                    stopMusic();
                    death.play();
                    return;
                }
            }
            else if (level[i][j] == 'b') {
                block.setPosition(drawX + 10, drawY);
                if (block.getGlobalBounds().intersects(iconBlockHitbox.getGlobalBounds())) {
                    iconAlive = false;
                    failed = true;
                    stopMusic();
                    death.play();
                    return;
                }
                if (block.getGlobalBounds().intersects(iconHitbox.getGlobalBounds())) {
                    isGrounded = true;
                    int iconDefaultY = static_cast<int>(std::round(icon.getPosition().y));
                }
            }
            else if (level[i][j] == 'h') {
                shortSpike.setPosition(drawX + 10, drawY + 10);
                if (shortSpike.getGlobalBounds().intersects(iconHitbox.getGlobalBounds())) {
                    iconAlive = false;
                    failed = true;
                    stopMusic();
                    death.play();
                    return;
                }
            }
            else if (level[i][j] == 'd') {
                jumpPad.setPosition(drawX + 10, drawY);
                if (jumpPad.getGlobalBounds().intersects(iconHitbox.getGlobalBounds())) {
                    jumpCheck = true;
                    jumpCount = -10; // Big jump
                    jumpSound.play();
                }
            }
            else if (level[i][j] == 'c') {
                coin.setPosition(drawX + 10, drawY);
                if (coin.getGlobalBounds().intersects(iconHitbox.getGlobalBounds())) {
                    level[i][j] = '-'; // Remove coin
                    coinsCollected++;
                    coinSound.play();
                }
            }
        }
    }

    if (icon.getPosition().y > 1080 || icon.getPosition().y < 0) {
        failed = true;
        stopMusic();
    }
}

void Level::render(sf::RenderWindow& window) {
    window.draw(backgroundSprite);

    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            float drawX = (i + 1) * icon.getGlobalBounds().width - levelPos + icon.getPosition().x;
            float drawY = iconGroundDefaultY - j * icon.getGlobalBounds().height;

            if (level[i][j] == 's') {
                spike.setPosition(drawX + 10, drawY);
                window.draw(spike);
            }
            else if (level[i][j] == 'b') {
                block.setPosition(drawX + 10, drawY);
                window.draw(block);
            }
            else if (level[i][j] == 'h') {
                shortSpike.setPosition(drawX + 10, drawY + 10);
                window.draw(shortSpike);
            }
            else if (level[i][j] == 'd') {
                jumpPad.setPosition(drawX + 10, drawY);
                window.draw(jumpPad);
            }
            else if (level[i][j] == 'o') {
                orb.setPosition(drawX + 10, drawY);
                window.draw(orb);
            }
            else if (level[i][j] == 'c') {
                coin.setPosition(drawX + 10, drawY);
                window.draw(coin);
            }

            if (i == endPos && j == 0) {
                endwall.setPosition(drawX + 1145, iconGroundDefaultY + icon.getGlobalBounds().height / 2);
                window.draw(endwall);
            }
        }
    }
    window.draw(icon);
}

void Level::restart() {
    resetPlayer();
    load();
    playMusic();
    failed = completed = false;
    coinsCollected = 0;
}

void Level::resetPlayer() {
    icon.setPosition(200, 880);
    iconDefaultY = 880;
    iconHitbox.setPosition(icon.getPosition());
    iconBlockHitbox.setPosition(icon.getPosition());
    jumpCount = jumpHeight = 0;
    levelPos = 0;
    isGrounded = true;
    jumpCheck = false;
    iconAlive = true;
}

void Level::playMusic() {
    music.play();
}

void Level::stopMusic() {
    if (musicLoaded && music.getStatus() == sf::Music::Playing) {
        music.stop();
    }
}

bool Level::isCompleted() const {
    return completed;
}

bool Level::isFailed() const {
    return failed;
}

int Level::getCoinsCollected() const {
    return coinsCollected;
}


void Level::pauseMusic() {
    if (musicLoaded) {
        music.pause();
    }
}


int Level::getJumpCount() const {
    return static_cast<int>(jumpCount);
}