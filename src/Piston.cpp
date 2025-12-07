#include "Piston.hpp"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Piston::Piston(float x, float y) 
    : crankCenter(x, y), crankRadius(50.f), rodLength(150.f) 
{
    // --- COLORES ---
    sf::Color steelColor(160, 160, 160);
    sf::Color darkSteel(100, 100, 100);
    sf::Color ironColor(70, 70, 80);
    sf::Color aluminumColor(200, 200, 200);
    sf::Color valveColor(180, 180, 180);

    colorFuel = sf::Color(100, 200, 255, 150);
    colorComp = sf::Color(50, 100, 255, 200);
    colorFire = sf::Color(255, 220, 0, 240); // Amarillo más cálido
    colorExhaust = sf::Color(100, 100, 100, 180);

    // 1. CIGÜEÑAL
    mainBearing.setRadius(15.f);
    mainBearing.setOrigin(15.f, 15.f);
    mainBearing.setPosition(crankCenter);
    mainBearing.setFillColor(sf::Color(30, 30, 30));

    crankArm.setSize(sf::Vector2f(crankRadius, 24.f));
    crankArm.setOrigin(0.f, 12.f);
    crankArm.setPosition(crankCenter);
    crankArm.setFillColor(darkSteel);

    crankPin.setRadius(10.f);
    crankPin.setOrigin(10.f, 10.f);
    crankPin.setFillColor(sf::Color(20, 20, 20));

    // 2. BIELA
    pistonRod.setSize(sf::Vector2f(14.f, rodLength + 10.f)); 
    pistonRod.setOrigin(7.f, 0.f); 
    pistonRod.setFillColor(steelColor);

    // 3. PISTÓN
    pistonHead.setSize(sf::Vector2f(64.f, 50.f));
    pistonHead.setOrigin(32.f, 25.f); 
    pistonHead.setFillColor(aluminumColor);
    pistonHead.setOutlineThickness(-2.f);
    pistonHead.setOutlineColor(sf::Color(50, 50, 50));

    wristPin.setRadius(7.f);
    wristPin.setOrigin(7.f, 7.f);
    wristPin.setFillColor(sf::Color(50, 50, 50));

    // --- BLOQUE MOTOR ---
    float tdcY = y - crankRadius - rodLength; 
    float deckHeight = tdcY - 35.f; 

    // Bloque Izquierdo
    leftBlock.setPointCount(5);
    leftBlock.setPoint(0, sf::Vector2f(x - 34.f, deckHeight)); 
    leftBlock.setPoint(1, sf::Vector2f(x - 34.f, y - 20.f)); 
    leftBlock.setPoint(2, sf::Vector2f(x - 80.f, y + 20.f)); 
    leftBlock.setPoint(3, sf::Vector2f(x - 80.f, y + 80.f));
    leftBlock.setPoint(4, sf::Vector2f(x - 100.f, deckHeight));
    leftBlock.setFillColor(ironColor);
    leftBlock.setOutlineThickness(2.f);
    leftBlock.setOutlineColor(sf::Color::Black);

    // Bloque Derecho
    rightBlock.setPointCount(5);
    rightBlock.setPoint(0, sf::Vector2f(x + 34.f, deckHeight)); 
    rightBlock.setPoint(1, sf::Vector2f(x + 34.f, y - 20.f)); 
    rightBlock.setPoint(2, sf::Vector2f(x + 80.f, y + 20.f)); 
    rightBlock.setPoint(3, sf::Vector2f(x + 80.f, y + 80.f));
    rightBlock.setPoint(4, sf::Vector2f(x + 100.f, deckHeight));
    rightBlock.setFillColor(ironColor);
    rightBlock.setOutlineThickness(2.f);
    rightBlock.setOutlineColor(sf::Color::Black);

    // Culata
    headBlock.setSize(sf::Vector2f(200.f, 60.f));
    headBlock.setOrigin(100.f, 60.f); 
    headBlock.setPosition(x, deckHeight);
    headBlock.setFillColor(aluminumColor);
    headBlock.setOutlineThickness(2.f);
    headBlock.setOutlineColor(sf::Color(50, 50, 50));

    // Gas Chamber
    gasChamber.setPosition(x - 32.f, deckHeight);
    gasChamber.setSize(sf::Vector2f(64.f, 0.f)); 
    gasChamber.setFillColor(sf::Color::Transparent);

    // Válvulas
    float valveBaseY = deckHeight - 45.f;
    
    valveIntake.setSize(sf::Vector2f(8.f, 50.f));
    valveIntake.setOrigin(4.f, 0.f);
    valveIntake.setPosition(x - 20.f, valveBaseY); 
    valveIntake.setFillColor(valveColor);

    valveExhaust.setSize(sf::Vector2f(8.f, 50.f));
    valveExhaust.setOrigin(4.f, 0.f);
    valveExhaust.setPosition(x + 20.f, valveBaseY);
    valveExhaust.setFillColor(valveColor);

    sparkPlugBody.setSize(sf::Vector2f(12.f, 30.f));
    sparkPlugBody.setOrigin(6.f, 15.f);
    sparkPlugBody.setPosition(x, deckHeight - 60.f); 
    sparkPlugBody.setFillColor(sf::Color::White); 

    sparkPlugTip.setSize(sf::Vector2f(4.f, 15.f)); 
    sparkPlugTip.setOrigin(2.f, 0.f);
    sparkPlugTip.setPosition(x, deckHeight - 45.f);
    sparkPlugTip.setFillColor(sf::Color(30, 30, 30));
}

// --- LOGICA AUXILIAR ---
float getCyclePhaseInternal(float angle) {
    float phaseOffset = angle + (M_PI / 2.0);
    float cyclePhase = std::fmod(phaseOffset, 4.0 * M_PI);
    if (cyclePhase < 0) cyclePhase += 4.0 * M_PI;
    return cyclePhase;
}

std::string Piston::getCyclePhaseName(float angle) const {
    float p = getCyclePhaseInternal(angle);
    if (p < M_PI) return "ADMISION";
    if (p < 2.0 * M_PI) return "COMPRESION";
    if (p < 3.0 * M_PI) return "EXPLOSION";
    return "ESCAPE";
}

sf::Vector2f Piston::getExhaustPortPosition() const {
    // La posición de salida está un poco arriba de la válvula de escape
    sf::Vector2f pos = valveExhaust.getPosition();
    return sf::Vector2f(pos.x + 10.f, pos.y - 10.f); // Ajuste manual
}

bool Piston::isExhaustPhase(float angle) const {
    float p = getCyclePhaseInternal(angle);
    return (p >= 3.0 * M_PI);
}

void Piston::update(float angle) {
    // 1. Cinemática
    float crankX = crankCenter.x + crankRadius * std::cos(angle);
    float crankY = crankCenter.y + crankRadius * std::sin(angle);
    sf::Vector2f crankPos(crankX, crankY);

    float dx = crankX - crankCenter.x;
    float diffX = std::abs(dx);
    float rodVerticalH = std::sqrt(rodLength * rodLength - diffX * diffX);
    sf::Vector2f pistonPos(crankCenter.x, crankY - rodVerticalH);

    // 2. Ciclo 4 Tiempos
    float cyclePhase = getCyclePhaseInternal(angle);

    const float PI = M_PI;
    const float TWO_PI = 2.0 * M_PI;
    const float THREE_PI = 3.0 * M_PI;

    float deckHeight = headBlock.getPosition().y;
    float pistonTopY = pistonPos.y - 25.f; 
    float chamberHeight = std::max(0.f, pistonTopY - deckHeight);
    gasChamber.setSize(sf::Vector2f(64.f, chamberHeight));

    float intakeLift = 0.f;
    float exhaustLift = 0.f;
    sf::Color gasColor = sf::Color::Transparent;
    sf::Color sparkColor = sf::Color(30, 30, 30);

    // ADMISIÓN
    if (cyclePhase >= 0 && cyclePhase < PI) {
        intakeLift = std::sin(cyclePhase) * 10.f; 
        gasColor = colorFuel;
    }
    // COMPRESIÓN
    else if (cyclePhase >= PI && cyclePhase < TWO_PI) {
        float factor = (cyclePhase - PI) / PI;
        gasColor.r = colorFuel.r * (1.f - factor) + colorComp.r * factor;
        gasColor.g = colorFuel.g * (1.f - factor) + colorComp.g * factor;
        gasColor.b = colorFuel.b * (1.f - factor) + colorComp.b * factor;
        gasColor.a = 150 + (105 * factor);
    }
    // EXPLOSIÓN
    else if (cyclePhase >= TWO_PI && cyclePhase < THREE_PI) {
        float powerPhase = cyclePhase - TWO_PI;
        if (powerPhase < 0.25f) { // Chispa corta
            sparkColor = sf::Color::White;
            gasColor = sf::Color(255, 255, 200, 255);
        } else {
            float fade = powerPhase / PI;
            gasColor.r = 255;
            gasColor.g = 255 * (1.f - fade * 0.8f); // Pasa a rojo
            gasColor.b = 0;
            gasColor.a = 240 * (1.f - fade * 0.5f);
        }
    }
    // ESCAPE
    else {
        float exhaustPhase = cyclePhase - THREE_PI;
        exhaustLift = std::sin(exhaustPhase) * 10.f;
        gasColor = colorExhaust;
    }

    float valveBaseY = deckHeight - 45.f;
    valveIntake.setPosition(valveIntake.getPosition().x, valveBaseY + intakeLift);
    valveExhaust.setPosition(valveExhaust.getPosition().x, valveBaseY + exhaustLift);
    
    sparkPlugTip.setFillColor(sparkColor);
    gasChamber.setFillColor(gasColor);

    // 3. Visuales
    crankPin.setPosition(crankPos);

    sf::Vector2f delta = crankPos - crankCenter;
    float armRotation = std::atan2(delta.y, delta.x) * 180.f / M_PI;
    crankArm.setPosition(crankCenter);
    crankArm.setRotation(armRotation); 

    pistonHead.setPosition(pistonPos);
    wristPin.setPosition(pistonPos);

    pistonRod.setPosition(pistonPos); 
    sf::Vector2f rodDelta = crankPos - pistonPos; 
    float rodAngle = std::atan2(rodDelta.y, rodDelta.x) * 180.f / M_PI;
    pistonRod.setRotation(rodAngle - 90.f); 
}

void Piston::draw(sf::RenderWindow& window) {
    window.draw(gasChamber);
    window.draw(sparkPlugTip);
    window.draw(valveIntake);
    window.draw(valveExhaust);
    window.draw(leftBlock);
    window.draw(rightBlock);
    window.draw(headBlock);
    window.draw(sparkPlugBody);
    window.draw(pistonRod);
    window.draw(pistonHead);
    window.draw(wristPin);
    window.draw(crankArm);
    window.draw(mainBearing);
    window.draw(crankPin);      
}