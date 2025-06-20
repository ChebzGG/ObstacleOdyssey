#include "Level.h"
#include "Game.h"
#include "Settings.h"
#include <fstream>
#include <cmath>
using namespace std;
using namespace sf;


Level::Level(const string& levelFile, const string& musicFile)
    : playerY(0), playerVelocityY(0), isOnGround(false), isJumping(false),
    failed(false), completed(false), paused(false), jumpCount(0), levelPos(0), endPos(0)
{
    string bgPath = "assets/images/level1.png";
    for (int i = 2; i <= 5; ++i) {
        string levelStr = "level" + to_string(i);
        if (levelFile.find(levelStr) != string::npos) {
            bgPath = "assets/images/level" + to_string(i) + ".png";
            break;
        }
    }

    Texture* blockTextures[16] = {
        &blockTexture0, &blockTexture1, &blockTexture2, &blockTexture3,
        &blockTexture4, &blockTexture5, &blockTexture6, &blockTexture7,
        &blockTexture8, &blockTexture9, &blockTexture10, &blockTexture11,
        &blockTexture12, &blockTexture13, &blockTexture14, &blockTexture15
    };
    RectangleShape* blocks[16] = {
        &block0, &block1, &block2, &block3, &block4, &block5, &block6, &block7,
        &block8, &block9, &block10, &block11, &block12, &block13, &block14, &block15
    };
    for (int i = 0; i < 16; ++i) {
        string path = "assets/images/block" + to_string(i) + ".png";
        blockTextures[i]->loadFromFile(path);
        blocks[i]->setSize(Vector2f(105, 105));
        blocks[i]->setOrigin(61, 61);
        blocks[i]->setTexture(blockTextures[i]);
    }

    backgroundTexture.loadFromFile(bgPath);
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setPosition(0, 0);

    spikeTexture.loadFromFile("assets/images/default spike.png");
    spike.setTexture(&spikeTexture);


    shortSpikeTexture.loadFromFile("assets/images/short spike.png");
    shortSpike.setTexture(&shortSpikeTexture);

    int selectedIcon = 0;
    ifstream iconFile("icon.txt");
    if (iconFile.is_open()) {
        for (int i = 0; i < 9; ++i) {
            int state;
            iconFile >> state;
            if (state == 2) {
                selectedIcon = i;
                break;
            }
        }
        iconFile.close();
    }
    string iconPath = "assets/images/icon" + to_string(selectedIcon) + ".png";
    iconTexture.loadFromFile(iconPath);
    player.setTexture(&iconTexture);

    spike2Texture.loadFromFile("assets/images/default spike2.png");
    spike2.setTexture(&spike2Texture);

    player.setSize(Vector2f(105, 105));
    player.setOrigin(105 / 2.f, 105 / 2.f);
    player.setPosition(200, 1080 - 105 / 2 - 50);

    deathBuffer.loadFromFile("assets/sounds/death.ogg");
    deathSound.setBuffer(deathBuffer);

    winBuffer.loadFromFile("assets/sounds/win.ogg");
    winSound.setBuffer(winBuffer);

    spike.setSize(Vector2f(105, 105));
    spike.setOrigin(110 / 2.f, 105 / 2.f);


    spike2.setSize(Vector2f(105, 105));
    spike2.setOrigin(105 / 2.f, 105 / 2.f);

    shortSpike.setSize(Vector2f(105, 49));
    shortSpike.setOrigin(105 / 2.f, 49 / 2.f);

    endWallTexture.loadFromFile("assets/images/endwall.png");
    endWall.setTexture(&endWallTexture);

    coinTexture.loadFromFile("assets/images/coin.png");
    coinBuffer.loadFromFile("assets/sounds/coin.ogg");
    coinSound.setBuffer(coinBuffer);

    coinShape.setSize(Vector2f(105, 105));
    coinShape.setOrigin(61, 61);
    coinShape.setTexture(&coinTexture);

    groundSquareTexture.loadFromFile("assets/images/ground square.png");
    float groundY = 1080.0f - groundSquareTexture.getSize().y / 2.0f;
    for (int i = 0; i < GROUND_COUNT; ++i) {
        ground[i].setTexture(groundSquareTexture);
        ground[i].setOrigin(groundSquareTexture.getSize().x / 2.0f, groundSquareTexture.getSize().y / 2.0f);
        ground[i].setPosition(i * (groundSquareTexture.getSize().x * 1.0f), 1160);
    }

    loadLevel(levelFile);

    resetPlayer();

    if (!musicFile.empty()) {
        if (music.openFromFile(musicFile)) {
            float volume = 100.f;
            if (Game::getInstance() && Game::getInstance()->getSettings()) {
                volume = Game::getInstance()->getSettings()->getMusicVolume();
            }
            music.setVolume(volume);
            music.setLoop(true);
            music.play();
        }
    }
}

Level::~Level() {
    music.stop();
}

void Level::loadLevel(const string& filename) {
    ifstream file(filename);
    char input;
    for (int i = 0; i < SIZE_X; ++i)
        for (int j = 0; j < SIZE_Y; ++j)
            for (int j = 0; j < SIZE_Y; ++j) {
                level[i][j] = '-';
                coinCollected[i][j] = false; 
            }

    if (file.is_open()) {
        for (int i = 0; i < SIZE_X; ++i) {
            for (int j = 0; j < SIZE_Y; ++j) {
                file >> input;
                level[i][j] = input;
            }
        }
        file.close();
    }
    setEnd();
}

void Level::setEnd() {
    endPos = 0;
    for (int i = 0; i < SIZE_X; ++i)
        for (int j = 0; j < SIZE_Y; ++j)
            if (level[i][j] == 's' || level[i][j] == 'S' ||
                (level[i][j] >= '0' && level[i][j] <= '9') ||
                level[i][j] == 'q' || level[i][j] == 'w' || level[i][j] == 'e' ||
                level[i][j] == 'r' || level[i][j] == 't' || level[i][j] == 'y' ||
                level[i][j] == 'h')
                endPos = i;
}

void Level::resetPlayer() {
    for (int i = 0; i < SIZE_X; ++i)
        for (int j = 0; j < SIZE_Y; ++j)
            coinCollected[i][j] = false;

    playerRotation = 0.0f;
    player.setPosition(200, 1080 - 105 / 2 - 50);
    playerY = 1080 - 105 / 2 - 50;
    playerVelocityY = 0;
    isOnGround = false;
    isJumping = false;
    failed = false;
    completed = false;
    jumpCount = 0;
    levelPos = 0;
}

void Level::restart() {
    if (Game::getInstance() && Game::getInstance()->getSettings()) {
        Game::getInstance()->getSettings()->incrementAttempts();
        Game::getInstance()->getSettings()->saveSettings();
    }
    resetPlayer();
    music.stop();
    music.play();
}

void Level::setPaused(bool p) {
    paused = p;
}

void Level::playMusic() {
    if (music.getStatus() != Music::Playing)
        music.play();
}

void Level::pauseMusic() {
    if (music.getStatus() == Music::Playing)
        music.pause();
}

void Level::stopMusic() {
    music.stop();
}

void Level::setMusicVolume(float volume) {
    music.setVolume(volume);
}

bool Level::isFailed() const {
    return failed;
}

bool Level::isCompleted() const {
    return completed;
}

int Level::getJumpCount() const {
    return jumpCount;
}

void Level::update() {
    if (failed || completed || paused) return;

    handleInput();
    handlePhysics();
    handleCollisions();

    float endWallX = (endPos + 2) * 105 - levelPos + 200;
    if (endWallX <= player.getPosition().x) {
        if (!completed) {
            if (Game::getInstance() && Game::getInstance()->getSettings()) {
                Game::getInstance()->getSettings()->incrementLevels();
            }
            float sfxVolume = 100.f;
            if (Game::getInstance() && Game::getInstance()->getSettings()) {
                sfxVolume = Game::getInstance()->getSettings()->getSFXVolume();
            }
            winSound.stop();
            winSound.setVolume(sfxVolume);
            winSound.play();
        }
        completed = true;
        music.stop();
    }
}

void Level::handleInput() {
    if ((Keyboard::isKeyPressed(Keyboard::Space) || Keyboard::isKeyPressed(Keyboard::Up)) && isOnGround && !isJumping) {
        playerVelocityY = -38.0f;
        isJumping = true;
        isOnGround = false;
        ++jumpCount;

        if (Game::getInstance() && Game::getInstance()->getSettings()) {
            Game::getInstance()->getSettings()->incrementJumps();
        }
    }
    if (!Keyboard::isKeyPressed(Keyboard::Space) && !Keyboard::isKeyPressed(Keyboard::Up)) {
        isJumping = false;
    }
}

void Level::handlePhysics() {

    playerVelocityY += 3.0f;
    playerY += playerVelocityY;

    player.setPosition(player.getPosition().x, playerY);

    levelPos += 18.68f;

    if (!isOnGround) {
        playerRotation += 7.3f;
        if (playerRotation >= 360.0f) playerRotation -= 360.0f;
    }
    else {
        int n = playerRotation / 90.0f;
        float nearest = 90.0f * n;
        float remainder = playerRotation - nearest;
        if (remainder != 0.0f) {
            if (remainder > 45.0f)
                playerRotation = 90.0f * (n + 1);
            else
                playerRotation = 90.0f * n;
        }
    }

    float groundSpeed = 18.68f;

    for (int i = 0; i < GROUND_COUNT; ++i) {
        ground[i].move(-groundSpeed, 0.0f);

        if (ground[i].getPosition().x < -(float)(groundSquareTexture.getSize().x) / 2.0f) {
            float maxX = ground[0].getPosition().x;
            for (int j = 1; j < GROUND_COUNT; ++j)
                if (ground[j].getPosition().x > maxX)
                    maxX = ground[j].getPosition().x - 26;
            ground[i].setPosition(maxX + (float)(groundSquareTexture.getSize().x), ground[i].getPosition().y);
        }
    }

    player.setRotation(playerRotation);
}

void Level::handleCollisions() {
    isOnGround = false;

    Vector2f pos = player.getPosition();
    float size = 105.0f;
    FloatRect playerRect(
        pos.x - size / 2.0f,
        pos.y - size / 2.0f,
        size,
        size
    );

    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            float objX = (i + 1) * 105.0f - levelPos + 200.0f;
            float objY = 1080.0f - 105.0f / 2.0f - 50.0f - j * 105.0f;

            if (level[i][j] == 's') { 
                float hbWidth = 105 / 4.f;
                float hbHeight = 105 * 0.7f;
                FloatRect spikeHitbox(
                    objX - hbWidth / 2.f,
                    objY - hbHeight / 2.f,
                    hbWidth,
                    hbHeight
                );
                if (checkCollision(playerRect, spikeHitbox)) {
                    failed = true;
                    music.stop();
                    float sfxVolume = 100.f;
                    if (Game::getInstance() && Game::getInstance()->getSettings()) {
                        sfxVolume = Game::getInstance()->getSettings()->getSFXVolume();
                    }
                    deathSound.setVolume(sfxVolume);
                    deathSound.play();
                    return;
                }
            }

            else if (level[i][j] == 'S') {
                float hbWidth = 105 / 4.f;
                float hbHeight = 105 * 0.7f;
                FloatRect spikeHitbox(
                    objX - hbWidth / 2.f,
                    objY - hbHeight / 2.f,
                    hbWidth,
                    hbHeight
                );
                if (checkCollision(playerRect, spikeHitbox)) {
                    failed = true;
                    music.stop();
                    float sfxVolume = 100.f;
                    if (Game::getInstance() && Game::getInstance()->getSettings()) {
                        sfxVolume = Game::getInstance()->getSettings()->getSFXVolume();
                    }
                    deathSound.setVolume(sfxVolume);
                    deathSound.play();
                    return;
                }
            }

            else if (
                (level[i][j] >= '0' && level[i][j] <= '9') ||
                level[i][j] == 'q' || level[i][j] == 'w' || level[i][j] == 'e' ||
                level[i][j] == 'r' || level[i][j] == 't' || level[i][j] == 'y') {

                RectangleShape* currentBlock = nullptr;
                switch (level[i][j]) {
                case '0': currentBlock = &block0; break;
                case '1': currentBlock = &block1; break;
                case '2': currentBlock = &block2; break;
                case '3': currentBlock = &block3; break;
                case '4': currentBlock = &block4; break;
                case '5': currentBlock = &block5; break;
                case '6': currentBlock = &block6; break;
                case '7': currentBlock = &block7; break;
                case '8': currentBlock = &block8; break;
                case '9': currentBlock = &block9; break;
                case 'q': currentBlock = &block10; break;
                case 'w': currentBlock = &block11; break;
                case 'e': currentBlock = &block12; break;
                case 'r': currentBlock = &block13; break;
                case 't': currentBlock = &block14; break;
                case 'y': currentBlock = &block15; break;
                }
                if (currentBlock) {
                    currentBlock->setPosition(objX, objY + 5);
                    FloatRect blockRect = currentBlock->getGlobalBounds();
                    if (playerRect.intersects(blockRect) && playerVelocityY >= 0 && pos.y < blockRect.top) {
                        playerY = blockRect.top - 105.0f / 2.0f;
                        playerVelocityY = 0;
                        isOnGround = true;
                    }
                    else if (playerRect.intersects(blockRect)) {
                        failed = true;
                        music.stop();
                        float sfxVolume = 100.f;
                        if (Game::getInstance() && Game::getInstance()->getSettings()) {
                            sfxVolume = Game::getInstance()->getSettings()->getSFXVolume();
                        }
                        deathSound.setVolume(sfxVolume);
                        deathSound.play();
                        return;
                    }
                }
            }
            else if (level[i][j] == 'h') { 
                float shortY = objY + (105 - 49) / 2;
                float hbWidth = 105 / 4.f;
                float hbHeight = (49 * 0.7f) / 2.f;
                FloatRect shortSpikeHitbox(
                    objX - hbWidth / 2.f,
                    shortY - hbHeight / 2.f,
                    hbWidth,
                    hbHeight
                );
                if (checkCollision(playerRect, shortSpikeHitbox)) {
                    failed = true;
                    music.stop();
                    float sfxVolume = 100.f;
                    if (Game::getInstance() && Game::getInstance()->getSettings()) {
                        sfxVolume = Game::getInstance()->getSettings()->getSFXVolume();
                    }
                    deathSound.setVolume(sfxVolume);
                    deathSound.play();
                    return;
                }
            }
            else if (level[i][j] == 'c' && !coinCollected[i][j]) { 
                coinShape.setPosition(objX, objY);
                FloatRect coinRect = coinShape.getGlobalBounds();
                if (playerRect.intersects(coinRect)) {
                    coinCollected[i][j] = true;
                    float sfxVolume = 100.f;
                    if (Game::getInstance() && Game::getInstance()->getSettings()) {
                        sfxVolume = Game::getInstance()->getSettings()->getSFXVolume();
                    }
                    coinSound.setVolume(sfxVolume);
                    coinSound.play();
                    if (Game::getInstance() && Game::getInstance()->getSettings()) {
                        Game::getInstance()->getSettings()->addCoins(1);
                        Game::getInstance()->getSettings()->addCustomCoins(1);
                    }
                }
            }

            float endWallX = (endPos + 2) * 105 - levelPos + 200 + 105 / 2 + 300;
            float endWallY = 1080 / 2;
            endWall.setPosition(endWallX, endWallY);
            FloatRect endWallRect = endWall.getGlobalBounds();
            if (playerRect.intersects(endWallRect)) {
                completed = true;
                music.stop();
                if (Game::getInstance() && Game::getInstance()->getSettings()) {
                    Game::getInstance()->getSettings()->incrementLevels();
                }
                return;
            }
        }
    }

    float groundY = 1080.0f - 105.0f / 2.0f - 50.0f;
    if (playerY >= groundY) {
        playerY = groundY;
        playerVelocityY = 0;
        isOnGround = true;
    }
}

bool Level::checkCollision(const FloatRect& a, const FloatRect& b) const {
    return a.intersects(b);
}

void Level::render(RenderWindow& window) {
    window.draw(backgroundSprite);

    for (int i = 0; i < GROUND_COUNT; ++i)
        window.draw(ground[i]);

    window.draw(player);
    drawObstacles(window);
}

void Level::drawObstacles(RenderWindow& window) {
    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            float objX = (i + 1) * 105 - levelPos + 200;
            float objY = 1080 - 105 / 2 - 50 - j * 105;

            if (level[i][j] == 's') {
                spike.setPosition(objX + 1, objY);
                window.draw(spike);
            }
            else if (level[i][j] == 'S') {
                spike2.setPosition(objX, objY - 1);
                window.draw(spike2);
            }
            else if (
                (level[i][j] >= '0' && level[i][j] <= '9') ||
                level[i][j] == 'q' || level[i][j] == 'w' || level[i][j] == 'e' ||
                level[i][j] == 'r' || level[i][j] == 't' || level[i][j] == 'y') {

                RectangleShape* currentBlock = nullptr;
                switch (level[i][j]) {
                case '0': currentBlock = &block0; break;
                case '1': currentBlock = &block1; break;
                case '2': currentBlock = &block2; break;
                case '3': currentBlock = &block3; break;
                case '4': currentBlock = &block4; break;
                case '5': currentBlock = &block5; break;
                case '6': currentBlock = &block6; break;
                case '7': currentBlock = &block7; break;
                case '8': currentBlock = &block8; break;
                case '9': currentBlock = &block9; break;
                case 'q': currentBlock = &block10; break;
                case 'w': currentBlock = &block11; break;
                case 'e': currentBlock = &block12; break;
                case 'r': currentBlock = &block13; break;
                case 't': currentBlock = &block14; break;
                case 'y': currentBlock = &block15; break;
                }
                if (currentBlock) {
                    currentBlock->setPosition(objX + 8, objY + 8);
                    window.draw(*currentBlock);
                }
            }
            else if (level[i][j] == 'h') {
                shortSpike.setPosition(objX, objY + (105 - 49) / 2);
                window.draw(shortSpike);
            }
            else if (level[i][j] == 'c' && !coinCollected[i][j]) {
                coinShape.setPosition(objX, objY);
                window.draw(coinShape);
            }
            if (i == endPos && j == 0) {
                endWall.setPosition(objX + 105 / 2 + 300 - 430, 1080 / 2);
                endWall.setSize(Vector2f(1920, 1080));
                endWall.setOrigin(0, 1080 / 2.f);
                window.draw(endWall);
            }
        }
    }

}

void Level::drawGroundSquare(RenderWindow& window, float x, float y, float size) {
    RectangleShape ground(Vector2f(size, size));
    ground.setOrigin(size / 2.f, size / 2.f);
    ground.setPosition(x, y);
    window.draw(ground);
}