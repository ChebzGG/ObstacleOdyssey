// Statistics.cpp
#include "Statistics.h"

Statistics::Statistics() {
    for (int i = 0; i < MAX_LEVELS; ++i) {
        levelScores[i] = 0; // Ручная инициализация
    }
}

void Statistics::recordAttempt() { attempts++; }
void Statistics::recordJump() { jumps++; }

void Statistics::display(sf::RenderWindow& window) const {
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arial.ttf")) return;

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    // Отображение статистики
    text.setString("Attempts: " + std::to_string(attempts));
    window.draw(text);

    text.move(0, 30);
    text.setString("Jumps: " + std::to_string(jumps));
    window.draw(text);
}