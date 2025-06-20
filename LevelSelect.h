// LevelSelect.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class LevelSelect {
public:
    LevelSelect(sf::RenderWindow& window);
    void run();
    int getSelectedLevel() const;

private:
    void processEvents();
    void render();
    void update();
    bool isMouseOver(const sf::Sprite& sprite);

    sf::RenderWindow& window;
    sf::Texture backgroundTexture;
    sf::Sprite background;

    sf::Texture backTexture;
    sf::Sprite backButton;

    sf::Texture levelTextures[5];
    sf::Sprite levelButtons[5];

    sf::Texture lvlTexture;
    sf::Sprite lvl;

    sf::SoundBuffer hoverBuffer;
    sf::Sound hoverSound;
    bool wasHovering = false;

    int selectedLevel = 0;

    static const int HOVER_SOUND_POOL = 15;
    sf::Sound hoverSounds[HOVER_SOUND_POOL];
    int hoverSoundIndex = 0;
};