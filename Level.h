#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

class Level {
public:
    Level(const std::string& levelFile, const std::string& musicFile);
    ~Level();

    void update();
    void render(sf::RenderWindow& window);

    void restart();
    bool isFailed() const;
    bool isCompleted() const;
    void setPaused(bool paused);
    void playMusic();
    void pauseMusic();
    void stopMusic();
    void setMusicVolume(float volume);


    int getJumpCount() const;

private:
    static const int SIZE_X = 1000;
    static const int SIZE_Y = 10;
    char level[SIZE_X][SIZE_Y];

    static const int GROUND_COUNT = 13;
    sf::Texture groundSquareTexture;
    sf::Sprite ground[GROUND_COUNT];

    sf::RectangleShape player;
    float playerY, playerVelocityY;
    bool isOnGround, isJumping, failed, completed, paused;
    int jumpCount;
    float levelPos;
    float playerRotation = 0.0f;
    int endPos;
    void drawGroundSquare(sf::RenderWindow& window, float x, float y, float size);

    sf::RectangleShape spike, spike2, shortSpike, endWall;
    sf::Texture spikeTexture, spike2Texture;




    sf::Texture endWallTexture;


    sf::Texture blockTexture0, blockTexture1, blockTexture2, blockTexture3, blockTexture4, blockTexture5, 
        blockTexture6, blockTexture7, blockTexture8, blockTexture9, blockTexture10, 
        blockTexture11, blockTexture12, blockTexture13, blockTexture14, blockTexture15;

    sf::RectangleShape block0, block1, block2, block3, block4, block5, 
        block6, block7, block8, block9, block10, block11, 
        block12, block13, block14, block15;;

    sf::Texture shortSpikeTexture;
    sf::Texture iconTexture;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Texture coinTexture;
    sf::SoundBuffer coinBuffer;
    sf::Sound coinSound;
    sf::RectangleShape coinShape;
    bool coinCollected[SIZE_X][SIZE_Y] = { {false} };

    sf::SoundBuffer deathBuffer;
    sf::Sound deathSound;

    sf::SoundBuffer winBuffer;
    sf::Sound winSound;

    sf::Music music;

    void loadLevel(const std::string& filename);
    void setEnd();
    void resetPlayer();
    void handleInput();
    void handlePhysics();
    void handleCollisions();
    void drawObstacles(sf::RenderWindow& window);
    bool checkCollision(const sf::FloatRect& a, const sf::FloatRect& b) const;

};