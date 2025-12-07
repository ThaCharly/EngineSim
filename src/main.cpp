#include <SFML/Graphics.hpp>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstdlib>
#include "Engine.hpp"
#include "Piston.hpp"
#include "SoundGenerator.hpp" // <--- Importante!

// --- PARTÍCULAS (Mismo código de antes) ---
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    float size;
    float rotation;
    float angularVelocity;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(900, 600), "Engine Simulation - Ultimate Edition");
    window.setFramerateLimit(60);

    // --- CÁMARA (VIEW) PARA EL EFECTO DE VIBRACIÓN ---
    sf::View view = window.getDefaultView();
    sf::Vector2f baseCenter = view.getCenter();

    Engine engine;
    Piston piston(400.f, 400.f);
    
    // --- SONIDO ---
    SoundGenerator engineSound;
    engineSound.play(); // Arrancar el stream (sonará silencio si rpm=0)

    sf::Font font;
    if (!font.loadFromFile("../fonts/arial.ttf")) {
        // Manejo de error básico
    }

    // --- HUD MEJORADO ---
    sf::Text rpmText;
    rpmText.setFont(font);
    rpmText.setCharacterSize(50); // Masivo
    rpmText.setPosition(600.f, 40.f);

    sf::Text statsText; // Odómetro y Tiempo
    statsText.setFont(font);
    statsText.setCharacterSize(18);
    statsText.setFillColor(sf::Color(200, 200, 200));
    statsText.setPosition(600.f, 100.f);

    sf::Text phaseText;
    phaseText.setFont(font);
    phaseText.setCharacterSize(25);
    phaseText.setPosition(600.f, 140.f);

    sf::Text controlsText;
    controlsText.setFont(font);
    controlsText.setCharacterSize(14);
    controlsText.setFillColor(sf::Color(150, 150, 150));
    controlsText.setPosition(600.f, 350.f);
    controlsText.setString(
        "CONTROLES:\n\n"
        "[E]      Arranque\n"
        "[W]      Acelerar\n"
        "[ESP]    Freno\n"
        "[Q]      Apagar\n"
        "[C]      Crucero\n"
        "[S]      Slow-Mo"
    );

    // Barra RPM Gráfica (Fondo + Relleno)
    sf::RectangleShape rpmBarBack(sf::Vector2f(250.f, 10.f));
    rpmBarBack.setPosition(600.f, 95.f);
    rpmBarBack.setFillColor(sf::Color(30, 30, 30));

    sf::RectangleShape rpmBarFill(sf::Vector2f(0.f, 10.f));
    rpmBarFill.setPosition(600.f, 95.f);

    std::vector<Particle> smokeParticles;
    sf::Clock clock;
    sf::Clock runTimeClock; // Tiempo total corriendo
    
    float timeScale = 1.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        float dtReal = clock.restart().asSeconds();
        
        // Input Slow-Mo
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) timeScale = 0.1f;
        else timeScale = 1.0f;

        float dtSim = dtReal * timeScale;
        float currentRPM = engine.getRPM();

        // --- SONIDO (Actualizar frecuencia y volumen) ---
        // Volumen basado en RPM (más rápido = más fuerte)
        float targetVol = 0.0f;
        if (currentRPM > 50) targetVol = 0.2f + (currentRPM / 2500.f) * 0.8f;
        
        // Si estamos acelerando (W), ruge más fuerte
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) targetVol += 0.2f;

        engineSound.setRPM(currentRPM);
        engineSound.setVolume(targetVol);


        // --- VIBRACIÓN DE PANTALLA (SCREEN SHAKE) ---
// --- VIBRACIÓN DE PANTALLA (SCREEN SHAKE) MEJORADO ---
        // Solo vibra si pasa de 1800 RPM (ralentí alto)
        if (currentRPM > 1800.f) {
            // Factor de intensidad suavizado: (RPM actual - base) / rango
            float shakeIntensity = (currentRPM - 800.f) / 1200.f; // 0.0 a 1.0 aprox hasta 2000rpm
            if (shakeIntensity > 1.0f) shakeIntensity = 1.0f;     // Tope máximo (Clamp)

            // Multiplicador reducido drásticamente: antes 1.5f, ahora 0.3f
            float maxOffset = 3.0f; // Máximo desplazamiento en píxeles
            
            float offsetX = ((rand() % 100) / 50.f - 1.f) * shakeIntensity * maxOffset;
            float offsetY = ((rand() % 100) / 50.f - 1.f) * shakeIntensity * maxOffset;
            
            view.setCenter(baseCenter.x + offsetX, baseCenter.y + offsetY);
        } else {
            view.setCenter(baseCenter);
        }
        window.setView(view);


        // Inputs
        float throttle = 0.f;
        float brake = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) throttle = 6.f; 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) brake = 6000.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) throttle += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) brake += 400.f;

        engine.accelerate(throttle);
        engine.deaccelerate(brake);

        if (throttle == 0 && brake == 0) engine.deaccelerate(20.f);

        static bool cruiseMode = false;
        static bool cLastState = false;
        bool cState = sf::Keyboard::isKeyPressed(sf::Keyboard::C);
        if (cState && !cLastState) {
            cruiseMode = !cruiseMode;
            if(cruiseMode) engine.cruise(currentRPM);
        }
        cLastState = cState;

        if (cruiseMode && throttle == 0 && brake == 0) engine.deaccelerate(0.f);

        engine.update(dtSim);
        piston.update(engine.getAngle());

        // --- PARTICULAS ---
        if (piston.isExhaustPhase(engine.getAngle()) && currentRPM > 50.f) {
            // Más partículas a más RPM
            int pCount = 1 + (int)(currentRPM / 800.f);
            for(int i=0; i<pCount; i++) {
                Particle p;
                p.position = piston.getExhaustPortPosition();
                float speedX = (rand() % 60 + 60); 
                float speedY = -(rand() % 40 + 20);
                p.velocity = sf::Vector2f(speedX, speedY);
                p.maxLifetime = 0.5f + (rand()%100)/200.f; 
                p.lifetime = p.maxLifetime;
                p.size = (rand() % 8) + 4.f;
                p.rotation = rand() % 360;
                p.angularVelocity = (rand() % 100) - 50.f;
                smokeParticles.push_back(p);
            }
        }

        for (auto it = smokeParticles.begin(); it != smokeParticles.end(); ) {
            it->lifetime -= dtReal; // Usar dtReal para fluidez visual independiente de slowmo
            if (it->lifetime <= 0) {
                it = smokeParticles.erase(it);
            } else {
                it->position += it->velocity * dtReal; 
                it->rotation += it->angularVelocity * dtReal;
                it->size += 15.f * dtReal; 
                it->velocity *= 0.98f; // Fricción aire
                it->velocity.y -= 5.f * dtReal; // Flotabilidad
                ++it;
            }
        }

        // --- HUD LOGIC ---
        std::stringstream ssRPM;
        ssRPM << (int)currentRPM;
        rpmText.setString(ssRPM.str());

        // Color RPM dinámico
        if (engine.isRedlining()) {
            // Parpadeo rojo/blanco frenético
            if ((int)(dtReal * 1000) % 2 == 0) rpmText.setFillColor(sf::Color::Red);
            else rpmText.setFillColor(sf::Color::White);
        } else if (currentRPM > 1500) {
            rpmText.setFillColor(sf::Color(255, 100, 0)); // Naranja alerta
        } else {
            rpmText.setFillColor(sf::Color::White);
        }

        std::stringstream ssStats;
        ssStats << "ODOMETRO: " << std::fixed << std::setprecision(1) << engine.getTotalRevolutions() << " revs\n"
                << "TIEMPO: " << (int)runTimeClock.getElapsedTime().asSeconds() << " s";
        statsText.setString(ssStats.str());
        
        phaseText.setString(piston.getCyclePhaseName(engine.getAngle()));
        if (phaseText.getString() == "EXPLOSION") phaseText.setFillColor(sf::Color::Yellow);
        else phaseText.setFillColor(sf::Color(100, 200, 255));

        // Barra RPM
        float fillPct = currentRPM / 2000.f;
        if(fillPct > 1.f) fillPct = 1.f;
        rpmBarFill.setSize(sf::Vector2f(250.f * fillPct, 10.f));
        // Color de la barra (Gradiente simulado)
        if(fillPct < 0.7f) rpmBarFill.setFillColor(sf::Color::Green);
        else if(fillPct < 0.9f) rpmBarFill.setFillColor(sf::Color::Yellow);
        else rpmBarFill.setFillColor(sf::Color::Red);

        // --- RENDER ---
        window.clear(sf::Color(20, 20, 25)); // Fondo aún más técnico
        
        for (const auto& p : smokeParticles) {
            sf::RectangleShape shape(sf::Vector2f(p.size, p.size));
            shape.setOrigin(p.size/2, p.size/2);
            shape.setPosition(p.position);
            shape.setRotation(p.rotation);
            float alpha = (p.lifetime / p.maxLifetime) * 100;
            shape.setFillColor(sf::Color(150, 150, 150, (sf::Uint8)alpha));
            window.draw(shape);
        }

        piston.draw(window);

        // Dibujar HUD (asegurarse de que la vista del HUD no vibre)
        window.setView(window.getDefaultView()); // Restaurar vista quieta para el texto
        window.draw(rpmText);
        window.draw(sf::Text("RPM", font, 15)); // Etiqueta pequeña
        window.draw(rpmBarBack);
        window.draw(rpmBarFill);
        window.draw(statsText);
        window.draw(phaseText);
        window.draw(controlsText);
        
        // Restaurar vista vibratoria para el siguiente frame del motor
        window.setView(view); 

        window.display();
    }

    return 0;
}