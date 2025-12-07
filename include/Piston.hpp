#pragma once
#include <SFML/Graphics.hpp>

class Piston {
private:
    // Geometría del motor
    float crankRadius;
    float rodLength;
    sf::Vector2f crankCenter;

    // --- Partes MÓVILES ---
    sf::RectangleShape pistonHead;
    sf::RectangleShape pistonRod;
    sf::RectangleShape crankArm;
    sf::CircleShape crankPin;
    sf::CircleShape wristPin;
    sf::CircleShape mainBearing;
    
    // --- Partes ESTÁTICAS (Motor) ---
    sf::ConvexShape leftBlock;
    sf::ConvexShape rightBlock;
    sf::RectangleShape headBlock;

    // --- Detalles Funcionales ---
    sf::RectangleShape valveIntake;
    sf::RectangleShape valveExhaust;
    sf::RectangleShape sparkPlugBody;
    sf::RectangleShape sparkPlugTip;
    
    // NUEVO: La cámara de combustión (el gas de colores)
    sf::RectangleShape gasChamber;

    // Colores base para reutilizar
    sf::Color colorFuel;    // Azul claro (mezcla)
    sf::Color colorComp;    // Azul oscuro (comprimido)
    sf::Color colorFire;    // Amarillo/Blanco (explosión)
    sf::Color colorExhaust; // Gris (humo)

public:
    Piston(float x, float y);
    void update(float angle);
    void draw(sf::RenderWindow& window);
};