// Stats.h
#pragma once
#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Settings.h"

class Stats {
public:
    Stats(sf::RenderWindow& window);
    void run();

private:

    sf::Texture logoTexture;
    sf::Sprite logo;

    void processEvents();
    void render();
    bool isMouseOver(const sf::Sprite& sprite);

    bool shouldExit = false; 
    sf::RenderWindow& window;
    sf::Texture backgroundTexture;
    sf::Sprite background;
    sf::Texture backTexture;
    sf::Sprite backButton;
    sf::Font font;

    sf::SoundBuffer hoverBuffer;
    sf::Sound hoverSound;
    bool wasHovering = false;

    static const int HOVER_SOUND_POOL = 15;
    sf::Sound hoverSounds[HOVER_SOUND_POOL];
    int hoverSoundIndex = 0;

    sf::Texture digitTextures[10];
    sf::Sprite digitSprites[10];
};