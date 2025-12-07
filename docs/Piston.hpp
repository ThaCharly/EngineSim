// Piston.hpp
#pragma once
#include <SFML/Graphics.hpp>

class Piston {
private:
    sf::RectangleShape head;    // Cabeza ancha del pistón
    sf::RectangleShape rod;     // Vástago delgado
    sf::CircleShape joint;      // Articulación redonda (donde iría la biela)

    float baseY;
    float amplitude;
    float x;

public:
    Piston(float x, float y);
    void update(float angle);
    void draw(sf::RenderWindow& window);
};