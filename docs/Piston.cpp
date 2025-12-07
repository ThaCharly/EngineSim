// Piston.cpp
#include "Piston.hpp"
#include <cmath>

Piston::Piston(float x, float y) : x(x), baseY(y), amplitude(100.f) {
    // === Cabeza del pistón: ancha, corta, metálica ===
    head.setSize(sf::Vector2f(80.f, 70.f));
    head.setFillColor(sf::Color(180, 180, 180));
    head.setOutlineThickness(1.f);
    head.setOutlineColor(sf::Color(60, 60, 60));
    head.setOrigin(39.f, 36.f); // centrado

    // === Vástago: delgado, largo, conectado al centro inferior de la cabeza ===
    rod.setSize(sf::Vector2f(8.f, 70.f));
    rod.setFillColor(sf::Color(100, 100, 100));
    rod.setOutlineThickness(1.f);
    rod.setOutlineColor(sf::Color(50, 50, 50));
    rod.setOrigin(4.f, 0.f); // origen en la parte superior

    // === Articulación: círculo en la base del vástago ===
    joint.setRadius(18.f);
    joint.setFillColor(sf::Color(70, 70, 70));
    joint.setOutlineThickness(1.f);
    joint.setOutlineColor(sf::Color(30, 30, 30));
    joint.setOrigin(18.f, 12.f); // centrado
}

void Piston::update(float angle) {
    float offset = std::sin(angle) * amplitude;
    float headY = baseY + offset;

    // Posicionar cabeza
    head.setPosition(x, headY);

    // Vástago cuelga desde el centro inferior de la cabeza
    rod.setPosition(x, headY + head.getSize().y / 2.f);

    // Articulación en la punta inferior del vástago
    joint.setPosition(x, rod.getPosition().y + rod.getSize().y);
}

void Piston::draw(sf::RenderWindow& window) {
    window.draw(head);
    window.draw(rod);
    window.draw(joint);
}