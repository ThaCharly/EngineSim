#pragma once
#include <SFML/System.hpp>

class Engine {
private:
    float rpm;
    float angle;
    float throttle;
    float friction;

public:
    Engine();

    void update(float dt);
    void accelerate(float amount);
    void deaccelerate(float amount);
    void cruise(float amount);
    float getAngle() const;
    float getRPM() const;
};
