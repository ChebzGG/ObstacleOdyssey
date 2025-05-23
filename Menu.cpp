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