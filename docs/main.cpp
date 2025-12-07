#include <SFML/Graphics.hpp>
#include "Engine.hpp"
#include "Piston.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Engine Simulation");
    window.setFramerateLimit(60);

    

    Engine engine;
    Piston piston(400.f, 300.f);

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


        // Acelerador
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            engine.accelerate(1.f);
        else
            engine.accelerate(0.f);

                // Freno??
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            engine.deaccelerate(200.f);
        else
            engine.deaccelerate(50.f);

            // Crucero??
        static bool cruiseMode = false;
        static bool cKeyPressed = false;
        static float cruiseRPM = 0.f;

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

        rpmText.setString("RPM: " + std::to_string((int)engine.getRPM()));

// NO hagas clear negro duro
// window.clear(); // <- sacalo o cambialo por un fade sutil

sf::RectangleShape fade(sf::Vector2f(window.getSize().x, window.getSize().y));
fade.setFillColor(sf::Color(0, 0, 0, 25)); // probá 15–40 de alpha

window.draw(fade); // esto limpia pero con persistencia


piston.draw(window);

window.display();

        window.draw(rpmText);

    }

    return 0;
}
