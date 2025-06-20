// Game.cpp
#include "Game.h"
#include "Settings.h"
#include <iostream>

Game* Game::instance = nullptr;

Game::Game()
    : window(sf::VideoMode(1920, 1080), "Obstacle Odyssey"),
    menu(window),
    currentLevel(-1),
    isPaused(false),
    inMenu(true),
    settings(nullptr) {

    instance = this;


    std::ifstream file("settings.cfg");
    float volume = 100.f; // Значение по умолчанию, если файл не найден
    if (file.is_open()) {
        file >> volume;
        file.close();
    }

    // Инициализация музыки меню
    if (!menuMusic.openFromFile("assets/music/menu.ogg")) {
        std::cerr << "Failed to load menu music" << std::endl;
    }
    menuMusic.setLoop(true);
    menuMusic.setVolume(volume); // Устанавливаем громкость из файла
    menuMusic.play();



    // Инициализация уровней
    for (int i = 0; i < 5; ++i) {
        levels[i] = nullptr;
    }


    // Загрузка текстур для паузы
    if (!pauseBackgroundTexture.loadFromFile("assets/images/pause.png")) {
        std::cerr << "Failed to load pause background" << std::endl;
    }
    pauseBackground.setTexture(pauseBackgroundTexture);

    if (!resumeButtonTexture.loadFromFile("assets/images/resume.png")) {
        std::cerr << "Failed to load resume button" << std::endl;
    }
    resumeButton.setTexture(resumeButtonTexture);
    resumeButton.setPosition(1020.f, 340.f);

    if (!exitButtonTexture.loadFromFile("assets/images/exit2.png")) {
        std::cerr << "Failed to load exit button" << std::endl;
    }
    exitButton.setTexture(exitButtonTexture);
    exitButton.setPosition(500.f, 340.f);

    window.setFramerateLimit(60);

    // Загрузка уровней
    levels[0] = new Level("levels/level1.txt", "assets/music/level1.ogg");
    levels[1] = new Level("levels/level2.txt", "assets/music/level2.ogg");
    levels[2] = new Level("levels/level3.txt", "assets/music/level3.ogg");
    levels[3] = new Level("levels/level4.txt", "assets/music/level4.ogg");
    levels[4] = new Level("levels/level5.txt", "assets/music/level5.ogg");
}

Game::~Game() {
    for (int i = 0; i < 5; ++i) {
        delete levels[i]; // Удаление nullptr безопасно
    }
    delete settings;
}

void Game::run() {
    while (window.isOpen()) {
        if (inMenu) {
            handleMenuState();
        }
        else {
            if (currentLevel >= 0 && currentLevel < 5) {
                handleGameState();
            }
        }
    }
}

void Game::handleMenuState() {
    while (inMenu && window.isOpen()) {
        menu.run();
        int menuSelection = menu.getMenuSelection();

        switch (menuSelection) {
        case 1: handleLevelSelection(); break;
        case 3: handleSettings(); break;
            //case 4: showStatistics(); break;
        case -1: window.close(); break;
        }
    }
}

void Game::handleLevelSelection() {
    LevelSelect levelSelect(window);
    levelSelect.run();
    int selected = levelSelect.getSelectedLevel();

    if (selected == -1) return;

    currentLevel = selected - 1;
    inMenu = false;
    startLevel();
}

void Game::handleSettings() {
    if (!settings) {
        settings = new Settings(window);
    }

    settings->run(); // После выхода из run() продолжится выполнение

    // Применяем новые настройки
    menuMusic.setVolume(settings->getMusicVolume());

    if (currentLevel >= 0 && currentLevel < 5 && levels[currentLevel]) {
        levels[currentLevel]->setMusicVolume(settings->getMusicVolume());
    }
}

void Game::startLevel() {
    if (currentLevel < 0 || currentLevel >= 5) return;

    std::string levelPath = "levels/level" + std::to_string(currentLevel + 1) + ".txt";
    std::string musicPath = "assets/music/level" + std::to_string(currentLevel + 1) + ".ogg";

    delete levels[currentLevel];  // Удаляем предыдущий уровень, если он был загружен
    levels[currentLevel] = new Level(levelPath, musicPath);

    menuMusic.stop();
    levels[currentLevel]->playMusic();
    if (settings) {
        levels[currentLevel]->setMusicVolume(settings->getMusicVolume());
    }
    isPaused = false;
    levelClock.restart();

    while (window.isOpen() && !inMenu) {
        processEvents();

        if (currentLevel >= 0 && currentLevel < 5 && levels[currentLevel] && levels[currentLevel]->isFailed()) {
            showGameOverMenu();
            continue;
        }

        if (currentLevel >= 0 && currentLevel < 5 && levels[currentLevel] && levels[currentLevel]->isCompleted()) {
            returnToMainMenu();
            break;
        }

        update();
        render();
    }

}


void Game::handleGameState() {
    processEvents();
    if (!isPaused) update();
    render();
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            isPaused = !isPaused;
            levels[currentLevel]->setPaused(isPaused);
            if (isPaused) {
                levels[currentLevel]->pauseMusic();
            }
            else {
                levels[currentLevel]->playMusic();
            }
        }

        if (isPaused && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

            if (resumeButton.getGlobalBounds().contains(mousePos)) {
                isPaused = false;
                levels[currentLevel]->setPaused(false);
                levels[currentLevel]->playMusic();
            }
            else if (exitButton.getGlobalBounds().contains(mousePos)) {
                // Правильно выходим в меню, без удаления levels[currentLevel]
                levels[currentLevel]->stopMusic();
                levels[currentLevel]->setPaused(false);
                currentLevel = -1;
                isPaused = false;
                inMenu = true;

                // Восстановим музыку меню
                std::ifstream file("settings.cfg");
                float volume = 100.f;
                if (file.is_open()) {
                    file >> volume;
                    file.close();
                }
                menuMusic.setVolume(volume);
                menuMusic.play();
            }
        }
    }
}



void Game::update() {
    if (isPaused) {
        handleMouseHover();
    }
    if (!inMenu && currentLevel >= 0 && currentLevel < 5 && levels[currentLevel]) {
        levels[currentLevel]->update();
    }
}

void Game::render() {
    window.clear();

    if (!inMenu && currentLevel >= 0 && currentLevel < 5 && levels[currentLevel]) {
        levels[currentLevel]->render(window);
    }

    if (isPaused) {
        window.draw(pauseBackground);
        window.draw(resumeButton);
        window.draw(exitButton);
    }

    window.display();
}

void Game::showPauseMenu() {
    while (isPaused && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                isPaused = false;
                levels[currentLevel]->playMusic();
            }
        }
        render();
    }
}




void Game::showGameOverMenu() {
    if (currentLevel < 0 || currentLevel >= 5 || !levels[currentLevel]) return;

    sf::Texture resetTexture, retryTexture, exitTexture;
    sf::Sprite resetBackground, retryButton, exitButton;

    if (!resetTexture.loadFromFile("assets/images/reset.png")) {
        std::cerr << "Failed to load reset texture" << std::endl;
    }
    if (!retryTexture.loadFromFile("assets/images/retry.png")) {
        std::cerr << "Failed to load retry texture" << std::endl;
    }
    if (!exitTexture.loadFromFile("assets/images/exit2.png")) {
        std::cerr << "Failed to load exit texture" << std::endl;
    }

    resetBackground.setTexture(resetTexture);
    retryButton.setTexture(retryTexture);
    exitButton.setTexture(exitTexture);

    retryButton.setPosition(1020.f, 340.f);
    exitButton.setPosition(500.f, 340.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }



        retryButton.setColor(isMouseOver(retryButton) ? sf::Color(255, 200, 255, 255) : sf::Color::White);
        exitButton.setColor(isMouseOver(exitButton) ? sf::Color(255, 200, 255, 255) : sf::Color::White);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
            if (retryButton.getGlobalBounds().contains(mousePos)) {
                levels[currentLevel]->restart();
                levelClock.restart();
                return;
            }
            else if (exitButton.getGlobalBounds().contains(mousePos)) {
                returnToMainMenu();
                return;
            }
        }

        window.clear();
        window.draw(resetBackground);
        window.draw(retryButton);
        window.draw(exitButton);

        // Отображение статистики
        sf::Font font;
        if (font.loadFromFile("assets/fonts/arial.ttf")) {
            sf::Text jumpText, timeText;
            jumpText.setFont(font);
            timeText.setFont(font);
            jumpText.setString("Jump Count: " + std::to_string(levels[currentLevel]->getJumpCount()));
            timeText.setString("Time Spent: " + std::to_string(static_cast<int>(levelClock.getElapsedTime().asSeconds())) + " sec");
            jumpText.setCharacterSize(40);
            timeText.setCharacterSize(40);
            jumpText.setPosition(800.f, 600.f);
            timeText.setPosition(800.f, 650.f);

            window.draw(jumpText);
            window.draw(timeText);
        }

        window.display();
    }
}

void Game::returnToMainMenu() {
    if (currentLevel >= 0 && currentLevel < 5 && levels[currentLevel]) {
        levels[currentLevel]->stopMusic();
    }
    inMenu = true;
    currentLevel = -1;
    isPaused = false;

    // Восстанавливаем громкость меню
    std::ifstream file("settings.cfg");
    float volume = 100.f;
    if (file.is_open()) {
        file >> volume;
        file.close();
    }
    menuMusic.setVolume(volume);
    menuMusic.play();
}




void Game::returnToSettings() {
    inMenu = true;
    currentLevel = -1;
    menuMusic.play();
}

void Game::handleMouseHover() {
    resumeButton.setColor(isMouseOver(resumeButton) ? sf::Color(255, 200, 255, 255) : sf::Color::White);
    exitButton.setColor(isMouseOver(exitButton) ? sf::Color(255, 200, 255, 255) : sf::Color::White);
}

bool Game::isMouseOver(const sf::Sprite& sprite) const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
}

void Game::setMusicVolume(float volume) {
    // Устанавливаем громкость для меню
    menuMusic.setVolume(volume);

    // Устанавливаем громкость для текущего уровня (если он активен)
    if (currentLevel >= 0 && currentLevel < 5 && levels[currentLevel]) {
        levels[currentLevel]->setMusicVolume(volume);
    }

    // Сохраняем громкость в настройках (если они есть)
    if (settings) {
        settings->setMusicVolume(volume);
    }
}

Game* Game::getInstance() {
    if (!instance) {
        instance = new Game();
    }
    return instance;
}






// Game.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Menu.h"
#include "Level.h"
#include "LevelSelect.h"
#include "Settings.h"

class Game {
public:
    Game();
    ~Game();
    void run();
    void returnToMainMenu();
    void setMusicVolume(float volume);
    static Game* getInstance();

private:
    void processEvents();
    void update();
    void render();
    void showPauseMenu();
    void handleMenuState();
    void handleGameState();
    void handleLevelSelection();
    void startLevel();
    void showGameOverMenu();
    void handleSettings();
    void returnToSettings(); // Новый метод для возврата в настройки

    Settings* settings;
    sf::RenderWindow window;
    sf::Music menuMusic;

    Menu menu;
    Level* levels[5];
    int currentLevel = -1;
    bool isPaused = false;
    bool inMenu = true;

    sf::Texture pauseBackgroundTexture;
    sf::Sprite pauseBackground;
    sf::Texture resumeButtonTexture;
    sf::Sprite resumeButton;
    sf::Texture exitButtonTexture;
    sf::Sprite exitButton;

    sf::Clock levelClock;

    void handleMouseHover();
    bool isMouseOver(const sf::Sprite& sprite) const;
    static Game* instance; // Объявление
};























// Level.cpp
#include "Level.h"
#include "Game.h"
#include <fstream>
#include <iostream>
#include <cmath>

Level::Level(const std::string& levelFile, const std::string& musicFile)
    : levelPath(levelFile), musicPath(musicFile) {

    // Определяем номер уровня по имени файла
    int levelNumber = 1;
    if (levelFile.find("level2") != std::string::npos) levelNumber = 2;
    if (levelFile.find("level3") != std::string::npos) levelNumber = 3;
    if (levelFile.find("level4") != std::string::npos) levelNumber = 4;
    if (levelFile.find("level5") != std::string::npos) levelNumber = 5;

    // Загрузка фона
    std::string bgPath = "assets/images/level" + std::to_string(levelNumber) + ".png";
    backgroundTexture.loadFromFile(bgPath);
    backgroundSprite.setTexture(backgroundTexture);

    // Загрузка объектов
    spikeTex.loadFromFile("assets/images/default spike.png");
    blockTex.loadFromFile("assets/images/default block.png");
    shortSpikeTex.loadFromFile("assets/images/short spike.png");
    endwallTex.loadFromFile("assets/images/endwall.png");

    spike.setTexture(spikeTex);
    block.setTexture(blockTex);
    shortSpike.setTexture(shortSpikeTex);
    endwall.setTexture(endwallTex);
    endwall.setOrigin(endwall.getLocalBounds().width, endwall.getLocalBounds().height);

    block.setOrigin(40.f, 61.f);
    block.setScale(0.48f, 0.48f);
    spike.setScale(0.5f, 0.5f);
    spike.setOrigin(spike.getLocalBounds().width / 2, spike.getLocalBounds().height / 2);
    shortSpike.setScale(0.5f, 0.5f);
    shortSpike.setOrigin(shortSpike.getLocalBounds().width / 2, shortSpike.getLocalBounds().height / 2);

    // Земля
    groundTex.loadFromFile("assets/images/ground square.png");
    for (int i = 0; i < GROUND_COUNT; ++i) {
        ground[i].setTexture(groundTex);
        ground[i].setScale(1.2f, 1.2f);
    }

    // Иконка игрока
    iconTex.loadFromFile("assets/images/icon.png");
    icon.setTexture(iconTex);
    icon.setOrigin(icon.getLocalBounds().width / 2.f, icon.getLocalBounds().height / 2.f);
    icon.setPosition(200.f, 880.f);

    iconHitbox.setSize(sf::Vector2f(icon.getGlobalBounds().width, icon.getGlobalBounds().height));
    iconHitbox.setFillColor(sf::Color::Transparent);
    iconHitbox.setOrigin(iconHitbox.getSize() / 2.f);

    iconBlockHitbox.setSize(iconHitbox.getSize() / 5.f);
    iconBlockHitbox.setFillColor(sf::Color::Transparent);
    iconBlockHitbox.setOrigin(iconBlockHitbox.getSize() / 2.f);

    iconDefaultY = icon.getPosition().y;
    iconGroundDefaultY = iconDefaultY;

    // Музыка и звуки
    if (music.openFromFile(musicPath)) musicLoaded = true;
    deathBuffer.loadFromFile("assets/sounds/death.ogg");
    death.setBuffer(deathBuffer);

    load();
}

Level::~Level() {
    if (musicLoaded) music.stop();
}

void Level::load() {
    setLevel();
    setEndPosition();
    resetPlayer();
}

void Level::setLevel() {
    std::ifstream file(levelPath);
    if (!file.is_open()) {
        std::cerr << "Could not open level file: " << levelPath << std::endl;
        return;
    }

    int loadedCount = 0;
    char input;

    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            if (!(file >> input)) {
                // Если недостаточно символов, заполняем оставшееся '-'
                level[i][j] = '-';
                continue;
            }

            // Разрешены только s, b, h, -, остальные игнорируем
            if (input == 's' || input == 'b' || input == 'h' || input == '-') {
                level[i][j] = input;
                if (input != '-') loadedCount++;
            }
            else {
                level[i][j] = '-';
            }
        }
    }

    file.close();

    std::cout << "Level loaded: " << loadedCount << " objects from file: " << levelPath << std::endl;
}


void Level::setEndPosition() {
    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
            if (level[i][j] == 's' || level[i][j] == 'b' || level[i][j] == 'h') {
                endPos = i;
            }
        }
    }
}

void Level::resetPlayer() {
    // Сначала земля
    for (int i = 0; i < GROUND_COUNT; ++i) {
        ground[i].setPosition(i * ground[i].getGlobalBounds().width,
            920.f); // абсолютное Y-положение земли (можно подкорректировать)
    }

    // Теперь иконка
    float groundY = ground[0].getPosition().y;

    icon.setPosition(200.f, groundY);
    iconDefaultY = iconGroundDefaultY = groundY;

    // Обновить хитбоксы
    iconHitbox.setPosition(icon.getPosition());
    iconBlockHitbox.setPosition(icon.getPosition());

    jumpCount = jumpHeight = 0;
    levelPos = 0;
    isGrounded = true;
    jumpCheck = false;
    iconAlive = true;
    completed = failed = paused = false;
}




void Level::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        if (isGrounded) jumpCheck = true;
    }
}

void Level::handlePhysics() {
    if (jumpCheck && iconAlive) {
        if (jumpCount++ == 0) isGrounded = false;
        jumpHeight = 1.25f * (0 - jumpCount) * (jumpCount - 25);
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
    }
    else {
        icon.rotate(180.f / 26);
    }

    levelPos += levelSpeed;

    for (int i = 0; i < GROUND_COUNT; ++i) {
        ground[i].move(-levelSpeed, 0.f);

        if (ground[i].getPosition().x + ground[i].getGlobalBounds().width < 0) {
            int prev = (i - 1 + GROUND_COUNT) % GROUND_COUNT;
            float rightEdge = ground[prev].getPosition().x + ground[prev].getGlobalBounds().width - 20.0f;
            ground[i].setPosition(rightEdge, icon.getGlobalBounds().top + icon.getGlobalBounds().height);
        }
    }
}

void Level::checkCollisions() {
    isGrounded = false;

    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
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
                    iconDefaultY = block.getPosition().y - iconHitbox.getSize().y / 2.f;
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

            if (i == endPos && j == 0) {
                endwall.setPosition(drawX + 1145, iconGroundDefaultY + icon.getGlobalBounds().height / 2);
                if (icon.getPosition().x >= endwall.getPosition().x - 50) {
                    completed = true;
                    stopMusic();
                    return;
                }
            }
        }
    }

    for (int i = 0; i < GROUND_COUNT; ++i) {
        if (ground[i].getGlobalBounds().intersects(iconHitbox.getGlobalBounds())) {
            iconDefaultY = ground[i].getPosition().y - iconHitbox.getSize().y / 2.f;
            icon.setPosition(icon.getPosition().x, iconDefaultY);
            iconHitbox.setPosition(icon.getPosition());
            iconBlockHitbox.setPosition(icon.getPosition());
            isGrounded = true;
            break;
        }
    }

    if (icon.getPosition().y > 1080 || icon.getPosition().y < 0) {
        failed = true;
        stopMusic();
    }
}

void Level::update() {
    if (paused || completed || failed) return;

    handleInput();
    handlePhysics();
    checkCollisions();
}


void Level::render(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
    for (int i = 0; i < GROUND_COUNT; ++i) {
        window.draw(ground[i]);
    }

    for (int i = 0; i < SIZE_X; ++i) {
        for (int j = 0; j < SIZE_Y; ++j) {
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
}

void Level::playMusic() {
    if (musicLoaded) music.play();
}

void Level::stopMusic() {
    if (musicLoaded) music.stop();
}

void Level::pauseMusic() {
    if (musicLoaded) music.pause();
}

bool Level::isCompleted() const {
    return completed;
}

bool Level::isFailed() const {
    return failed;
}

int Level::getJumpCount() const {
    return static_cast<int>(jumpCount);
}







//Level.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

const int SIZE_X = 1000;
const int SIZE_Y = 10;

class Level {
public:
    Level(const std::string& levelFile, const std::string& musicFile);
    ~Level();

    void load();
    void update();
    void render(sf::RenderWindow& window);
    void restart();

    void playMusic();
    void stopMusic();
    void pauseMusic();

    bool isCompleted() const;
    bool isFailed() const;

    float getMusicVolume() const { return music.getVolume(); }
    void setMusicVolume(float volume) { if (musicLoaded) music.setVolume(volume); }

    int getJumpCount() const;

    // Пауза
    void setPaused(bool p) { paused = p; }
    bool isPaused() const { return paused; }

private:
    void setLevel();
    void setEndPosition();
    void resetPlayer();
    void handleInput();
    void handlePhysics();
    void checkCollisions();

    char level[SIZE_X][SIZE_Y]{};
    int endPos = 0;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Texture spikeTex, blockTex, shortSpikeTex, endwallTex;
    sf::Sprite spike, block, shortSpike, endwall;

    static const int GROUND_COUNT = 16;
    sf::Texture groundTex;
    sf::Sprite ground[GROUND_COUNT];

    sf::Texture iconTex;
    sf::Sprite icon;

    sf::RectangleShape iconHitbox, iconBlockHitbox;

    sf::Music music;
    sf::SoundBuffer deathBuffer;
    sf::Sound death;

    float levelSpeed = 14.4f;
    float levelPos = 0;
    float jumpCount = 0;
    float jumpHeight = 0;

    float iconDefaultY;
    float iconGroundDefaultY;
    bool iconAlive = true;
    bool isGrounded = true;
    bool jumpCheck = false;
    bool completed = false;
    bool failed = false;
    bool paused = false;

    std::string levelPath;
    std::string musicPath;
    bool musicLoaded = false;

    sf::Sprite jumpPad;
    sf::Sprite orb;
    sf::Sprite coin;

    // Счётчики и звук
    int coinsCollected = 0;
    sf::Sound jumpSound;
    sf::Sound coinSound;


};




























// LevelSelect.cpp
#include "LevelSelect.h"
#include <iostream>

LevelSelect::LevelSelect(sf::RenderWindow& win) : window(win) {
    backgroundTexture.loadFromFile("assets/images/menu.png");
    background.setTexture(backgroundTexture);

    lvlTexture.loadFromFile("assets/images/levels.png");
    lvl.setTexture(lvlTexture);
    lvl.setPosition(680.f, 20.f);
    lvl.setScale(1.0f, 1.0f);

    backTexture.loadFromFile("assets/images/back.png");
    backButton.setTexture(backTexture);
    backButton.setPosition(20.f, 20.f);
    backButton.setScale(0.18f, 0.18f);

    for (int i = 0; i < 5; ++i) {
        levelTextures[i].loadFromFile("assets/images/" + std::to_string(i + 1) + ".png");
        levelButtons[i].setTexture(levelTextures[i]);

        if (i % 2 == 0) {
            levelButtons[i].setPosition(190.f + (i * 320.f), 300.f);
        }
        else {
            levelButtons[i].setPosition(190.f + (i * 320.f), 400.f);
        }
        levelButtons[i].setScale(0.7f, 0.7f);

        hoverBuffer.loadFromFile("assets/sounds/hover.ogg");
        hoverSound.setBuffer(hoverBuffer);

    }
}

void LevelSelect::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
        if (selectedLevel != 0 || selectedLevel == -1) { // -1 для кнопки "Назад"
            break;
        }
    }
}

void LevelSelect::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (isMouseOver(backButton)) {
                selectedLevel = -1; // Устанавливаем -1 для кнопки "Назад"
                return;
            }
            for (int i = 0; i < 5; ++i) {
                if (isMouseOver(levelButtons[i])) {
                    selectedLevel = i + 1;
                }
            }
        }
    }
}

void LevelSelect::update() {
    bool isHoveringNow = false;

    for (int i = 0; i < 5; ++i) {
        if (isMouseOver(levelButtons[i])) {
            levelButtons[i].setColor(sf::Color(255, 200, 255, 255));
            isHoveringNow = true;
        }
        else {
            levelButtons[i].setColor(sf::Color::White);
        }
    }

    if (isMouseOver(backButton)) {
        backButton.setColor(sf::Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        backButton.setColor(sf::Color::White);
    }

    // Воспроизводим звук только при новом наведении
    if (isHoveringNow && !wasHovering) {
        hoverSound.play();
    }

    wasHovering = isHoveringNow; // Обновляем флаг
}

void LevelSelect::render() {
    window.clear();
    window.draw(background);
    window.draw(backButton);
    window.draw(lvl);
    for (int i = 0; i < 5; ++i) {
        window.draw(levelButtons[i]);
    }
    window.display();
}

bool LevelSelect::isMouseOver(const sf::Sprite& sprite) {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
}

int LevelSelect::getSelectedLevel() const {
    return selectedLevel;
}






// LevelSelect.h
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // Добавляем это

class LevelSelect {
public:
    LevelSelect(sf::RenderWindow& window);
    void run();
    int getSelectedLevel() const;

private:
    void processEvents();
    void render();
    void update();
    bool isMouseOver(const sf::Sprite& sprite);

    sf::RenderWindow& window;
    sf::Texture backgroundTexture;
    sf::Sprite background;

    sf::Texture backTexture;
    sf::Sprite backButton;

    sf::Texture levelTextures[5];
    sf::Sprite levelButtons[5];

    sf::Texture lvlTexture;
    sf::Sprite lvl;

    // Звуковые объекты
    sf::SoundBuffer hoverBuffer;
    sf::Sound hoverSound;
    bool wasHovering = false;

    int selectedLevel = 0;
};


















// main.cpp
#include <SFML/Graphics.hpp>
#include "Game.h"

int main() {
    Game game;
    game.run();
    return 0;
}





















// Menu.cpp
#include "Menu.h"
#include "LevelSelect.h"
#include <iostream>

Menu::Menu(sf::RenderWindow& win) : window(win) {
    backgroundTexture.loadFromFile("assets/images/menu.png");
    background.setTexture(backgroundTexture);

    logoTexture.loadFromFile("assets/images/logo.png");
    logo.setTexture(logoTexture);
    logo.setPosition(150.f, 50.f);

    playTexture.loadFromFile("assets/images/play.png");
    playButton.setTexture(playTexture);
    playButton.setPosition(760.f, 350.f);
    playButton.setScale(1.0f, 1.0f);

    customTexture.loadFromFile("assets/images/custom.png");
    customButton.setTexture(customTexture);
    customButton.setPosition(360.f, 415.f);
    customButton.setScale(0.7f, 0.7f);

    settingsTexture.loadFromFile("assets/images/settings.png");
    settingsButton.setTexture(settingsTexture);
    settingsButton.setPosition(1280.f, 415.f);
    settingsButton.setScale(0.7f, 0.7f);

    exitTexture.loadFromFile("assets/images/exit.png");
    exitButton.setTexture(exitTexture);
    exitButton.setPosition(20.f, 20.f);
    exitButton.setScale(0.18f, 0.18f);

    statsTexture.loadFromFile("assets/images/stats.png");
    statsButton.setTexture(statsTexture);
    statsButton.setPosition(1730.f, 870.f);
    statsButton.setScale(0.4f, 0.4f);

    hoverBuffer.loadFromFile("assets/sounds/hover.ogg");
    hoverSound.setBuffer(hoverBuffer);

}

void Menu::run() {
    selectedOption = -1;
    while (window.isOpen()) {
        processEvents();
        update();
        render();
        if (selectedOption != -1) {
            break;
        }
    }
}


void Menu::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (isMouseOver(playButton)) selectedOption = 1;
            else if (isMouseOver(customButton)) selectedOption = 2;
            else if (isMouseOver(settingsButton)) selectedOption = 3;  // Настройки
            else if (isMouseOver(exitButton)) window.close();
            else if (isMouseOver(statsButton)) selectedOption = 4;
        }
    }
}

void Menu::update() {
    handleMouseHover();
}

void Menu::render() {
    window.clear();
    window.draw(background);
    window.draw(logo);
    window.draw(playButton);
    window.draw(customButton);
    window.draw(settingsButton);
    window.draw(exitButton);
    window.draw(statsButton);
    window.display();
}

void Menu::handleMouseHover() {
    bool isHoveringNow = false;

    if (isMouseOver(playButton)) {
        playButton.setColor(sf::Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        playButton.setColor(sf::Color::White);
    }

    if (isMouseOver(customButton)) {
        customButton.setColor(sf::Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        customButton.setColor(sf::Color::White);
    }

    if (isMouseOver(settingsButton)) {
        settingsButton.setColor(sf::Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        settingsButton.setColor(sf::Color::White);
    }

    if (isMouseOver(exitButton)) {
        exitButton.setColor(sf::Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        exitButton.setColor(sf::Color::White);
    }

    if (isMouseOver(statsButton)) {
        statsButton.setColor(sf::Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        statsButton.setColor(sf::Color::White);
    }

    // Воспроизводим звук только при новом наведении
    if (isHoveringNow && !wasHovering) {
        hoverSound.play();
    }

    wasHovering = isHoveringNow; // Обновляем флаг
}

bool Menu::isMouseOver(const sf::Sprite& sprite) {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
}

int Menu::getMenuSelection() const {
    return selectedOption;
}

int Menu::getChosenLevel() const {
    return chosenLevel;
}

void Menu::handleMenuSelection() {

}



// Menu.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Menu {
public:
    Menu(sf::RenderWindow& window);
    void run();
    int getMenuSelection() const;
    int getChosenLevel() const;

private:
    void processEvents();
    void update();
    void render();
    void handleMouseHover();
    bool isMouseOver(const sf::Sprite& sprite);
    void handleMenuSelection();

    sf::RenderWindow& window;
    sf::Texture backgroundTexture;
    sf::Sprite background;

    sf::Texture logoTexture;
    sf::Sprite logo;

    sf::Texture playTexture;
    sf::Sprite playButton;

    sf::Texture customTexture;
    sf::Sprite customButton;

    sf::Texture settingsTexture;
    sf::Sprite settingsButton;

    sf::Texture exitTexture;
    sf::Sprite exitButton;

    sf::Texture statsTexture;
    sf::Sprite statsButton;

    int selectedOption = -1;
    int chosenLevel = -1;

    sf::SoundBuffer hoverBuffer;
    sf::Sound hoverSound;
    bool wasHovering = false;
};



























//Settings.cpp
#include <iostream>
#include <fstream>
#include "Settings.h"
#include "Game.h"

Settings::Settings(sf::RenderWindow& win)
    : window(win) {
    // Загрузка фона
    if (!backgroundTexture.loadFromFile("assets/images/menu.png")) {
        std::cerr << "Failed to load settings background" << std::endl;
    }
    background.setTexture(backgroundTexture);

    // Кнопка назад
    if (!backTexture.loadFromFile("assets/images/back.png")) {
        std::cerr << "Failed to load back button texture" << std::endl;
    }
    backButton.setTexture(backTexture);
    backButton.setPosition(20.f, 20.f);
    backButton.setScale(0.18f, 0.18f);

    // Кнопка сброса
    if (!resetTexture.loadFromFile("assets/images/resetbtn.png")) {
        std::cerr << "Failed to load reset button texture" << std::endl;
    }
    resetButton.setTexture(resetTexture);
    resetButton.setPosition(760.f, 700.f);
    resetButton.setScale(0.5f, 0.5f);

    // Загрузка шрифта
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font for settings" << std::endl;
    }

    // Заголовки
    musicLabel.setFont(font);
    musicLabel.setString("Music:");
    musicLabel.setCharacterSize(40);
    musicLabel.setPosition(50.f, 250.f);  // Сдвинуто влево
    musicLabel.setFillColor(sf::Color::White);

    sfxLabel.setFont(font);
    sfxLabel.setString("SFX:");
    sfxLabel.setCharacterSize(40);
    sfxLabel.setPosition(50.f, 550.f);   // Сдвинуто влево
    sfxLabel.setFillColor(sf::Color::White);

    // Загрузка настроек из файла при старте
    loadSettings();

    // Загрузка кнопок громкости
    for (int i = 0; i < 6; ++i) {
        if (!volumeTextures[i].loadFromFile("assets/images/set" + std::to_string(i) + ".png")) {
            std::cerr << "Failed to load volume button " << i << std::endl;
        }

        // Кнопки для музыки
        musicVolumeButtons[i].setTexture(volumeTextures[i]);
        musicVolumeButtons[i].setPosition(300.f + i * 225.f, 150.f);
        musicVolumeButtons[i].setScale(200.f / volumeTextures[i].getSize().x,
            200.f / volumeTextures[i].getSize().y);

        // Кнопки для SFX
        sfxVolumeButtons[i].setTexture(volumeTextures[i]);
        sfxVolumeButtons[i].setPosition(300.f + i * 225.f, 450.f);
        sfxVolumeButtons[i].setScale(200.f / volumeTextures[i].getSize().x,
            200.f / volumeTextures[i].getSize().y);


    }

    updateVolumeButtons();


    hoverBuffer.loadFromFile("assets/sounds/hover.ogg");
    hoverSound.setBuffer(hoverBuffer);
}

Settings::~Settings() {
    saveSettings();
}

void Settings::run() {
    shouldExit = false; // Сбрасываем флаг при запуске
    while (window.isOpen() && !shouldExit) { // Добавляем проверку флага
        processEvents();
        update();
        render();
    }
}

void Settings::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (isMouseOver(backButton)) {
                    returnToMainMenu();
                    return;
                }
                if (isMouseOver(resetButton)) {
                    resetSettings();
                }

                // Обработка изменения громкости музыки
                for (int i = 0; i < 6; ++i) {
                    if (isMouseOver(musicVolumeButtons[i])) {
                        musicVolume = i * 20.f;
                        updateVolumeButtons();
                        saveSettings();

                        // Применяем новую громкость к игре
                        if (Game* game = Game::getInstance()) {
                            game->setMusicVolume(musicVolume);
                        }
                    }
                    if (isMouseOver(sfxVolumeButtons[i])) {
                        sfxVolume = i * 20.f;
                        updateVolumeButtons();
                        saveSettings();
                    }
                }
            }
        }
    }
}

void Settings::returnToMainMenu() {
    shouldExit = true;
}

float Settings::getMusicVolume() const {
    return musicVolume;
}

float Settings::getGameMusicVolume() const {
    return musicVolume; // То же самое, что и getMusicVolume()
}

float Settings::getSFXVolume() const {
    return sfxVolume;
}

void Settings::setMusicVolume(float volume) {
    musicVolume = volume;
    updateVolumeButtons();
}

void Settings::setSFXVolume(float volume) {
    sfxVolume = volume;
    updateVolumeButtons();
}


void Settings::update() {
    handleMouseHover();
}

void Settings::render() {
    window.clear();
    window.draw(background);
    window.draw(backButton);
    window.draw(resetButton);

    window.draw(musicLabel);
    window.draw(sfxLabel);

    for (int i = 0; i < 6; ++i) {
        window.draw(musicVolumeButtons[i]);
        window.draw(sfxVolumeButtons[i]);
        window.draw(volumeLabels[i]);
    }

    window.display();
}

void Settings::updateVolumeButtons() {
    // Обновляем состояние кнопок громкости для музыки
    for (int i = 0; i < 6; ++i) {
        musicVolumeButtons[i].setColor(i * 20 <= musicVolume ? sf::Color::White : sf::Color(100, 100, 100));
    }

    // Обновляем состояние кнопок громкости для SFX
    for (int i = 0; i < 6; ++i) {
        sfxVolumeButtons[i].setColor(i * 20 <= sfxVolume ? sf::Color::White : sf::Color(100, 100, 100));
    }
}

void Settings::loadSettings() {
    std::ifstream file("settings.cfg");
    if (file.is_open()) {
        file >> musicVolume >> sfxVolume;
        file.close();
    }
}

void Settings::saveSettings() {
    std::ofstream file("settings.cfg");
    if (file.is_open()) {
        file << musicVolume << " " << sfxVolume;
        file.close();
    }
}

void Settings::resetSettings() {
    musicVolume = 100.f;
    sfxVolume = 100.f;
    updateVolumeButtons();
    saveSettings();
}

void Settings::handleMouseHover() {
    bool isHoveringNow = false;

    if (isMouseOver(backButton)) {
        backButton.setColor(sf::Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        backButton.setColor(sf::Color::White);
    }

    if (isMouseOver(resetButton)) {
        resetButton.setColor(sf::Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        resetButton.setColor(sf::Color::White);
    }

    // Проверка для кнопок громкости
    for (int i = 0; i < 6; ++i) {
        if (isMouseOver(musicVolumeButtons[i]) || isMouseOver(sfxVolumeButtons[i])) {
            isHoveringNow = true;
            break;
        }
    }

    // Воспроизводим звук только при новом наведении
    if (isHoveringNow && !wasHovering) {
        hoverSound.play();
    }

    wasHovering = isHoveringNow; // Обновляем флаг
}

bool Settings::isMouseOver(const sf::Sprite& sprite) const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
}





//Settings.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // Добавляем
#include <iostream>
#include <fstream>

class Settings {
public:
    Settings(sf::RenderWindow& window);
    ~Settings();

    void run();
    float getMusicVolume() const;
    float getGameMusicVolume() const; // Добавленный метод
    float getSFXVolume() const;
    void setMusicVolume(float volume);
    void setSFXVolume(float volume);
    void returnToMainMenu();

private:
    void processEvents();
    void update();
    void render();
    void updateVolumeButtons();
    void loadSettings();
    void saveSettings();
    void resetSettings();


    sf::RenderWindow& window;
    sf::Texture backgroundTexture;
    sf::Sprite background;
    sf::Texture backTexture;
    sf::Sprite backButton;
    sf::Texture resetTexture;
    sf::Sprite resetButton;

    sf::Texture volumeTextures[6];
    sf::Sprite musicVolumeButtons[6];
    sf::Sprite sfxVolumeButtons[6];
    sf::Text volumeLabels[6];

    sf::Font font;
    sf::Text musicLabel;
    sf::Text sfxLabel;

    float musicVolume = 100.f;
    float sfxVolume = 100.f;

    sf::SoundBuffer hoverBuffer;
    sf::Sound hoverSound;
    bool wasHovering = false;

    void handleMouseHover();
    bool isMouseOver(const sf::Sprite& sprite) const;
    bool shouldExit = false;


};




