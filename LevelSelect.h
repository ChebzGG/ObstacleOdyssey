// LevelSelect.h
#include <SFML/Graphics.hpp>

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

    int selectedLevel = 0;
};