//Level.h
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

const int SIZE_X = 1000;
const int SIZE_Y = 10;

class Level {
public:
    Level(const std::string& levelFile, const std::string& musicFile);
    ~Level();

    void load();
    void update();
    void render(sf::RenderWindow& window);
    void restart();
    void playMusic();
    void stopMusic();
    bool isCompleted() const;
    bool isFailed() const;
    int getCoinsCollected() const;
    int getJumpCount() const;
    void pauseMusic(); // Добавляем этот метод

    float getMusicVolume() const { return music.getVolume(); }
    void setMusicVolume(float volume) { if (musicLoaded) music.setVolume(volume); }
private:
    void setLevel();
    void setEndPosition();
    void handleInput();
    void handlePhysics();
    void checkCollisions();
    void resetPlayer();

    char level[SIZE_X][SIZE_Y]{};
    int endPos = 0;
    int coinsCollected = 0;

    sf::Texture spikeTex, blockTex, shortSpikeTex, endwallTex, iconTex, jumpPadTex, orbTex, coinTex;
    sf::Sprite spike, block, shortSpike, endwall, icon, jumpPad, orb, coin;
    sf::RectangleShape iconHitbox, iconBlockHitbox;
    sf::RectangleShape groundLine;

    sf::Music music;
    sf::SoundBuffer deathBuffer, coinBuffer, jumpBuffer;
    sf::Sound death, coinSound, jumpSound;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    float levelSpeed = 9.6f;
    float levelPos = 0;
    float jumpCount = 0;
    float jumpHeight = 0;

    int iconDefaultY;
    int iconGroundDefaultY;
    bool iconAlive = true;
    bool isGrounded = true;
    bool jumpCheck = false;
    bool fall = false;
    bool completed = false;
    bool failed = false;

    std::string levelPath;
    std::string musicPath;

    bool musicLoaded = false; // Флаг для проверки загрузки музыки
};