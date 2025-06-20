//Settings.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include <iostream>
#include <fstream>

class Settings {
public:
    Settings(sf::RenderWindow& window);
    ~Settings();

    void run();
    float getMusicVolume() const;
    float getGameMusicVolume() const;
    float getSFXVolume() const;
    void setMusicVolume(float volume);
    void setSFXVolume(float volume);
    void returnToMainMenu();


    int getAttempts() const { return attempts; }
    int getJumps() const { return jumps; }
    int getLevelsCompleted() const { return levelsCompleted; }
    int getCoins() const { return coinsCollected; }
    int getCustomCoins() const;

    void incrementAttempts();
    void incrementJumps();
    void incrementLevels();
    void addCoins(int amount);
    void addCustomCoins(int amount);
    void saveSettings();

private:

    sf::Texture logoTexture;
    sf::Sprite logo;

    void processEvents();
    void update();
    void render();
    void updateVolumeButtons();
    void loadSettings();

    void resetSettings();
    void resetIcons();

    sf::RenderWindow& window;
    sf::Texture backgroundTexture;
    sf::Sprite background;
    sf::Texture backTexture;
    sf::Sprite backButton;
    sf::Texture resetTexture;
    sf::Sprite resetButton;

    sf::Texture musicTexture;
    sf::Sprite musicButton;
    sf::Texture sfxTexture;
    sf::Sprite sfxButton;

    sf::Texture volumeTextures[6];
    sf::Sprite musicVolumeButtons[6];
    sf::Sprite sfxVolumeButtons[6];
    sf::Text volumeLabels[6];


    float musicVolume = 100.f;
    float sfxVolume = 100.f;

    sf::SoundBuffer hoverBuffer;
    sf::Sound hoverSound;
    bool wasHovering = false;

    void handleMouseHover();
    bool isMouseOver(const sf::Sprite& sprite) const;
    bool shouldExit = false;


    int attempts = 0;
    int jumps = 0;
    int levelsCompleted = 0;
    int coinsCollected = 0;
    int coinsCustom = 0;

    static const int HOVER_SOUND_POOL = 15;
    sf::Sound hoverSounds[HOVER_SOUND_POOL];
    int hoverSoundIndex = 0;

    int iconStates[9] = { 2,0,0,0,0,0,0,0,0 };
};