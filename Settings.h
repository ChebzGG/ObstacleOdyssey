//Settings.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // Добавляем
#include <iostream>
#include <fstream>

class Settings {
public:
    Settings(sf::RenderWindow& window);
    ~Settings();

    void run();
    float getMusicVolume() const;
    float getGameMusicVolume() const; // Добавленный метод
    float getSFXVolume() const;
    void setMusicVolume(float volume);
    void setSFXVolume(float volume);
    void returnToMainMenu();

private:
    void processEvents();
    void update();
    void render();
    void updateVolumeButtons();
    void loadSettings();
    void saveSettings();
    void resetSettings();


    sf::RenderWindow& window;
    sf::Texture backgroundTexture;
    sf::Sprite background;
    sf::Texture backTexture;
    sf::Sprite backButton;
    sf::Texture resetTexture;
    sf::Sprite resetButton;

    sf::Texture volumeTextures[6];
    sf::Sprite musicVolumeButtons[6];
    sf::Sprite sfxVolumeButtons[6];
    sf::Text volumeLabels[6];

    sf::Font font;
    sf::Text musicLabel;
    sf::Text sfxLabel;

    float musicVolume = 100.f;
    float sfxVolume = 100.f;

    sf::SoundBuffer hoverBuffer;
    sf::Sound hoverSound;
    bool wasHovering = false;

    void handleMouseHover();
    bool isMouseOver(const sf::Sprite& sprite) const;
    bool shouldExit = false;


};