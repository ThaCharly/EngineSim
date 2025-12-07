#include "Engine.hpp"
#include <cmath>

Engine::Engine()
    : rpm(0.f), angle(0.f), throttle(0.f), friction(50.f) {}

void Engine::accelerate(float amount) {
    throttle = amount;
}

void Engine::cruise(float amount) {
    rpm = amount;
}

void Engine::deaccelerate(float amount) {
    friction = amount;
}

void Engine::update(float dt) {
    if (throttle > 0.f) {
        rpm += 300.f * throttle * dt;  // Aceleración
    } else {
        rpm -= friction * dt;          // Pierde vueltas
        if (rpm < 0.f) rpm = 0.f;
    }

    // Limitar RPM
    if (rpm > 2001.f) rpm = 2001.f;

    // actualiza ángulo del cigüeñal
    angle += (rpm / 60.f) * 2.f * M_PI * dt;
}

float Engine::getAngle() const { return angle; }
float Engine::getRPM() const { return rpm; }
