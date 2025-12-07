#pragma once
#include <SFML/Graphics.hpp>

class Piston {
private:
    // Geometría del motor
    float crankRadius; // Radio del cigüeñal
    float rodLength;   // Largo de la biela
    sf::Vector2f crankCenter;

    // --- Partes MÓVILES ---
    sf::RectangleShape pistonHead; // Cabeza del pistón
    sf::RectangleShape pistonRod;  // Biela
    sf::RectangleShape crankArm;   // Brazo cigüeñal
    sf::CircleShape crankPin;      // Muñequilla
    sf::CircleShape wristPin;      // Bulón
    sf::CircleShape mainBearing;   // Eje central
    
    // --- Partes ESTÁTICAS (Motor) ---
    // Usaremos ConvexShape para formas complejas (el bloque cortado)
    sf::ConvexShape leftBlock;     // Pared izquierda + Cárter
    sf::ConvexShape rightBlock;    // Pared derecha + Cárter
    sf::RectangleShape headBlock;  // La culata (tapa)

    // --- Detalles (Válvulas y Bujía) ---
    sf::RectangleShape valveIntake;
    sf::RectangleShape valveExhaust;
    sf::RectangleShape sparkPlugBody;
    sf::RectangleShape sparkPlugTip;

public:
    Piston(float x, float y);
    void update(float angle);
    void draw(sf::RenderWindow& window);
};