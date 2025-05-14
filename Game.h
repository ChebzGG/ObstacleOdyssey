#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Menu.h"
#include "Level.h"
#include "LevelSelect.h"
#include "Statistics.h"

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void showPauseMenu();
    void restartLevel();
    void completeLevel();
    void loadPlayerTexture();
    void returnToMainMenu();
    void startLevel();
    void showLevelResults(float time, int coins);
    void handleMenuState();
    void handleGameState();
    void handleLevelSelection();
    void showStatistics();

    sf::RenderWindow window;
    sf::Music menuMusic;
    sf::Texture playerTexture;
    std::string currentSkin = "assets/images/icon0.png";

    Menu menu;
    Level* levels[5];
    int currentLevel = -1;
    bool isPaused = false;
    bool inMenu = true;

    sf::Clock levelClock;
    int levelAttempts[5] = { 0 };
    int levelCoins[5] = { 0 };
    float levelBestTimes[5] = { 0.0f };
    int totalCoins = 0;
};