#include "Custom.h"
#include "Game.h"
using namespace std;
using namespace sf;

Custom::Custom(RenderWindow& win) : window(win) {
    backTexture.loadFromFile("assets/images/back.png");
    font.loadFromFile("assets/fonts/arial.ttf");
    hoverBuffer.loadFromFile("assets/sounds/hover.ogg");
    backgroundTexture.loadFromFile("assets/images/BGcustomisation.png");
    logoTexture.loadFromFile("assets/images/customisation.png");

    logoSprite.setTexture(logoTexture);
    logoSprite.setPosition(280.f, 50.f);
    logoSprite.setScale(1.0f, 1.0f);

    backSprite.setTexture(backTexture);
    backSprite.setPosition(20.f, 20.f);
    backSprite.setScale(0.18f, 0.18f);

    backgroundSprite.setTexture(backgroundTexture);

    hoverSound.setBuffer(hoverBuffer);

    for (int i = 0; i < 10; ++i) {
        digitTextures[i].loadFromFile("assets/images/logo - " + to_string(i) + ".png");
        digitSprites[i].setTexture(digitTextures[i]);
    }

    for (int i = 0; i < 9; ++i) {
        iconTextures[i].loadFromFile("assets/images/icon" + to_string(i) + ".png");
        iconSprites[i].setTexture(iconTextures[i]);
        iconSprites[i].setScale(1.5f, 1.5f);
    }

    buyTexture.loadFromFile("assets/images/cbuy.png");
    selectTexture.loadFromFile("assets/images/cselect.png");
    selectedTexture.loadFromFile("assets/images/cselected.png");

    for (int i = 0; i < 9; ++i) {
        buySprites[i].setTexture(buyTexture);
        selectSprites[i].setTexture(selectTexture);
        selectedSprites[i].setTexture(selectedTexture);

        buySprites[i].setScale(0.4f, 0.4f);
        selectSprites[i].setScale(0.4f, 0.4f);
        selectedSprites[i].setScale(0.4f, 0.4f);
    }

    buyBuffer.loadFromFile("assets/sounds/buy.ogg");
    buySound.setBuffer(buyBuffer);

    loadIconStates();
}

Custom::~Custom() {}

bool Custom::isMouseOver(const Sprite& sprite) {
    FloatRect bounds = sprite.getGlobalBounds();
    return bounds.contains(Vector2f(Mouse::getPosition(window)));
}

int Custom::getCustomCoins() const {
    return Game::getInstance()->getSettings()->getCustomCoins();
}

void Custom::render() {
    window.clear();
    window.draw(backgroundSprite);
    window.draw(logoSprite);
    window.draw(backSprite);

    string coinsStr = to_string(getCustomCoins());
    float x = 1500.f;
    float y = 740.f; 
    float digitSpacing = 60.f;

    for (char ch : coinsStr) {
        if (ch >= '0' && ch <= '9') {
            int digit = ch - '0';
            digitSprites[digit].setPosition(x, y);
            digitSprites[digit].setScale(0.5f, 0.5f);
            window.draw(digitSprites[digit]);
            x += digitSpacing;
        }
    }

    float shelf1_y = 420.f; 
    float shelf2_y = 725.f;
    float shelf1_x = 320.f;
    float shelf2_x = 200.f; 
    float shelf1_spacing = 280.f;
    float shelf2_spacing = 280.f; 

    for (int i = 0; i < 5; ++i) {
        iconSprites[i].setPosition(shelf1_x + i * shelf1_spacing, shelf1_y);
        window.draw(iconSprites[i]);
    }
    for (int i = 0; i < 4; ++i) {
        iconSprites[i + 5].setPosition(shelf2_x + i * shelf2_spacing, shelf2_y);
        window.draw(iconSprites[i + 5]);
    }

    for (int i = 0; i < 9; ++i) {
        window.draw(iconSprites[i]);

        string priceStr = to_string(iconPrices[i]);
        float px = iconSprites[i].getPosition().x + 20;
        float py = iconSprites[i].getPosition().y - 60;
        for (char ch : priceStr) {
            int d = ch - '0';
            digitSprites[d].setPosition(px, py);
            digitSprites[d].setScale(0.4f, 0.4f);
            window.draw(digitSprites[d]);
            px += 40;
        }

        float bx = iconSprites[i].getPosition().x + 10; 
        float by = iconSprites[i].getPosition().y + 110;
        if (iconStates[i] == 0) {
            buySprites[i].setPosition(bx, by);
            window.draw(buySprites[i]);
        }
        else if (iconStates[i] == 1) {
            selectSprites[i].setPosition(bx, by);
            window.draw(selectSprites[i]);
        }
        else if (iconStates[i] == 2) {
            selectedSprites[i].setPosition(bx, by);
            window.draw(selectedSprites[i]);
        }
    }

    window.display();
}

void Custom::run() {
    shouldExit = false;
    while (window.isOpen() && !shouldExit) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });

                if (backSprite.getGlobalBounds().contains(mousePos)) {
                    shouldExit = true;
                }

                for (int i = 0; i < 9; ++i) {
                    if (iconStates[i] == 0 && buySprites[i].getGlobalBounds().contains(mousePos)) {
                        if (getCustomCoins() >= iconPrices[i]) {
                            Game::getInstance()->getSettings()->addCustomCoins(-iconPrices[i]);
                            iconStates[i] = 1;
                            saveIconStates();
                            float sfxVolume = 100.f;
                            if (Game::getInstance() && Game::getInstance()->getSettings()) {
                                sfxVolume = Game::getInstance()->getSettings()->getSFXVolume();
                            }
                            buySound.setVolume(sfxVolume);
                            buySound.play();
                        }
                    }
                    if (iconStates[i] == 1 && selectSprites[i].getGlobalBounds().contains(mousePos)) {
                        for (int j = 0; j < 9; ++j)
                            if (iconStates[j] == 2) iconStates[j] = 1;
                        iconStates[i] = 2;
                        selectedIcon = i;
                        saveIconStates();
                    }
                }
            }
        }
        bool isHoveringBackNow = isMouseOver(backSprite);
        if (isHoveringBackNow) {
            backSprite.setColor(Color(255, 200, 255, 255));
            if (!wasHoveringBack) {
                float sfxVolume = 100.f;
                if (Game::getInstance() && Game::getInstance()->getSettings()) {
                    sfxVolume = Game::getInstance()->getSettings()->getSFXVolume();
                }
                hoverSound.setVolume(sfxVolume);
                hoverSound.play();
            }
        }
        else {
            backSprite.setColor(Color::White);
        }
        wasHoveringBack = isHoveringBackNow;

        handleMouseHover();
        render();
    }
}

void Custom::loadIconStates() {
    ifstream file("icon.txt");
    if (file.is_open()) {
        for (int i = 0; i < 9; ++i) file >> iconStates[i];
        file.close();
    }
    for (int i = 0; i < 9; ++i)
        if (iconStates[i] == 2) selectedIcon = i;
}

void Custom::saveIconStates() {
    ofstream file("icon.txt");
    if (file.is_open()) {
        for (int i = 0; i < 9; ++i) file << iconStates[i] << " ";
        file.close();
    }
}

void Custom::handleMouseHover() {
    for (int i = 0; i < 9; ++i) {
        Sprite* btn = nullptr;
        if (iconStates[i] == 0) btn = &buySprites[i];
        else if (iconStates[i] == 1) btn = &selectSprites[i];
        else if (iconStates[i] == 2) btn = &selectedSprites[i];
        if (!btn) continue;

        bool isHovering = isMouseOver(*btn);
        btn->setColor(isHovering ? Color(255, 200, 255, 255) : Color::White);

        if (isHovering && !wasHoveringButton[i]) {
            float sfxVolume = 100.f;
            if (Game::getInstance() && Game::getInstance()->getSettings()) {
                sfxVolume = Game::getInstance()->getSettings()->getSFXVolume();
            }
            hoverSound.setVolume(sfxVolume);
            hoverSound.play();
        }
        wasHoveringButton[i] = isHovering;
    }
}

