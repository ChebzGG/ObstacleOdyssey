// Statistics.h
#pragma once
#include <SFML/Graphics.hpp>

class Statistics {
public:
    Statistics();
    void recordAttempt();
    void recordJump();
    void display(sf::RenderWindow& window) const;

private:
    int attempts = 0;
    int jumps = 0;
    int completedLevels = 0;
    static const int MAX_LEVELS = 5;
    int levelScores[MAX_LEVELS]; // Статический массив для рекордов
};