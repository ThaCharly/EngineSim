#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>

// Generador de sonido de motor basado en Impulsos Asíncronos (Jitter) y Ruido Marrón
class SoundGenerator : public sf::SoundStream {
public:
    SoundGenerator() : currentRPM(0.f), targetVolume(1.0f), 
                       samplesUntilNextFire(0), lastBrownNoise(0.f) {
        initialize(1, 44100);
        // Pre-calentamos el buffer de voces
        voices.resize(8); // 8 voces de polifonía para que los bajos se superpongan bien
    }

    void setRPM(float rpm) {
        // Suavizado para que el sonido no "patine" al acelerar
        currentRPM = currentRPM * 0.9f + rpm * 0.1f;
        if (currentRPM < 0.f) currentRPM = 0.f;
    }

    void setVolume(float vol) {
        targetVolume = vol;
    }

private:
    // Estructura para una única explosión individual
    struct Voice {
        bool active = false;
        float time = 0.f;
        float decayRate = 0.f;
        float amplitude = 0.f;
        float toneFreq = 0.f;
    };

    std::vector<Voice> voices;
    float currentRPM;
    float targetVolume;
    int samplesUntilNextFire;
    float lastBrownNoise; // Memoria para generar ruido marrón

protected:
    virtual bool onGetData(Chunk& data) {
        const int samplesToStream = 4096;
        static std::vector<sf::Int16> samples(samplesToStream);
        const float sampleRate = 44100.f;

        for (int i = 0; i < samplesToStream; ++i) {
            
            // --- 1. SECUENCIADOR CON JITTER (Anti-Robótico) ---
            // En lugar de usar un timer flotante perfecto, contamos muestras.
            
            samplesUntilNextFire--;
            
            if (samplesUntilNextFire <= 0) {
                // Calcular cuándo ocurre la PRÓXIMA explosión
                float fireFreq = (currentRPM / 120.f); 
                if (fireFreq < 1.0f) fireFreq = 1.0f;
                
                // Base: muestras por ciclo
                float samplesPerCycle = sampleRate / fireFreq;

                // JITTER: Variación aleatoria del +/- 10% en el tiempo de detonación
                // Esto rompe la perfección matemática que suena a "robot".
                float jitter = 1.0f + ((std::rand() % 200) / 1000.f - 0.1f); 
                
                samplesUntilNextFire = static_cast<int>(samplesPerCycle * jitter);

                triggerExplosion();
            }

            // --- 2. MEZCLA DE VOCES (Anti-Lata) ---
            float mixedOutput = 0.f;

            for (auto& v : voices) {
                if (!v.active) continue;

                v.time += 1.0f / sampleRate;

                // Envolvente de volumen exponencial (Golpe seco)
                float envelope = std::exp(-v.time * v.decayRate);

                if (envelope < 0.001f) {
                    v.active = false;
                    continue;
                }

                // A. SUB-BAJOS (Cuerpo)
                // Onda seno pura en frecuencia muy baja (30-50Hz) para el "PUM"
                // Pitch drop: El tono cae ligeramente durante el golpe
                float instantFreq = v.toneFreq * (1.0f - v.time * 2.0f);
                float sub = std::sin(v.time * instantFreq * 2.f * 3.14159f);

                // B. RUIDO MARRÓN (Textura)
                // Generamos ruido blanco
                float white = (std::rand() % 100) / 50.f - 1.f;
                // Lo filtramos agresivamente para hacerlo "Marrón" (Graves sucios)
                // Esto elimina el sonido a "lata" o "arena".
                lastBrownNoise = (lastBrownNoise + white) * 0.5f; 
                float noise = lastBrownNoise;

                // Mezcla por voz: Mucho Sub, Ruido moderado
                float voiceMix = (sub * 0.6f) + (noise * 0.4f);
                
                // Distorsión suave para carácter
                if (voiceMix > 1.0f) voiceMix = 1.0f;
                if (voiceMix < -1.0f) voiceMix = -1.0f;

                mixedOutput += voiceMix * envelope * v.amplitude;
            }

            // --- 3. SALIDA FINAL ---
            float finalOut = mixedOutput * targetVolume * 20000.f; 

            // Hard Limiter de seguridad
            if (finalOut > 32000.f) finalOut = 32000.f;
            if (finalOut < -32000.f) finalOut = -32000.f;

            samples[i] = static_cast<sf::Int16>(finalOut);
        }

        data.samples = &samples[0];
        data.sampleCount = samplesToStream;
        return true;
    }

    virtual void onSeek(sf::Time timeOffset) {}

    void triggerExplosion() {
        // Buscar voz libre
        for (auto& v : voices) {
            if (!v.active) {
                v.active = true;
                v.time = 0.f;
                
                // Variación aleatoria de volumen (más realismo)
                v.amplitude = 0.8f + ((std::rand() % 40) / 100.f); 

                // Configurar tono grave (Deep bass)
                // 40Hz base + un poco según RPM. Nunca sube mucho para no sonar agudo.
                v.toneFreq = 40.f + (currentRPM * 0.015f); 

                // Duración: A más RPM, golpes más cortos pero nunca instantáneos.
                // El factor 15.f asegura que el bajo tenga tiempo de retumbar.
                v.decayRate = 15.f + (currentRPM * 0.02f); 
                return;
            }
        }
        // Si no hay libres, reiniciamos la primera (robo de voz)
        voices[0].active = true;
        voices[0].time = 0.f;
    }
};