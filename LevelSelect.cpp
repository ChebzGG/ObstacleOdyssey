// LevelSelect.cpp
#include "LevelSelect.h"
#include <iostream>
#include "Game.h"
#include "Settings.h"
using namespace std;
using namespace sf;

LevelSelect::LevelSelect(RenderWindow& win) : window(win), selectedLevel(0) {
    backgroundTexture.loadFromFile("assets/images/BGlevels.png");
    background.setTexture(backgroundTexture);

    lvlTexture.loadFromFile("assets/images/levels.png");
    lvl.setTexture(lvlTexture);
    lvl.setPosition(610.f, 50.f);
    lvl.setScale(1.0f, 1.0f);

    backTexture.loadFromFile("assets/images/back.png");
    backButton.setTexture(backTexture);
    backButton.setPosition(20.f, 20.f);
    backButton.setScale(0.18f, 0.18f);

    for (int i = 0; i < 5; ++i) {
        levelTextures[i].loadFromFile("assets/images/" + to_string(i + 1) + ".png");
        levelButtons[i].setTexture(levelTextures[i]);

        if (i % 2 == 0) {
            levelButtons[i].setPosition(180.f + (i * 320.f), 300.f);
        }
        else {
            levelButtons[i].setPosition(180.f + (i * 320.f), 400.f);
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
        if (selectedLevel != 0 || selectedLevel == -1) {
            break;
        }
    }

    if (selectedLevel > 0) {
        if (Game::getInstance() && Game::getInstance()->getSettings()) {
            Game::getInstance()->getSettings()->incrementAttempts();
            Game::getInstance()->getSettings()->saveSettings();
        }
    }
}

void LevelSelect::processEvents() {
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
        }

        if (event.type == Event::MouseButtonPressed) {
            if (isMouseOver(backButton)) {
                selectedLevel = -1;
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
            levelButtons[i].setColor(Color(255, 200, 255, 255));
            isHoveringNow = true;
        }
        else {
            levelButtons[i].setColor(Color::White);
        }
    }

    if (isMouseOver(backButton)) {
        backButton.setColor(Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        backButton.setColor(Color::White);
    }

    if (isHoveringNow && !wasHovering) {
        float sfxVolume = 100.f;
        if (Game::getInstance() && Game::getInstance()->getSettings()) {
            sfxVolume = Game::getInstance()->getSettings()->getSFXVolume();
        }
        hoverSounds[hoverSoundIndex].setBuffer(hoverBuffer);
        hoverSounds[hoverSoundIndex].setVolume(sfxVolume);
        hoverSounds[hoverSoundIndex].play();
        hoverSoundIndex = (hoverSoundIndex + 1) % HOVER_SOUND_POOL;
    }

    wasHovering = isHoveringNow; 
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

bool LevelSelect::isMouseOver(const Sprite& sprite) {
    FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(Vector2f(Mouse::getPosition(window)));
}

int LevelSelect::getSelectedLevel() const {
    return selectedLevel;
}