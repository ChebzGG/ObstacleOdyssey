// Stats.cpp
#include "Stats.h"
#include <iostream>
using namespace std;
using namespace sf;

Stats::Stats(RenderWindow& win)
    : window(win)
{
    backgroundTexture.loadFromFile("assets/images/BGstats.png");
    background.setTexture(backgroundTexture);

    logoTexture.loadFromFile("assets/images/statistics.png");
    logo.setTexture(logoTexture);
    logo.setPosition(450.f, 50.f);

    backTexture.loadFromFile("assets/images/back.png");
    backButton.setTexture(backTexture);
    backButton.setPosition(20.f, 20.f);
    backButton.setScale(0.18f, 0.18f);


    for (int i = 0; i < 10; ++i) {
        digitTextures[i].loadFromFile("assets/images/logo - " + to_string(i) + ".png");
        digitSprites[i].setTexture(digitTextures[i]);
    }

    font.loadFromFile("assets/fonts/arial.ttf");
    hoverBuffer.loadFromFile("assets/sounds/hover.ogg");
    hoverSound.setBuffer(hoverBuffer);


}

void Stats::run() {
    shouldExit = false; 
    while (window.isOpen() && !shouldExit) { 
        processEvents();
        render();
    }
}

void Stats::processEvents() {
    bool isHoveringNow = false;
    Event event;

    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
        }
        if (event.type == Event::MouseButtonPressed &&
            event.mouseButton.button == Mouse::Left &&
            isMouseOver(backButton)) {
            shouldExit = true;
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

void Stats::render() {
    window.clear();
    window.draw(background);
    window.draw(logo);

    font.loadFromFile("assets/fonts/arial.ttf");

    Settings* settings = Game::getInstance()->getSettings();
    if (settings) {
        vector<string> statsStrings = {
            to_string(settings->getAttempts()),
            to_string(settings->getJumps()),
            to_string(settings->getLevelsCompleted()),
            to_string(settings->getCoins())
        };

        float startY = 250.f;
        float startX = 1410.f;
        float digitSpacing = 90.f; 
        float lineSpacing = 178.f; 

        for (size_t line = 0; line < statsStrings.size(); ++line) {
            float x = startX;
            for (char ch : statsStrings[line]) {
                if (ch >= '0' && ch <= '9') {
                    int digit = ch - '0';
                    digitSprites[digit].setPosition(x, startY + line * lineSpacing);
                    digitSprites[digit].setScale(0.8f, 0.8f);
                    window.draw(digitSprites[digit]);
                    x += digitSpacing;
                }
            }
        }
    }

    window.draw(backButton);
    window.display();
}

bool Stats::isMouseOver(const Sprite& sprite) {
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    return sprite.getGlobalBounds().contains(mousePos);
}