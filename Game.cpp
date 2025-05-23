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
        case 4: showStatistics(); break;
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
    if (currentLevel < 0 || currentLevel >= 5 || !levels[currentLevel]) return;

    menuMusic.stop();
    levels[currentLevel]->playMusic();
    if (settings) {
        levels[currentLevel]->setMusicVolume(settings->getMusicVolume());
    }
    isPaused = false;
    levelClock.restart();

    while (window.isOpen() && !inMenu) {
        processEvents();

        if (levels[currentLevel]->isFailed()) {
            showGameOverMenu();
            continue;
        }

        if (levels[currentLevel]->isCompleted()) {
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
            if (isPaused) {
                levels[currentLevel]->pauseMusic();
            }
            else {
                levels[currentLevel]->playMusic();
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

void Game::showStatistics() {
    sf::RenderWindow statsWindow(sf::VideoMode(800, 600), "Statistics");
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        return;
    }

    sf::Text statsText;
    statsText.setFont(font);
    statsText.setString("Game Statistics\n\nComing Soon...");
    statsText.setCharacterSize(40);
    statsText.setPosition(100.f, 100.f);
    statsText.setFillColor(sf::Color::White);

    while (statsWindow.isOpen()) {
        sf::Event event;
        while (statsWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                statsWindow.close();
            }
        }

        statsWindow.clear(sf::Color(50, 50, 70));
        statsWindow.draw(statsText);
        statsWindow.display();
    }
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