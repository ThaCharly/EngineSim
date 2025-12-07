#include <SFML/Graphics.hpp>
#include <iomanip>
#include <sstream>
#include "Engine.hpp"
#include "Piston.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Engine Simulation");
    window.setFramerateLimit(60);

    

    Engine engine;
    Piston piston(400.f, 400.f);
    

    sf::Font font;
    font.loadFromFile("../fonts/arial.ttf");

    sf::Text rpmText;
    rpmText.setFont(font);
    rpmText.setCharacterSize(24);
    rpmText.setPosition(10.f, 10.f);

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.restart().asSeconds();
        float currentRPM = engine.getRPM();


        float throttle = 0.f;
        float brake = 0.f;

        // PRENDER
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            throttle = 6.f; // golpe inicial de encendido

        // APAGAR
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            brake = 6000.f;

        // ACELERADOR NORMAL
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            throttle += 1.f;

        // FRENO
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            brake += 400.f;

        // Aplicar solo UNA vez por frame
        engine.accelerate(throttle);
        engine.deaccelerate(brake); 

        static bool cruiseMode = false;
        static bool cKeyPressed = false;
        static float cruiseRPM = 0.f;

        // Si no hay nada, desaceleración natural
        if (throttle == 0 && brake == 0) {
            if (cruiseMode) {
                engine.deaccelerate(0.f); // Sin fricción en crucero
            } else {
                engine.deaccelerate(20.f); // Fricción normal
            }
        }


                // Crucero??

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
            if (!cKeyPressed) {
                cruiseMode = !cruiseMode;
                    if (cruiseMode)
                        cruiseRPM = currentRPM;
                    cKeyPressed = true;
            }
            } else {
                cKeyPressed = false;
            }

            if (cruiseMode)
            engine.cruise(cruiseRPM);

        engine.update(dt);
        piston.update(engine.getAngle());

        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << engine.getRPM();
        rpmText.setString("RPM: " + stream.str());

        window.clear(sf::Color::Black);

        window.clear(sf::Color::Black);
        piston.draw(window);
        window.draw(rpmText);
        window.display();
    }

    return 0;
}
