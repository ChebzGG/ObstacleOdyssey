// Game.cpp
#include "Game.h"
#include "Settings.h"
#include <iostream>
using namespace std;
using namespace sf;

Game* Game::instance = nullptr;

Game::Game()
    : window(VideoMode(1920, 1080), "Obstacle Odyssey", Style::Fullscreen/**/),
    menu(window),
    currentLevel(-1),
    isPaused(false),
    inMenu(true),
    settings(nullptr) {

    instance = this;

    ifstream file("settings.txt");
    float volume = 100.f; 
    if (file.is_open()) {
        file >> volume;
        file.close();
    }

    menuMusic.openFromFile("assets/music/menu.ogg");
    menuMusic.setLoop(true);
    menuMusic.setVolume(volume); 
    menuMusic.play();

    for (int i = 0; i < 5; ++i) {
        levels[i] = nullptr;
    }

    pauseBackgroundTexture.loadFromFile("assets/images/BGpause.png");
    pauseBackground.setTexture(pauseBackgroundTexture);

    resumeButtonTexture.loadFromFile("assets/images/resume.png");
    resumeButton.setTexture(resumeButtonTexture);
    resumeButton.setPosition(1020.f, 340.f);

    exitButtonTexture.loadFromFile("assets/images/exit2.png");
    exitButton.setTexture(exitButtonTexture);
    exitButton.setPosition(500.f, 340.f);

    window.setFramerateLimit(60);

}

Settings* Game::getSettings() {
    if (!settings) { 
        settings = new Settings(window); 
    }
    return settings;
}


Game::~Game() {
    for (int i = 0; i < 5; ++i) {
        delete levels[i];
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

    settings->run();

    menuMusic.setVolume(settings->getMusicVolume());

    if (currentLevel >= 0 && currentLevel < 5 && levels[currentLevel]) {
        levels[currentLevel]->setMusicVolume(settings->getMusicVolume());
    }
}

void Game::startLevel() {
    if (currentLevel < 0 || currentLevel >= 5) return;

    delete levels[currentLevel];

    string levelPath = "assets/levels/level" + to_string(currentLevel + 1) + ".txt";
    string musicPath = "assets/music/level" + to_string(currentLevel + 1) + ".ogg";
    levels[currentLevel] = new Level(levelPath, musicPath);

    float volume = settings ? settings->getMusicVolume() : 100.f;
    levels[currentLevel]->setMusicVolume(volume);

    menuMusic.stop();
    levels[currentLevel]->playMusic();
    isPaused = false;
    levelClock.restart();

    while (window.isOpen() && !inMenu) {
        processEvents();

        if (currentLevel >= 0 && currentLevel < 5 && levels[currentLevel] && levels[currentLevel]->isFailed()) {
            showGameOverMenu();
            continue;
        }

        if (currentLevel >= 0 && currentLevel < 5 && levels[currentLevel] && levels[currentLevel]->isCompleted()) {
            showVictoryMenu();
            continue;
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
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
        }

        if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
            isPaused = !isPaused;
            levels[currentLevel]->setPaused(isPaused);
            if (isPaused) {
                levels[currentLevel]->pauseMusic();
            }
            else {
                levels[currentLevel]->playMusic();
            }
        }

        if (isPaused && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
            Vector2f mousePos = Vector2f(Mouse::getPosition(window));

            if (resumeButton.getGlobalBounds().contains(mousePos)) {
                isPaused = false;
                levels[currentLevel]->setPaused(false);
                levels[currentLevel]->playMusic();
            }
            else if (exitButton.getGlobalBounds().contains(mousePos)) {
                levels[currentLevel]->stopMusic();
                levels[currentLevel]->setPaused(false);
                currentLevel = -1;
                isPaused = false;
                inMenu = true;

                ifstream file("settings.txt");
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
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                isPaused = false;
                levels[currentLevel]->playMusic();
            }
        }
        render();
    }
}




void Game::showGameOverMenu() {
    if (currentLevel < 0 || currentLevel >= 5 || !levels[currentLevel]) return;

    Texture resetTexture, retryTexture, exitTexture;
    Sprite resetBackground, retryButton, exitButton;

    resetTexture.loadFromFile("assets/images/BGlost.png");
    retryTexture.loadFromFile("assets/images/retry.png");
    exitTexture.loadFromFile("assets/images/exit2.png");

    resetBackground.setTexture(resetTexture);
    retryButton.setTexture(retryTexture);
    exitButton.setTexture(exitTexture);

    retryButton.setPosition(1020.f, 340.f);
    exitButton.setPosition(500.f, 340.f);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
        }



        retryButton.setColor(isMouseOver(retryButton) ? Color(255, 200, 255, 255) : Color::White);
        exitButton.setColor(isMouseOver(exitButton) ? Color(255, 200, 255, 255) : Color::White);

        if (Mouse::isButtonPressed(Mouse::Left)) {
            Vector2f mousePos = Vector2f(Mouse::getPosition(window));
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

        window.display();
    }
}

void Game::showVictoryMenu() {
    Texture winTexture, retryTexture, exitTexture;
    Sprite winBackground, retryButton, exitButton;

    winTexture.loadFromFile("assets/images/BGwin.png");
    retryTexture.loadFromFile("assets/images/retry.png");
    exitTexture.loadFromFile("assets/images/exit2.png");

    winBackground.setTexture(winTexture);
    retryButton.setTexture(retryTexture);
    exitButton.setTexture(exitTexture);

    retryButton.setPosition(1020.f, 340.f);
    exitButton.setPosition(500.f, 340.f);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
        }

        retryButton.setColor(isMouseOver(retryButton) ? Color(255, 200, 255, 255) : Color::White);
        exitButton.setColor(isMouseOver(exitButton) ? Color(255, 200, 255, 255) : Color::White);

        if (Mouse::isButtonPressed(Mouse::Left)) {
            Vector2f mousePos = Vector2f(Mouse::getPosition(window));
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
        window.draw(winBackground);
        window.draw(retryButton);
        window.draw(exitButton);



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

    ifstream file("settings.txt");
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
    resumeButton.setColor(isMouseOver(resumeButton) ? Color(255, 200, 255, 255) : Color::White);
    exitButton.setColor(isMouseOver(exitButton) ? Color(255, 200, 255, 255) : Color::White);
}

bool Game::isMouseOver(const Sprite& sprite) const {
    FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(Vector2f(Mouse::getPosition(window)));
}

void Game::setMusicVolume(float volume) {
    menuMusic.setVolume(volume);

    for (int i = 0; i < 5; ++i) {
        if (levels[i]) {
            levels[i]->setMusicVolume(volume);
        }
    }

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
