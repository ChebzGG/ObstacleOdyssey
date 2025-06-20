//Settings.cpp
#include <iostream>
#include <fstream>
#include "Settings.h"
#include "Game.h"
using namespace std;
using namespace sf;

Settings::Settings(RenderWindow& win)
    : window(win) {

    backgroundTexture.loadFromFile("assets/images/BGsettings.png");
    background.setTexture(backgroundTexture);


    logoTexture.loadFromFile("assets/images/settings.png");
    logo.setTexture(logoTexture);
    logo.setPosition(470.f, 50.f);

    backTexture.loadFromFile("assets/images/back.png");
    backButton.setTexture(backTexture);
    backButton.setPosition(20.f, 20.f);
    backButton.setScale(0.18f, 0.18f);

    loadSettings();

    for (int i = 0; i < 6; ++i) {
        volumeTextures[i].loadFromFile("assets/images/set" + to_string(i) + ".png");
        
        musicVolumeButtons[i].setTexture(volumeTextures[i]);
        musicVolumeButtons[i].setPosition(500.f + i * 225.f, 250.f);
        musicVolumeButtons[i].setScale(200.f / volumeTextures[i].getSize().x,
            200.f / volumeTextures[i].getSize().y);

        sfxVolumeButtons[i].setTexture(volumeTextures[i]);
        sfxVolumeButtons[i].setPosition(500.f + i * 225.f, 480.f);
        sfxVolumeButtons[i].setScale(200.f / volumeTextures[i].getSize().x,
            200.f / volumeTextures[i].getSize().y);
    }

    resetTexture.loadFromFile("assets/images/resetbtn.png");
    resetButton.setTexture(resetTexture);
    resetButton.setPosition(760.f, 700.f);
    resetButton.setScale(0.5f, 0.5f);

    musicTexture.loadFromFile("assets/images/music.png");
    musicButton.setTexture(musicTexture);
    musicButton.setPosition(20.f, 300.f);
    musicButton.setScale(0.6f, 0.6f);

    sfxTexture.loadFromFile("assets/images/sfx.png");
    sfxButton.setTexture(sfxTexture);
    sfxButton.setPosition(20.f, 530.f);
    sfxButton.setScale(0.6f, 0.6f);

    updateVolumeButtons();

    hoverBuffer.loadFromFile("assets/sounds/hover.ogg");
    hoverSound.setBuffer(hoverBuffer);
}

Settings::~Settings() {
    saveSettings();
}

void Settings::run() {
    shouldExit = false;
    while (window.isOpen() && !shouldExit) {
        processEvents();
        update();
        render();
    }
}

void Settings::processEvents() {
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
        }

        if (event.type == Event::MouseButtonPressed) {
            if (event.mouseButton.button == Mouse::Left) {
                if (isMouseOver(backButton)) {
                    returnToMainMenu();
                    return;
                }
                if (isMouseOver(resetButton)) {
                    resetSettings();
                }

                for (int i = 0; i < 6; ++i) {
                    if (isMouseOver(musicVolumeButtons[i])) {
                        musicVolume = i * 20.f;
                        updateVolumeButtons();
                        saveSettings();

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
    return musicVolume; 
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
    window.draw(musicButton);
    window.draw(sfxButton);
    window.draw(logo);

    for (int i = 0; i < 6; ++i) {
        window.draw(musicVolumeButtons[i]);
        window.draw(sfxVolumeButtons[i]);
        window.draw(volumeLabels[i]);
    }

    window.display();
}

void Settings::updateVolumeButtons() {
    for (int i = 0; i < 6; ++i) {
        musicVolumeButtons[i].setColor(i * 20 <= musicVolume ? Color::White : Color(100, 100, 100));
    }

    for (int i = 0; i < 6; ++i) {
        sfxVolumeButtons[i].setColor(i * 20 <= sfxVolume ? Color::White : Color(100, 100, 100));
    }
}

void Settings::loadSettings() {
    ifstream file("settings.txt");
    if (file.is_open()) {
        file >> musicVolume >> sfxVolume >> attempts >> jumps >> levelsCompleted >> coinsCollected >> coinsCustom;
        for (int i = 0; i < 9; ++i) {
            file >> iconStates[i];
        }
        file.close();
    }
    else {
        resetSettings();
    }

    if (Game* game = Game::getInstance()) {
        game->setMusicVolume(musicVolume);
    }
}

void Settings::saveSettings() {
    ofstream file("settings.txt");
    if (file.is_open()) {
        file << musicVolume << " "
            << sfxVolume << " "
            << attempts << " "
            << jumps << " "
            << levelsCompleted << " "
            << coinsCollected << " "
            << coinsCustom;

    }
}

void Settings::resetSettings() {
    musicVolume = 100.f;
    sfxVolume = 100.f;
    attempts = 0;
    jumps = 0;
    levelsCompleted = 0;
    coinsCollected = 0;
    coinsCustom = 0;

    updateVolumeButtons();
    saveSettings();
}

void Settings::resetIcons() {
    ofstream file("icon.txt");
    if (file.is_open()) {
        file << "2 0 0 0 0 0 0 0 0";
        file.close();
    }
}

void Settings::handleMouseHover() {
    bool isHoveringNow = false;

    if (isMouseOver(backButton)) {
        backButton.setColor(Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        backButton.setColor(Color::White);
    }

    if (isMouseOver(resetButton)) {
        resetButton.setColor(Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        resetButton.setColor(Color::White);
    }

    for (int i = 0; i < 6; ++i) {
        if (isMouseOver(musicVolumeButtons[i])) {
            musicVolumeButtons[i].setColor(Color(255, 200, 255, 255));
            isHoveringNow = true;
        }
        else {
            musicVolumeButtons[i].setColor(i * 20 <= musicVolume ? Color::White : Color(100, 100, 100));
        }
    }

    for (int i = 0; i < 6; ++i) {
        if (isMouseOver(sfxVolumeButtons[i])) {
            sfxVolumeButtons[i].setColor(Color(255, 200, 255, 255));
            isHoveringNow = true;
        }
        else {
            sfxVolumeButtons[i].setColor(i * 20 <= sfxVolume ? Color::White : Color(100, 100, 100));
        }
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

bool Settings::isMouseOver(const Sprite& sprite) const {
    FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(Vector2f(Mouse::getPosition(window)));
}

void Settings::incrementAttempts() {
    attempts++;
    saveSettings(); 
}

void Settings::incrementJumps() {
    jumps++;
    saveSettings();
}

void Settings::incrementLevels() {
    levelsCompleted++;
    saveSettings();
}

void Settings::addCoins(int amount) {
    coinsCollected += amount;
    saveSettings();
}

void Settings::addCustomCoins(int amount) {
    coinsCustom += amount;
    saveSettings();
}

int Settings::getCustomCoins() const {
    return coinsCustom; 
}

