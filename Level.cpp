// Level.cpp
#include "Level.h"
#include <fstream>
#include <iostream>

Level::Level(const char* mapPath, const char* musicPath) : filePath(mapPath) {
    initializeGrid();
    load();
    loadTextures();
    loadSounds();

    if (!music.openFromFile(musicPath)) {
        std::cerr << "Failed to load music: " << musicPath << "\n";
    }
    music.setLoop(true);

    player.setSize(sf::Vector2f(TILE_SIZE - 4, TILE_SIZE - 4));
    player.setFillColor(sf::Color::Red);
}

Level::~Level() {
    music.stop();
}

void Level::loadSounds() {
    if (!jumpBuffer.loadFromFile("assets/sounds/jump.ogg")) {
        std::cerr << "Failed to load jump sound\n";
    }
    jumpSound.setBuffer(jumpBuffer);

    if (!coinBuffer.loadFromFile("assets/sounds/coin.ogg")) {
        std::cerr << "Failed to load coin sound\n";
    }
    coinSound.setBuffer(coinBuffer);

    if (!deathBuffer.loadFromFile("assets/sounds/death.ogg")) {
        std::cerr << "Failed to load death sound\n";
    }
    deathSound.setBuffer(deathBuffer);
}

void Level::initializeGrid() {
    for (int y = 0; y < MAX_GRID_HEIGHT; ++y) {
        for (int x = 0; x < MAX_GRID_WIDTH; ++x) {
            grid[y][x] = 0;
            coinCollected[y][x] = false;
        }
    }
}

void Level::load() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << filePath << "\n";
        return;
    }

    std::string line;
    int y = 0;
    while (std::getline(file, line) && y < MAX_GRID_HEIGHT) {
        for (int x = 0; x < line.size() && x < MAX_GRID_WIDTH; ++x) {
            char ch = line[x];
            if (ch >= '0' && ch <= '9') {
                grid[y][x] = ch - '0';

                if (grid[y][x] == 8) {
                    startPosition = { x, y };
                    player.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                }
                if (grid[y][x] == 9) {
                    finishPosition = { x, y };
                }
            }
        }
        ++y;
    }
}

void Level::loadTextures() {
    std::string names[10] = {
        "", "block.png", "spike.png", "jumppad.png", "orb.png",
        "portal.png", "coin.png", "", "start.png", "finish.png"
    };

    for (int i = 1; i <= 9; ++i) {
        if (names[i] != "") {
            if (!tileTextures[i].loadFromFile("assets/images/" + names[i])) {
                std::cerr << "Failed to load " << names[i] << "\n";
            }
        }
    }

    tileSprite.setScale(
        static_cast<float>(TILE_SIZE) / tileTextures[1].getSize().x,
        static_cast<float>(TILE_SIZE) / tileTextures[1].getSize().y
    );
}

void Level::playMusic() {
    music.play();
}

void Level::stopMusic() {
    music.stop();
}

void Level::restart() {
    player.setPosition(startPosition.x * TILE_SIZE, startPosition.y * TILE_SIZE);
    velocity = { 0.f, 0.f };
    completed = false;
    failed = false;
    gravityInverted = false;
    coinsCollected = 0;
    initializeGrid();
    load();
    playMusic();
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

void Level::update() {
    if (completed || failed) return;

    const float gravity = gravityInverted ? -0.5f : 0.5f;
    const float jumpVelocity = gravityInverted ? 10.f : -10.f;

    velocity.y += gravity;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && onGround) {
        velocity.y = jumpVelocity;
        onGround = false;
    }

    float moveSpeed = 5.f;
    velocity.x = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = -moveSpeed;
    }

    player.move(velocity.x, velocity.y);
    handleCollisions();

    sf::Vector2f pos = player.getPosition();
    int px = static_cast<int>(pos.x) / TILE_SIZE;
    int py = static_cast<int>(pos.y) / TILE_SIZE;

    if (px == finishPosition.x && py == finishPosition.y) {
        completed = true;
        stopMusic();
    }

    if (pos.y > MAX_GRID_HEIGHT * TILE_SIZE || pos.y < 0) {
        failed = true;
        stopMusic();
    }
}

void Level::handleCollisions() {
    sf::Vector2f pos = player.getPosition();
    int px = static_cast<int>(pos.x) / TILE_SIZE;
    int py = static_cast<int>(pos.y + (gravityInverted ? 0 : player.getSize().y)) / TILE_SIZE;

    if (px < 0 || py < 0 || px >= MAX_GRID_WIDTH || py >= MAX_GRID_HEIGHT)
        return;

    int block = grid[py][px];

    switch (block) {
    case 1:
        onGround = true;
        velocity.y = 0;
        player.setPosition(pos.x, gravityInverted ? (py + 1) * TILE_SIZE : py * TILE_SIZE - player.getSize().y);
        break;

    case 2:
        failed = true;
        deathSound.play();
        stopMusic();
        break;

    case 3:
        velocity.y = gravityInverted ? 15.f : -15.f;
        jumpSound.play();
        break;

    case 4:
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            velocity.y = gravityInverted ? 15.f : -15.f;
            jumpSound.play();
        }
        break;

    case 5:
        gravityInverted = !gravityInverted;
        velocity.y = 0;
        break;

    case 6:
        if (!coinCollected[py][px]) {
            coinCollected[py][px] = true;
            coinsCollected++;
            coinSound.play();
            grid[py][px] = 0;
        }
        break;

    default:
        onGround = false;
        break;
    }
}

void Level::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);

    sf::Sprite tileSprite;

    for (int y = 0; y < MAX_GRID_HEIGHT; ++y) {
        for (int x = 0; x < MAX_GRID_WIDTH; ++x) {
            int type = grid[y][x];
            if (type == 0) continue;

            tileSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);

            switch (type) {
            case 1: tileSprite.setTexture(tileTextures[1]); break;
            case 2: tileSprite.setTexture(tileTextures[2]); break;
            case 3: tileSprite.setTexture(tileTextures[3]); break;
            case 4: tileSprite.setTexture(tileTextures[4]); break;
            case 5: tileSprite.setTexture(tileTextures[5]); break;
            case 6: tileSprite.setTexture(tileTextures[6]); break;
            case 8: tileSprite.setTexture(tileTextures[8]); break;
            case 9: tileSprite.setTexture(tileTextures[9]); break;
            }

            window.draw(tileSprite);
        }
    }

    window.draw(player);
}

void Level::setPlayerTexture(sf::Texture* texture) {
    player.setTexture(texture);
    player.setTextureRect(sf::IntRect(0, 0, texture->getSize().x, texture->getSize().y));
    player.setScale(
        (TILE_SIZE - 4) / static_cast<float>(texture->getSize().x),
        (TILE_SIZE - 4) / static_cast<float>(texture->getSize().y)
    );
}