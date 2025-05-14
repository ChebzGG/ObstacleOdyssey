// Menu.cpp
#include "Menu.h"
#include "LevelSelect.h"
#include <iostream>

Menu::Menu(sf::RenderWindow& win) : window(win) {
    if (!backgroundTexture.loadFromFile("assets/images/menu.png")) {
        std::cerr << "Failed to load menu background\n";
    }
    background.setTexture(backgroundTexture);

    if (!logoTexture.loadFromFile("assets/images/logo.png")) {
        std::cerr << "Failed to load logo\n";
    }
    logo.setTexture(logoTexture);
    logo.setPosition(150.f, 50.f);

    if (!playTexture.loadFromFile("assets/images/play.png")) {
        std::cerr << "Failed to load play button\n";
    }
    playButton.setTexture(playTexture);
    playButton.setPosition(760.f, 350.f);
    playButton.setScale(1.0f, 1.0f);

    if (!customTexture.loadFromFile("assets/images/custom.png")) {
        std::cerr << "Failed to load custom button\n";
    }
    customButton.setTexture(customTexture);
    customButton.setPosition(360.f, 415.f);
    customButton.setScale(0.7f, 0.7f);

    if (!settingsTexture.loadFromFile("assets/images/settings.png")) {
        std::cerr << "Failed to load settings button\n";
    }
    settingsButton.setTexture(settingsTexture);
    settingsButton.setPosition(1280.f, 415.f);
    settingsButton.setScale(0.7f, 0.7f);

    if (!exitTexture.loadFromFile("assets/images/exit.png")) {
        std::cerr << "Failed to load exit button\n";
    }
    exitButton.setTexture(exitTexture);
    exitButton.setPosition(20.f, 20.f);
    exitButton.setScale(0.18f, 0.18f);

    if (!statsTexture.loadFromFile("assets/images/stats.png")) {
        std::cerr << "Failed to load stats button\n";
    }
    statsButton.setTexture(statsTexture);
    statsButton.setPosition(1730.f, 870.f);
    statsButton.setScale(0.4f, 0.4f);

    if (!menuMusic.openFromFile("assets/music/menu_music.ogg")) {
        std::cerr << "Failed to load menu music\n";
    }
    else {
        menuMusic.setLoop(true);
        menuMusic.play();
    }
}

void Menu::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
        if (selectedOption != -1) {
            handleMenuSelection();
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
            else if (isMouseOver(settingsButton)) selectedOption = 3;
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
    playButton.setColor(isMouseOver(playButton) ? sf::Color(255, 200, 255, 255) : sf::Color::White);
    customButton.setColor(isMouseOver(customButton) ? sf::Color(255, 200, 255, 255) : sf::Color::White);
    settingsButton.setColor(isMouseOver(settingsButton) ? sf::Color(255, 200, 255, 255) : sf::Color::White);
    exitButton.setColor(isMouseOver(exitButton) ? sf::Color(255, 200, 255, 255) : sf::Color::White);
    statsButton.setColor(isMouseOver(statsButton) ? sf::Color(255, 200, 255, 255) : sf::Color::White);
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
    if (selectedOption == 1) {
        LevelSelect levelSelect(window);
        levelSelect.run();
        chosenLevel = levelSelect.getSelectedLevel() - 1;
        selectedOption = -1;
    }
    else if (selectedOption == 4) {
        // Statistics (to be implemented)
    }
}