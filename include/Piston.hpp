#pragma once
#include <SFML/Graphics.hpp>
#include <string>

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
    
    // Cámara de combustión
    sf::RectangleShape gasChamber;

    // Colores
    sf::Color colorFuel;
    sf::Color colorComp;
    sf::Color colorFire;
    sf::Color colorExhaust;

public:
    Piston(float x, float y);
    void update(float angle);
    void draw(sf::RenderWindow& window);

    // --- NUEVOS MÉTODOS PARA QoS ---
    // Devuelve el nombre de la fase (Admisión, etc.) para el HUD
    std::string getCyclePhaseName(float angle) const;
    
    // Devuelve la posición de la salida de escape para generar partículas
    sf::Vector2f getExhaustPortPosition() const;
    
    // Devuelve true si estamos en fase de escape (para activar el emisor de humo)
    bool isExhaustPhase(float angle) const;
};