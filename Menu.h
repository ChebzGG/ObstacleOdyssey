// Menu.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Menu {
public:
    Menu(sf::RenderWindow& window);
    void run();
    int getMenuSelection() const;
    int getChosenLevel() const;

private:
    void processEvents();
    void update();
    void render();
    void handleMouseHover();
    bool isMouseOver(const sf::Sprite& sprite);
    void handleMenuSelection();

    sf::RenderWindow& window;
    sf::Texture backgroundTexture;
    sf::Sprite background;

    sf::Texture logoTexture;
    sf::Sprite logo;

    sf::Texture playTexture;
    sf::Sprite playButton;

    sf::Texture customTexture;
    sf::Sprite customButton;

    sf::Texture settingsTexture;
    sf::Sprite settingsButton;

    sf::Texture exitTexture;
    sf::Sprite exitButton;

    sf::Texture statsTexture;
    sf::Sprite statsButton;

    int selectedOption = -1;
    int chosenLevel = -1;

    sf::SoundBuffer hoverBuffer;
    sf::Sound hoverSound;
    bool wasHovering = false;
};