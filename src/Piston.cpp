#include "Piston.hpp"
#include <cmath>
#include <algorithm> // Para std::max

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Piston::Piston(float x, float y) 
    : crankCenter(x, y), crankRadius(50.f), rodLength(150.f) 
{
    // --- PALETA DE COLORES ---
    sf::Color steelColor(160, 160, 160);
    sf::Color darkSteel(100, 100, 100);
    sf::Color ironColor(70, 70, 80);
    sf::Color aluminumColor(200, 200, 200);
    sf::Color valveColor(180, 180, 180);

    // Colores del ciclo
    colorFuel = sf::Color(100, 200, 255, 150);  // Celeste semitransparente
    colorComp = sf::Color(50, 100, 255, 200);   // Azul intenso
    colorFire = sf::Color(255, 255, 0, 240);    // Amarillo explosión
    colorExhaust = sf::Color(100, 100, 100, 180); // Gris humo

    // 1. CIGÜEÑAL
    mainBearing.setRadius(15.f);
    mainBearing.setOrigin(15.f, 15.f);
    mainBearing.setPosition(crankCenter);
    mainBearing.setFillColor(sf::Color(40, 40, 40));

    crankArm.setSize(sf::Vector2f(crankRadius, 24.f));
    crankArm.setOrigin(0.f, 12.f);
    crankArm.setPosition(crankCenter);
    crankArm.setFillColor(darkSteel);

    crankPin.setRadius(10.f);
    crankPin.setOrigin(10.f, 10.f);
    crankPin.setFillColor(sf::Color(30, 30, 30));

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

    // --- GAS CHAMBER (NUEVO) ---
    // Se inicializa aquí, pero su tamaño y color cambian en update()
    gasChamber.setPosition(x - 32.f, deckHeight);
    gasChamber.setSize(sf::Vector2f(64.f, 0.f)); // Altura variable
    gasChamber.setFillColor(sf::Color::Transparent);

    // --- VÁLVULAS Y BUJÍA ---
    // Posición base de las válvulas (cerradas)
    valveIntake.setSize(sf::Vector2f(8.f, 50.f));
    valveIntake.setOrigin(4.f, 0.f);
    valveIntake.setPosition(x - 20.f, deckHeight - 45.f); 
    valveIntake.setFillColor(valveColor);

    valveExhaust.setSize(sf::Vector2f(8.f, 50.f));
    valveExhaust.setOrigin(4.f, 0.f);
    valveExhaust.setPosition(x + 20.f, deckHeight - 45.f);
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

void Piston::update(float angle) {
    // --- 1. CÁLCULO FÍSICO BÁSICO ---
    float crankX = crankCenter.x + crankRadius * std::cos(angle);
    float crankY = crankCenter.y + crankRadius * std::sin(angle);
    sf::Vector2f crankPos(crankX, crankY);

    float dx = crankX - crankCenter.x;
    float diffX = std::abs(dx);
    float rodVerticalH = std::sqrt(rodLength * rodLength - diffX * diffX);
    sf::Vector2f pistonPos(crankCenter.x, crankY - rodVerticalH);

    // --- 2. LÓGICA DEL CICLO DE 4 TIEMPOS ---
    
    // Necesitamos convertir el ángulo continuo en una fase de 0 a 720 grados (0 a 4*PI)
    // Ajuste: En SFML, ángulo 0 es "derecha" (3 en punto).
    // Queremos que el ciclo empiece en TDC (Punto Muerto Superior).
    // TDC ocurre visualmente a -PI/2 (-90 grados).
    // Entonces, Phase 0 = Angle + PI/2.
    
    float phaseOffset = angle + (M_PI / 2.0);
    
    // Usamos fmod para obtener el ciclo repetitivo de 0 a 4PI
    float cyclePhase = std::fmod(phaseOffset, 4.0 * M_PI);
    if (cyclePhase < 0) cyclePhase += 4.0 * M_PI; // Manejar negativos por si acaso

    // Definimos las constantes del ciclo
    const float PI = M_PI;
    const float TWO_PI = 2.0 * M_PI;
    const float THREE_PI = 3.0 * M_PI;
    const float FOUR_PI = 4.0 * M_PI;

    // Altura del pistón relativa al "deck" (tope del cilindro) para dibujar el gas
    float deckHeight = headBlock.getPosition().y;
    // El tope del pistón visualmente es su posición Y - 25 (su origen está en el centro)
    float pistonTopY = pistonPos.y - 25.f; 
    float chamberHeight = std::max(0.f, pistonTopY - deckHeight);

    gasChamber.setSize(sf::Vector2f(64.f, chamberHeight));

    // --- 3. ANIMACIÓN DE COMPONENTES ---

    // Resetear posiciones y colores
    float intakeLift = 0.f;
    float exhaustLift = 0.f;
    sf::Color gasColor = sf::Color::Transparent;
    sf::Color sparkColor = sf::Color(30, 30, 30); // Apagada

    // --- FASE 1: ADMISIÓN (0 a PI) ---
    // El pistón baja, válvula admisión abierta.
    if (cyclePhase >= 0 && cyclePhase < PI) {
        // Curva seno simple para abrir y cerrar la válvula
        intakeLift = std::sin(cyclePhase) * 10.f; 
        gasColor = colorFuel; // Entra mezcla fresca
    }
    
    // --- FASE 2: COMPRESIÓN (PI a 2PI) ---
    // El pistón sube, válvulas cerradas.
    else if (cyclePhase >= PI && cyclePhase < TWO_PI) {
        // Oscurecemos el color a medida que se comprime
        float compressionFactor = (cyclePhase - PI) / PI; // 0.0 a 1.0
        // Mezclamos colorFuel con colorComp
        gasColor.r = colorFuel.r * (1.f - compressionFactor) + colorComp.r * compressionFactor;
        gasColor.g = colorFuel.g * (1.f - compressionFactor) + colorComp.g * compressionFactor;
        gasColor.b = colorFuel.b * (1.f - compressionFactor) + colorComp.b * compressionFactor;
        gasColor.a = 150 + (105 * compressionFactor); // Más opaco
    }

    // --- FASE 3: EXPLOSIÓN / EXPANSIÓN (2PI a 3PI) ---
    // Justo al inicio (2PI) salta la chispa. El pistón baja.
    else if (cyclePhase >= TWO_PI && cyclePhase < THREE_PI) {
        float powerPhase = cyclePhase - TWO_PI; // 0.0 a PI
        
        // CHISPA: Solo al principio (primeros 15 grados aprox = 0.26 rad)
        if (powerPhase < 0.3f) {
            sparkColor = sf::Color::White; // Flash chispa
            gasColor = sf::Color::White;   // Flash cámara completa
        } else {
            // Fuego que se desvanece a humo/naranja
            float fade = powerPhase / PI; // 0 a 1
            // De Amarillo (Fire) a Rojo/Naranja
            gasColor.r = 255;
            gasColor.g = 255 * (1.f - fade); // Verde baja -> se hace rojo
            gasColor.b = 0;
            gasColor.a = 240 * (1.f - fade * 0.5); 
        }
    }

    // --- FASE 4: ESCAPE (3PI a 4PI) ---
    // El pistón sube, válvula escape abierta.
    else {
        float exhaustPhase = cyclePhase - THREE_PI; // 0 a PI
        exhaustLift = std::sin(exhaustPhase) * 10.f;
        gasColor = colorExhaust;
    }

    // Aplicar movimiento a las válvulas
    // Posición base = deckHeight - 45.
    // + Lift (bajar visualmente es sumar en Y)
    float valveBaseY = deckHeight - 45.f;
    valveIntake.setPosition(valveIntake.getPosition().x, valveBaseY + intakeLift);
    valveExhaust.setPosition(valveExhaust.getPosition().x, valveBaseY + exhaustLift);
    
    // Aplicar colores
    sparkPlugTip.setFillColor(sparkColor);
    gasChamber.setFillColor(gasColor);

    // --- ACTUALIZAR VISUALES MECÁNICOS ---
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
    // 1. Gas (Fondo de la cámara) - Primero para que quede detrás del bloque y pistón
    window.draw(gasChamber);

    // 2. Detalles internos traseros
    window.draw(sparkPlugTip);
    window.draw(valveIntake);
    window.draw(valveExhaust);
    
    // 3. Bloque (Capa media)
    window.draw(leftBlock);
    window.draw(rightBlock);
    window.draw(headBlock);
    window.draw(sparkPlugBody);

    // 4. Mecanismo (Frente)
    window.draw(pistonRod);
    window.draw(pistonHead);
    window.draw(wristPin);
    window.draw(crankArm);
    window.draw(mainBearing);
    window.draw(crankPin);      
}