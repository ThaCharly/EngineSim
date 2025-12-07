#include <SFML/Graphics.hpp>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstdlib> // para rand()
#include "Engine.hpp"
#include "Piston.hpp"

// --- ESTRUCTURA PARA EL HUMO ---
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;     // Cuánto vive
    float maxLifetime;
    float size;
    float rotation;
    float angularVelocity;
};

int main() {
    // Ventana un poco más ancha para que entre el HUD cómodo
    sf::RenderWindow window(sf::VideoMode(900, 600), "Engine Simulation - Ultimate Edition");
    window.setFramerateLimit(60);

    Engine engine;
    Piston piston(400.f, 400.f); // Centrado a la izquierda, HUD a la derecha
    
    sf::Font font;
    // Intentar cargar fuentes del sistema si las locales fallan, o usar las provistas
    if (!font.loadFromFile("../fonts/arial.ttf")) {
        // Fallback simple por si acaso
    }

    // --- ELEMENTOS DEL HUD ---
    sf::Text rpmText;
    rpmText.setFont(font);
    rpmText.setCharacterSize(40); // Más grande
    rpmText.setFillColor(sf::Color::White);
    rpmText.setPosition(600.f, 50.f);

    sf::Text phaseText;
    phaseText.setFont(font);
    phaseText.setCharacterSize(25);
    phaseText.setFillColor(sf::Color(255, 200, 0)); // Dorado
    phaseText.setPosition(600.f, 110.f);

    sf::Text controlsText;
    controlsText.setFont(font);
    controlsText.setCharacterSize(16);
    controlsText.setFillColor(sf::Color(180, 180, 180));
    controlsText.setPosition(600.f, 300.f);
    controlsText.setString(
        "CONTROLES:\n\n"
        "[E]      Encender\n"
        "[W]      Acelerador\n"
        "[ESPACIO] Freno\n"
        "[Q]      Apagar (Freno Total)\n"
        "[C]      Crucero\n"
        "[S]      Camara Lenta (Hold)"
    );

    // Barras visuales
    sf::RectangleShape throttleBarBack(sf::Vector2f(200.f, 20.f));
    throttleBarBack.setPosition(600.f, 160.f);
    throttleBarBack.setFillColor(sf::Color(50, 50, 50));
    
    sf::RectangleShape throttleBarFill(sf::Vector2f(0.f, 20.f));
    throttleBarFill.setPosition(600.f, 160.f);
    throttleBarFill.setFillColor(sf::Color(0, 255, 100)); // Verde

    // Sistema de partículas
    std::vector<Particle> smokeParticles;

    sf::Clock clock;
    
    // Variable para la cámara lenta
    float timeScale = 1.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Delta time real
        float dtReal = clock.restart().asSeconds();
        
        // --- LOGICA DE CAMARA LENTA ---
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            timeScale = 0.1f; // 10% de velocidad
        } else {
            timeScale = 1.0f;
        }

        // El motor usa el dt escalado
        float dtSim = dtReal * timeScale;
        
        float currentRPM = engine.getRPM();

        // Inputs
        float throttle = 0.f;
        float brake = 0.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) throttle = 6.f; 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) brake = 6000.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) throttle += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) brake += 400.f;

        // Física del motor
        engine.accelerate(throttle);
        engine.deaccelerate(brake);

        if (throttle == 0 && brake == 0)
             engine.deaccelerate(20.f); // Fricción suave

        // Crucero (simplificado para el ejemplo)
        static bool cruiseMode = false;
        static bool cLastState = false;
        bool cState = sf::Keyboard::isKeyPressed(sf::Keyboard::C);
        if (cState && !cLastState) {
            cruiseMode = !cruiseMode;
            if(cruiseMode) engine.cruise(currentRPM);
        }
        cLastState = cState;

        if (cruiseMode && throttle == 0 && brake == 0)
            engine.deaccelerate(0.f); // Mantiene velocidad

        engine.update(dtSim);
        piston.update(engine.getAngle());

        // --- SISTEMA DE PARTÍCULAS (HUMO) ---
        // 1. Generar humo si está en escape y el motor gira rápido
        if (piston.isExhaustPhase(engine.getAngle()) && currentRPM > 50.f) {
            // Generar 2 partículas por frame para densidad
            for(int i=0; i<2; i++) {
                Particle p;
                p.position = piston.getExhaustPortPosition();
                // Velocidad aleatoria hacia arriba/derecha
                float speedX = (rand() % 50 + 50); 
                float speedY = -(rand() % 50 + 20);
                p.velocity = sf::Vector2f(speedX, speedY);
                p.maxLifetime = 1.0f + (rand()%100)/100.f; // 1 a 2 segs
                p.lifetime = p.maxLifetime;
                p.size = (rand() % 10) + 5.f;
                p.rotation = rand() % 360;
                p.angularVelocity = (rand() % 100) - 50.f;
                smokeParticles.push_back(p);
            }
        }

        // 2. Actualizar partículas (SIEMPRE usar dtReal para que la animación fluya suave
        //    incluso si el motor va en cámara lenta, O usar dtSim si quieres humo lento.
        //    Usaremos dtSim para que sea coherente con el efecto Matrix)
        for (auto it = smokeParticles.begin(); it != smokeParticles.end(); ) {
            it->lifetime -= dtSim;
            if (it->lifetime <= 0) {
                it = smokeParticles.erase(it);
            } else {
                it->position += it->velocity * dtSim;
                it->rotation += it->angularVelocity * dtSim;
                it->size += 10.f * dtSim; // El humo se expande
                it->velocity.y -= 10.f * dtSim; // Flota suave
                ++it;
            }
        }

        // --- ACTUALIZAR HUD ---
        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << currentRPM;
        rpmText.setString(ss.str() + " RPM");
        
        phaseText.setString(piston.getCyclePhaseName(engine.getAngle()));
        
        // Barra acelerador (visual) - Escala simple
        float barWidth = std::min(throttle * 20.f, 200.f); 
        if (cruiseMode) {
             throttleBarFill.setFillColor(sf::Color::Cyan); // Azul para crucero
             throttleBarFill.setSize(sf::Vector2f(200.f, 20.f)); // Lleno en crucero visualmente
        } else {
             throttleBarFill.setFillColor(sf::Color(0, 255, 100));
             throttleBarFill.setSize(sf::Vector2f(barWidth, 20.f));
        }

        // --- RENDER ---
        window.clear(sf::Color(25, 25, 30)); // Fondo Gris Oscuro (Técnico)
        
        // Dibujar partículas (detrás del HUD, delante o al nivel del motor)
        for (const auto& p : smokeParticles) {
            sf::RectangleShape shape(sf::Vector2f(p.size, p.size));
            shape.setOrigin(p.size/2, p.size/2);
            shape.setPosition(p.position);
            shape.setRotation(p.rotation);
            
            // Color gris que se desvanece
            float alpha = (p.lifetime / p.maxLifetime) * 150;
            shape.setFillColor(sf::Color(150, 150, 150, (sf::Uint8)alpha));
            
            window.draw(shape);
        }

        piston.draw(window);

        // Dibujar HUD
        window.draw(rpmText);
        window.draw(phaseText);
        window.draw(throttleBarBack);
        window.draw(throttleBarFill);
        window.draw(controlsText);

        window.display();
    }

    return 0;
}