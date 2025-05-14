// LevelSelect.cpp
#include "LevelSelect.h"
#include <iostream>

LevelSelect::LevelSelect(sf::RenderWindow& win) : window(win) {
    if (!backgroundTexture.loadFromFile("assets/images/menu.png")) {
        std::cerr << "Failed to load menu background\n";
    }
    background.setTexture(backgroundTexture);

    if (!lvlTexture.loadFromFile("assets/images/levels.png")) {
        std::cerr << "Failed to load levels texture\n";
    }
    lvl.setTexture(lvlTexture);
    lvl.setPosition(680.f, 20.f);
    lvl.setScale(1.0f, 1.0f);

    if (!backTexture.loadFromFile("assets/images/back.png")) {
        std::cerr << "Failed to load back button texture\n";
    }
    backButton.setTexture(backTexture);
    backButton.setPosition(20.f, 20.f);
    backButton.setScale(0.18f, 0.18f);

    for (int i = 0; i < 5; ++i) {
        if (!levelTextures[i].loadFromFile("assets/images/" + std::to_string(i + 1) + ".png")) {
            std::cerr << "Failed to load level " << i + 1 << " texture\n";
        }
        levelButtons[i].setTexture(levelTextures[i]);

        if (i % 2 == 0) {
            levelButtons[i].setPosition(190.f + (i * 320.f), 300.f);
        }
        else {
            levelButtons[i].setPosition(190.f + (i * 320.f), 400.f);
        }
        levelButtons[i].setScale(0.7f, 0.7f);
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
    for (int i = 0; i < 5; ++i) {
        if (isMouseOver(levelButtons[i])) {
            levelButtons[i].setColor(sf::Color(255, 200, 255, 255));
        }
        else {
            levelButtons[i].setColor(sf::Color::White);
        }
    }
    backButton.setColor(isMouseOver(backButton) ? sf::Color(255, 200, 255, 255) : sf::Color::White);
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