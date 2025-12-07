#pragma once
#include <SFML/Graphics.hpp>

class Piston {
private:
    // Geometría del motor
    float crankRadius; // Radio del cigüeñal (r)
    float rodLength;   // Largo de la biela (l)
    sf::Vector2f crankCenter; // Donde gira el cigüeñal en la pantalla

    // Partes visuales
    sf::RectangleShape pistonHead; // La cabeza del pistón
    sf::RectangleShape pistonRod;  // La biela
    sf::RectangleShape crankArm;   // El brazo del cigüeñal
    sf::CircleShape crankPin;      // Unión biela-cigüeñal
    sf::CircleShape wristPin;      // Unión biela-pistón
    sf::CircleShape mainBearing;   // El centro del cigüeñal (estático)
    
    // Decoración (paredes del cilindro)
    sf::VertexArray cylinderWalls;

public:
    Piston(float x, float y); // x,y será el centro del cigüeñal ahora
    void update(float angle);
    void draw(sf::RenderWindow& window);
};