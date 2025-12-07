#include "Engine.hpp"
#include <cmath>
#include <cstdlib> // rand

Engine::Engine()
    : rpm(0.f), angle(0.f), throttle(0.f), friction(50.f), 
      maxRPM(2000.f), totalRevolutions(0.0), revLimiterActive(false) {}

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
    // 1. Física básica
    if (throttle > 0.f) {
        // Si estamos cortando inyección, ignoramos el acelerador momentáneamente
        if (!revLimiterActive) {
            rpm += 300.f * throttle * dt;
        } else {
            // Efecto de corte: cae RPM bruscamente aunque aceleres
            rpm -= 500.f * dt; 
        }
    } else {
        rpm -= friction * dt;
        if (rpm < 0.f) rpm = 0.f;
    }

    // 2. Lógica del Limitador (Rev Limiter)
    // Histéresis: Corta a 2000, vuelve a activar a 1900
    if (rpm > maxRPM) {
        rpm = maxRPM + (rand()%50); // Pequeña variación para caos
        revLimiterActive = true; 
    }
    
    if (revLimiterActive && rpm < (maxRPM - 100.f)) {
        revLimiterActive = false; // Recupera inyección
    }

    // 3. Odómetro y Ángulo
    // Vueltas en este frame = (RPM / 60) * dt
    float revsThisFrame = (rpm / 60.f) * dt;
    totalRevolutions += revsThisFrame;

    angle += revsThisFrame * 2.f * M_PI; // angle en radianes
}

float Engine::getAngle() const { return angle; }
float Engine::getRPM() const { return rpm; }
double Engine::getTotalRevolutions() const { return totalRevolutions; }
bool Engine::isRedlining() const { return revLimiterActive || (rpm > maxRPM - 100.f); }