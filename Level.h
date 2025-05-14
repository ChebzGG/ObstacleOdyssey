#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

const int TILE_SIZE = 100;
const int MAX_GRID_WIDTH = 100;
const int MAX_GRID_HEIGHT = 50;

class Level {
public:
    Level(const char* mapPath, const char* musicPath);
    ~Level();

    void load();
    void update();
    void render(sf::RenderWindow& window);

    void playMusic();
    void stopMusic();
    void restart();

    bool isCompleted() const;
    bool isFailed() const;
    int getCoinsCollected() const;

    void setPlayerTexture(sf::Texture* texture);

private:
    int grid[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];
    bool coinCollected[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];

    sf::Music music;
    bool completed = false;
    bool failed = false;
    const char* filePath;

    sf::RectangleShape player;
    sf::Vector2f velocity;
    bool onGround = false;
    bool gravityInverted = false;

    sf::Vector2i startPosition;
    sf::Vector2i finishPosition;

    int coinsCollected = 0;

    sf::Texture tileTextures[10];
    sf::Sprite tileSprite;

    sf::SoundBuffer jumpBuffer;
    sf::Sound jumpSound;
    sf::SoundBuffer coinBuffer;
    sf::Sound coinSound;
    sf::SoundBuffer deathBuffer;
    sf::Sound deathSound;

    void initializeGrid();
    void loadFromFile();
    void handleCollisions();
    void loadTextures();
    void loadSounds();
};