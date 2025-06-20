#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

class Custom {
public:
    Custom(sf::RenderWindow& window);
    ~Custom();

    void run();
    void render();
    int getCustomCoins() const;
    void loadIconStates();
    void saveIconStates();

private:
    sf::RenderWindow& window;
    sf::Texture backTexture;
    sf::Font font;
    sf::SoundBuffer hoverBuffer;
    sf::Texture backgroundTexture;
    sf::Texture logoTexture;

    sf::Sprite backSprite;
    sf::Sprite backgroundSprite;
    sf::Sprite logoSprite;

    sf::Sound hoverSound;
    bool wasHoveringBack = false;
    bool wasHoveringButton[9];

    void handleMouseHover();
    bool isMouseOver(const sf::Sprite& sprite);
    bool shouldExit = false;
    int coinsCustom = 0;

    sf::Texture digitTextures[10];
    sf::Sprite digitSprites[10];

    sf::Texture iconTextures[9];
    sf::Sprite iconSprites[9];

    sf::Texture buyTexture, selectTexture, selectedTexture;
    sf::Sprite buySprites[9], selectSprites[9], selectedSprites[9];

    sf::SoundBuffer buyBuffer;
    sf::Sound buySound;


    int iconStates[9] = { 2,0,0,0,0,0,0,0,0 };
    int iconPrices[9] = { 0, 10, 20, 45, 70, 100, 130, 160, 200 };
    int selectedIcon = 0;
};