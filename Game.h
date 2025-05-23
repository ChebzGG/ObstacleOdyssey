// Game.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Menu.h"
#include "Level.h"
#include "LevelSelect.h"
#include "Settings.h"

class Game {
public:
    Game();
    ~Game();
    void run();
    void showStatistics();
    void returnToMainMenu();
    void setMusicVolume(float volume);
    static Game* getInstance();

private:
    void processEvents();
    void update();
    void render();
    void showPauseMenu();
    void handleMenuState();
    void handleGameState();
    void handleLevelSelection();
    void startLevel();
    void showGameOverMenu();
    void handleSettings();
    void returnToSettings(); // Новый метод для возврата в настройки

    Settings* settings;
    sf::RenderWindow window;
    sf::Music menuMusic;

    Menu menu;
    Level* levels[5];
    int currentLevel = -1;
    bool isPaused = false;
    bool inMenu = true;

    sf::Texture pauseBackgroundTexture;
    sf::Sprite pauseBackground;
    sf::Texture resumeButtonTexture;
    sf::Sprite resumeButton;
    sf::Texture exitButtonTexture;
    sf::Sprite exitButton;

    sf::Clock levelClock;

    void handleMouseHover();
    bool isMouseOver(const sf::Sprite& sprite) const;
    static Game* instance; // Объявление
};