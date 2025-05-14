#include "Game.h"
#include <iostream>

Game::Game()
    : window(sf::VideoMode(1920, 1080), "Obstacle Odyssey", sf::Style::Fullscreen),
    menu(window) {

    if (!menuMusic.openFromFile("assets/music/menu.ogg")) {
        std::cerr << "Failed to load menu music\n";
    }
    menuMusic.setLoop(true);
    menuMusic.play();

    levels[0] = new Level("levels/level1.txt", "assets/music/level1.ogg");
    levels[1] = new Level("levels/level2.txt", "assets/music/level2.ogg");
    levels[2] = new Level("levels/level3.txt", "assets/music/level3.ogg");
    levels[3] = new Level("levels/level4.txt", "assets/music/level4.ogg");
    levels[4] = new Level("levels/level5.txt", "assets/music/level5.ogg");

    loadPlayerTexture();
}

Game::~Game() {
    for (int i = 0; i < 5; ++i) {
        delete levels[i];
    }
}

void Game::run() {
    while (window.isOpen()) {
        if (inMenu) {
            handleMenuState();
        }
        else {
            handleGameState();
        }
    }
}

void Game::handleMenuState() {
    menu.run();
    int menuSelection = menu.getMenuSelection();

    switch (menuSelection) {
    case 1: // Play
        handleLevelSelection();
        break;
    case 2: // Custom
        // Customization logic
        break;
    case 3: // Settings
        // Settings logic
        break;
    case 4: // Statistics
        showStatistics();
        break;
    case -1: // Exit
        window.close();
        break;
    }
}

void Game::handleLevelSelection() {
    LevelSelect levelSelect(window);
    levelSelect.run();

    int selected = levelSelect.getSelectedLevel();
    if (selected == -1) { // Back button
        return;
    }

    if (selected >= 1 && selected <= 5) {
        currentLevel = selected - 1;
        startLevel();
    }
}

void Game::startLevel() {
    levelAttempts[currentLevel]++;
    levels[currentLevel]->restart();
    levels[currentLevel]->playMusic();
    levelClock.restart();
    inMenu = false;
    isPaused = false;
}

void Game::handleGameState() {
    processEvents();

    if (!isPaused) {
        update();
    }

    render();
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                isPaused = true;
                showPauseMenu();
            }
        }
    }
}

void Game::update() {
    levels[currentLevel]->update();

    if (levels[currentLevel]->isFailed()) {
        restartLevel();
        return;
    }

    if (levels[currentLevel]->isCompleted()) {
        completeLevel();
    }
}

void Game::render() {
    window.clear();
    levels[currentLevel]->render(window);
    window.display();
}

void Game::showPauseMenu() {
    levels[currentLevel]->stopMusic();

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font\n";
        return;
    }

    sf::Text pauseText("PAUSED", font, 80);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(window.getSize().x / 2 - pauseText.getGlobalBounds().width / 2, 300);

    sf::Text resumeText("Continue (Space)", font, 40);
    resumeText.setFillColor(sf::Color::White);
    resumeText.setPosition(window.getSize().x / 2 - resumeText.getGlobalBounds().width / 2, 450);

    sf::Text menuText("Main Menu (Esc)", font, 40);
    menuText.setFillColor(sf::Color::White);
    menuText.setPosition(window.getSize().x / 2 - menuText.getGlobalBounds().width / 2, 520);

    sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    while (isPaused && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    isPaused = false;
                    levels[currentLevel]->playMusic();
                    return;
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    returnToMainMenu();
                    return;
                }
            }
        }

        window.clear();
        levels[currentLevel]->render(window);
        window.draw(overlay);
        window.draw(pauseText);
        window.draw(resumeText);
        window.draw(menuText);
        window.display();
    }
}

void Game::restartLevel() {
    levelAttempts[currentLevel]++;
    levels[currentLevel]->restart();
    levelClock.restart();
}

void Game::completeLevel() {
    float timeSpent = levelClock.getElapsedTime().asSeconds();
    int coins = levels[currentLevel]->getCoinsCollected();

    if (levelBestTimes[currentLevel] == 0 || timeSpent < levelBestTimes[currentLevel]) {
        levelBestTimes[currentLevel] = timeSpent;
    }
    levelCoins[currentLevel] = coins;
    totalCoins += coins;

    levels[currentLevel]->stopMusic();
    showLevelResults(timeSpent, coins);

    if (currentLevel < 4) {
        currentLevel++;
        startLevel();
    }
    else {
        returnToMainMenu();
    }
}

void Game::showLevelResults(float time, int coins) {
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font\n";
        return;
    }

    sf::Text resultText("LEVEL COMPLETE!", font, 60);
    resultText.setFillColor(sf::Color::White);
    resultText.setPosition(window.getSize().x / 2 - resultText.getGlobalBounds().width / 2, 250);

    sf::Text timeText("Time: " + std::to_string(time) + "s", font, 40);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(window.getSize().x / 2 - timeText.getGlobalBounds().width / 2, 350);

    sf::Text coinsText("Coins: " + std::to_string(coins) + "/3", font, 40);
    coinsText.setFillColor(sf::Color::White);
    coinsText.setPosition(window.getSize().x / 2 - coinsText.getGlobalBounds().width / 2, 420);

    sf::Text nextText("Press Space to continue", font, 30);
    nextText.setFillColor(sf::Color::White);
    nextText.setPosition(window.getSize().x / 2 - nextText.getGlobalBounds().width / 2, 520);

    sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));

    window.clear();
    levels[currentLevel]->render(window);
    window.draw(overlay);
    window.draw(resultText);
    window.draw(timeText);
    window.draw(coinsText);
    window.draw(nextText);
    window.display();

    bool waiting = true;
    while (waiting && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                waiting = false;
            }
        }
    }
}

void Game::showStatistics() {
    // Implementation of statistics screen
}

void Game::returnToMainMenu() {
    inMenu = true;
    currentLevel = -1;
    menuMusic.play();
}

void Game::loadPlayerTexture() {
    if (!playerTexture.loadFromFile(currentSkin)) {
        if (!playerTexture.loadFromFile("assets/images/icon0.png")) {
            std::cerr << "Failed to load player texture\n";
        }
    }

    for (int i = 0; i < 5; ++i) {
        levels[i]->setPlayerTexture(&playerTexture);
    }
}