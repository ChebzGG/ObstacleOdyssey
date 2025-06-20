// Menu.cpp
#include "Menu.h"
#include "LevelSelect.h"
#include "Stats.h"
#include "Custom.h"
#include <iostream>
using namespace std;
using namespace sf;

Menu::Menu(RenderWindow& win) : window(win) {
    backgroundTexture.loadFromFile("assets/images/menu.png");
    background.setTexture(backgroundTexture);

    logoTexture.loadFromFile("assets/images/logo.png");
    logo.setTexture(logoTexture);
    logo.setPosition(155.f, 50.f);

    playTexture.loadFromFile("assets/images/play.png");
    playButton.setTexture(playTexture);
    playButton.setPosition(760.f, 350.f);
    playButton.setScale(1.0f, 1.0f);

    customTexture.loadFromFile("assets/images/custom.png");
    customButton.setTexture(customTexture);
    customButton.setPosition(360.f, 415.f);
    customButton.setScale(0.7f, 0.7f);

    settingsTexture.loadFromFile("assets/images/setting.png");
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

    if (selectedOption == 2) { 
        Custom customMenu(window);
        customMenu.run(); 
    }
}


void Menu::processEvents() {
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
        }

        if (event.type == Event::MouseButtonPressed) {
            if (isMouseOver(playButton)) selectedOption = 1;
            else if (isMouseOver(customButton)) selectedOption = 2;
            else if (isMouseOver(settingsButton)) selectedOption = 3;
            else if (isMouseOver(exitButton)) window.close();
            else if (isMouseOver(statsButton)) {
                Stats statsMenu(window);
                statsMenu.run();
            }
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
        playButton.setColor(Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        playButton.setColor(Color::White);
    }

    if (isMouseOver(customButton)) {
        customButton.setColor(Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        customButton.setColor(Color::White);
    }

    if (isMouseOver(settingsButton)) {
        settingsButton.setColor(Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        settingsButton.setColor(Color::White);
    }

    if (isMouseOver(exitButton)) {
        exitButton.setColor(Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        exitButton.setColor(Color::White);
    }

    if (isMouseOver(statsButton)) {
        statsButton.setColor(Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        statsButton.setColor(Color::White);
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

bool Menu::isMouseOver(const Sprite& sprite) {
    FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(Vector2f(Mouse::getPosition(window)));
}

int Menu::getMenuSelection() const {
    return selectedOption;
}

int Menu::getChosenLevel() const {
    return chosenLevel;
}

void Menu::handleMenuSelection() {

}