#include "Piston.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Piston::Piston(float x, float y) 
    : crankCenter(x, y), crankRadius(50.f), rodLength(150.f) // Aumenté biela a 150 para suavizar ángulo
{
    // --- CONFIGURACIÓN DE COLORES Y ESTILOS ---
    sf::Color steelColor(160, 160, 160);
    sf::Color darkSteel(100, 100, 100);
    sf::Color ironColor(70, 70, 80);      // Bloque motor oscuro
    sf::Color aluminumColor(200, 200, 200); // Pistón y Culata
    sf::Color valveColor(180, 180, 180);

    // 1. CIGÜEÑAL
    mainBearing.setRadius(15.f);
    mainBearing.setOrigin(15.f, 15.f);
    mainBearing.setPosition(crankCenter);
    mainBearing.setFillColor(sf::Color(40, 40, 40));

    crankArm.setSize(sf::Vector2f(crankRadius, 24.f)); // Un poco más robusto
    crankArm.setOrigin(0.f, 12.f);
    crankArm.setPosition(crankCenter);
    crankArm.setFillColor(darkSteel);

    crankPin.setRadius(10.f);
    crankPin.setOrigin(10.f, 10.f);
    crankPin.setFillColor(sf::Color(30, 30, 30));

    // 2. BIELA (Ahora más realista, más ancha abajo)
    pistonRod.setSize(sf::Vector2f(14.f, rodLength + 10.f)); 
    pistonRod.setOrigin(7.f, 0.f); 
    pistonRod.setFillColor(steelColor);

    // 3. PISTÓN
    // Ancho del pistón = 64. Haremos el cilindro de 66 para dejar holgura de 1px por lado.
    pistonHead.setSize(sf::Vector2f(64.f, 50.f));
    pistonHead.setOrigin(32.f, 25.f); 
    pistonHead.setFillColor(aluminumColor);
    // Anillos del pistón (detalle visual simple usando borde)
    pistonHead.setOutlineThickness(-2.f); // Borde interior
    pistonHead.setOutlineColor(sf::Color(50, 50, 50));

    wristPin.setRadius(7.f);
    wristPin.setOrigin(7.f, 7.f);
    wristPin.setFillColor(sf::Color(50, 50, 50));

    // --- CONSTRUCCIÓN DEL MOTOR (BLOQUE) ---
    
    // Calculamos alturas clave
    // TDC (Punto Muerto Superior) del centro del bulón: y - crank - rod
    float tdcY = y - crankRadius - rodLength; 
    // Altura de la tapa (Deck height): un poco más arriba del TDC del pistón
    float deckHeight = tdcY - 35.f; // Espacio para compresión

    // A. Bloque Izquierdo (Corte transversal)
    leftBlock.setPointCount(5);
    // 0. Esquina superior interior (borde del cilindro)
    leftBlock.setPoint(0, sf::Vector2f(x - 34.f, deckHeight)); 
    // 1. Abajo, fin del cilindro (comienzo del cárter)
    leftBlock.setPoint(1, sf::Vector2f(x - 34.f, y - 20.f)); 
    // 2. Ensanchamiento para el cigüeñal
    leftBlock.setPoint(2, sf::Vector2f(x - 80.f, y + 20.f)); 
    // 3. Fondo exterior
    leftBlock.setPoint(3, sf::Vector2f(x - 80.f, y + 80.f));
    // 4. Arriba exterior (cierre)
    leftBlock.setPoint(4, sf::Vector2f(x - 100.f, deckHeight));
    leftBlock.setFillColor(ironColor);
    leftBlock.setOutlineThickness(2.f);
    leftBlock.setOutlineColor(sf::Color::Black);

    // B. Bloque Derecho (Espejo del izquierdo)
    rightBlock.setPointCount(5);
    rightBlock.setPoint(0, sf::Vector2f(x + 34.f, deckHeight)); 
    rightBlock.setPoint(1, sf::Vector2f(x + 34.f, y - 20.f)); 
    rightBlock.setPoint(2, sf::Vector2f(x + 80.f, y + 20.f)); 
    rightBlock.setPoint(3, sf::Vector2f(x + 80.f, y + 80.f));
    rightBlock.setPoint(4, sf::Vector2f(x + 100.f, deckHeight));
    rightBlock.setFillColor(ironColor);
    rightBlock.setOutlineThickness(2.f);
    rightBlock.setOutlineColor(sf::Color::Black);

    // C. Culata (Cylinder Head)
    headBlock.setSize(sf::Vector2f(200.f, 60.f));
    headBlock.setOrigin(100.f, 60.f); // Pivote abajo al centro para colocar sobre el deck
    headBlock.setPosition(x, deckHeight);
    headBlock.setFillColor(aluminumColor);
    headBlock.setOutlineThickness(2.f);
    headBlock.setOutlineColor(sf::Color(50, 50, 50));

    // --- VÁLVULAS Y BUJÍA ---

    // Válvula Admisión (Izquierda)
    valveIntake.setSize(sf::Vector2f(8.f, 50.f));
    valveIntake.setOrigin(4.f, 0.f);
    // Posicionada en la culata, desplazada a la izquierda
    valveIntake.setPosition(x - 25.f, deckHeight - 55.f); 
    valveIntake.setFillColor(valveColor);

    // Válvula Escape (Derecha)
    valveExhaust.setSize(sf::Vector2f(8.f, 50.f));
    valveExhaust.setOrigin(4.f, 0.f);
    valveExhaust.setPosition(x + 25.f, deckHeight - 55.f);
    valveExhaust.setFillColor(valveColor);

    // Bujía (Centro)
    sparkPlugBody.setSize(sf::Vector2f(12.f, 30.f));
    sparkPlugBody.setOrigin(6.f, 15.f);
    sparkPlugBody.setPosition(x, deckHeight - 60.f); // Arriba de la culata
    sparkPlugBody.setFillColor(sf::Color::White); // Cerámica

    sparkPlugTip.setSize(sf::Vector2f(4.f, 15.f)); // El electrodo que entra
    sparkPlugTip.setOrigin(2.f, 0.f);
    sparkPlugTip.setPosition(x, deckHeight - 45.f);
    sparkPlugTip.setFillColor(sf::Color(30, 30, 30));
}

void Piston::update(float angle) {
    // 1. Cinemática
    float crankX = crankCenter.x + crankRadius * std::cos(angle);
    float crankY = crankCenter.y + crankRadius * std::sin(angle);
    sf::Vector2f crankPos(crankX, crankY);

    float dx = crankX - crankCenter.x;
    float diffX = std::abs(dx);
    
    // Pitágoras para altura del pistón
    // Si la biela es muy corta, esto podría dar NaN, pero con 150 vs 50 es seguro.
    float rodVerticalH = std::sqrt(rodLength * rodLength - diffX * diffX);
    sf::Vector2f pistonPos(crankCenter.x, crankY - rodVerticalH);

    // 2. Actualizar posiciones visuales
    crankPin.setPosition(crankPos);

    sf::Vector2f delta = crankPos - crankCenter;
    float armRotation = std::atan2(delta.y, delta.x) * 180.f / M_PI;
    crankArm.setPosition(crankCenter);
    crankArm.setRotation(armRotation); 

    pistonHead.setPosition(pistonPos);
    wristPin.setPosition(pistonPos);

    // 3. Rotación de la biela
    pistonRod.setPosition(pistonPos); 
    sf::Vector2f rodDelta = crankPos - pistonPos; 
    float rodAngle = std::atan2(rodDelta.y, rodDelta.x) * 180.f / M_PI;
    pistonRod.setRotation(rodAngle - 90.f); 
}

void Piston::draw(sf::RenderWindow& window) {
    // Orden de dibujo (capas):
    // 1. Partes traseras del bloque
    window.draw(sparkPlugTip); // Punta dentro de la cámara
    window.draw(valveIntake);
    window.draw(valveExhaust);
    
    // 2. El bloque motor (tapa las partes superiores de las válvulas si quisieras animarlas)
    window.draw(leftBlock);
    window.draw(rightBlock);
    window.draw(headBlock);
    window.draw(sparkPlugBody);

    // 3. Partes móviles internas
    window.draw(pistonRod);     // Biela detrás
    window.draw(pistonHead);    // Pistón
    window.draw(wristPin);      // Bulón
    
    // 4. Cigüeñal (al frente)
    window.draw(crankArm);
    window.draw(mainBearing);
    window.draw(crankPin);      
}