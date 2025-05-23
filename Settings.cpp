//Settings.cpp
#include <iostream>
#include <fstream>
#include "Settings.h"
#include "Game.h"

Settings::Settings(sf::RenderWindow& win)
    : window(win) {
    // �������� ����
    if (!backgroundTexture.loadFromFile("assets/images/menu.png")) {
        std::cerr << "Failed to load settings background" << std::endl;
    }
    background.setTexture(backgroundTexture);

    // ������ �����
    if (!backTexture.loadFromFile("assets/images/back.png")) {
        std::cerr << "Failed to load back button texture" << std::endl;
    }
    backButton.setTexture(backTexture);
    backButton.setPosition(20.f, 20.f);
    backButton.setScale(0.18f, 0.18f);

    // ������ ������
    if (!resetTexture.loadFromFile("assets/images/resetbtn.png")) {
        std::cerr << "Failed to load reset button texture" << std::endl;
    }
    resetButton.setTexture(resetTexture);
    resetButton.setPosition(760.f, 700.f);
    resetButton.setScale(0.5f, 0.5f);

    // �������� ������
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font for settings" << std::endl;
    }

    // ���������
    musicLabel.setFont(font);
    musicLabel.setString("Music:");
    musicLabel.setCharacterSize(40);
    musicLabel.setPosition(50.f, 250.f);  // �������� �����
    musicLabel.setFillColor(sf::Color::White);

    sfxLabel.setFont(font);
    sfxLabel.setString("SFX:");
    sfxLabel.setCharacterSize(40);
    sfxLabel.setPosition(50.f, 550.f);   // �������� �����
    sfxLabel.setFillColor(sf::Color::White);

    // �������� �������� �� ����� ��� ������
    loadSettings();

    // �������� ������ ���������
    for (int i = 0; i < 6; ++i) {
        if (!volumeTextures[i].loadFromFile("assets/images/set" + std::to_string(i) + ".png")) {
            std::cerr << "Failed to load volume button " << i << std::endl;
        }

        // ������ ��� ������
        musicVolumeButtons[i].setTexture(volumeTextures[i]);
        musicVolumeButtons[i].setPosition(300.f + i * 225.f, 150.f);
        musicVolumeButtons[i].setScale(200.f / volumeTextures[i].getSize().x,
            200.f / volumeTextures[i].getSize().y);

        // ������ ��� SFX
        sfxVolumeButtons[i].setTexture(volumeTextures[i]);
        sfxVolumeButtons[i].setPosition(300.f + i * 225.f, 450.f);
        sfxVolumeButtons[i].setScale(200.f / volumeTextures[i].getSize().x,
            200.f / volumeTextures[i].getSize().y);


    }

    updateVolumeButtons();


    hoverBuffer.loadFromFile("assets/sounds/hover.ogg");
    hoverSound.setBuffer(hoverBuffer);
}

Settings::~Settings() {
    saveSettings();
}

void Settings::run() {
    shouldExit = false; // ���������� ���� ��� �������
    while (window.isOpen() && !shouldExit) { // ��������� �������� �����
        processEvents();
        update();
        render();
    }
}

void Settings::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (isMouseOver(backButton)) {
                    returnToMainMenu();
                    return;
                }
                if (isMouseOver(resetButton)) {
                    resetSettings();
                }

                // ��������� ��������� ��������� ������
                for (int i = 0; i < 6; ++i) {
                    if (isMouseOver(musicVolumeButtons[i])) {
                        musicVolume = i * 20.f;
                        updateVolumeButtons();
                        saveSettings();

                        // ��������� ����� ��������� � ����
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
    return musicVolume; // �� �� �����, ��� � getMusicVolume()
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

    window.draw(musicLabel);
    window.draw(sfxLabel);

    for (int i = 0; i < 6; ++i) {
        window.draw(musicVolumeButtons[i]);
        window.draw(sfxVolumeButtons[i]);
        window.draw(volumeLabels[i]);
    }

    window.display();
}

void Settings::updateVolumeButtons() {
    // ��������� ��������� ������ ��������� ��� ������
    for (int i = 0; i < 6; ++i) {
        musicVolumeButtons[i].setColor(i * 20 <= musicVolume ? sf::Color::White : sf::Color(100, 100, 100));
    }

    // ��������� ��������� ������ ��������� ��� SFX
    for (int i = 0; i < 6; ++i) {
        sfxVolumeButtons[i].setColor(i * 20 <= sfxVolume ? sf::Color::White : sf::Color(100, 100, 100));
    }
}

void Settings::loadSettings() {
    std::ifstream file("settings.cfg");
    if (file.is_open()) {
        file >> musicVolume >> sfxVolume;
        file.close();
    }
}

void Settings::saveSettings() {
    std::ofstream file("settings.cfg");
    if (file.is_open()) {
        file << musicVolume << " " << sfxVolume;
        file.close();
    }
}

void Settings::resetSettings() {
    musicVolume = 100.f;
    sfxVolume = 100.f;
    updateVolumeButtons();
    saveSettings();
}

void Settings::handleMouseHover() {
    bool isHoveringNow = false;

    if (isMouseOver(backButton)) {
        backButton.setColor(sf::Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        backButton.setColor(sf::Color::White);
    }

    if (isMouseOver(resetButton)) {
        resetButton.setColor(sf::Color(255, 200, 255, 255));
        isHoveringNow = true;
    }
    else {
        resetButton.setColor(sf::Color::White);
    }

    // �������� ��� ������ ���������
    for (int i = 0; i < 6; ++i) {
        if (isMouseOver(musicVolumeButtons[i]) || isMouseOver(sfxVolumeButtons[i])) {
            isHoveringNow = true;
            break;
        }
    }

    // ������������� ���� ������ ��� ����� ���������
    if (isHoveringNow && !wasHovering) {
        hoverSound.play();
    }

    wasHovering = isHoveringNow; // ��������� ����
}

bool Settings::isMouseOver(const sf::Sprite& sprite) const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
}