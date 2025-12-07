#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

// Clase para sintetizar sonido de motor mediante "Impulsos de Combustión"
class SoundGenerator : public sf::SoundStream {
public:
    SoundGenerator() : currentRPM(0.f), targetVolume(1.0f) {
        // Inicializamos audio mono a 44.1kHz
        initialize(1, 44100);
    }

    void setRPM(float rpm) {
        // Suavizado para evitar saltos bruscos de tono
        currentRPM = currentRPM * 0.9f + rpm * 0.1f;
        if (currentRPM < 10.f) currentRPM = 0.f;
    }

    void setVolume(float vol) {
        targetVolume = vol;
    }

protected:
    virtual bool onGetData(Chunk& data) {
        const int samplesToStream = 4096;
        static std::vector<sf::Int16> samples(samplesToStream);
        const float sampleRate = 44100.f;

        for (int i = 0; i < samplesToStream; ++i) {
            // 1. Calcular frecuencia de disparo (Firing Rate)
            // Motor 4 tiempos = 1 explosión cada 2 revoluciones (720 grados)
            // Frecuencia (Hz) = (RPM / 60) / 2
            // Ejemplo: 600 RPM -> 10 rev/s -> 5 explosiones/s (5 Hz)
            float fireFreq = (currentRPM / 120.f); 
            if (fireFreq < 0.1f) fireFreq = 0.1f; // Evitar div/0

            // Avance del tiempo del ciclo del motor (0.0 a 1.0)
            engineCycleTime += fireFreq / sampleRate;
            if (engineCycleTime >= 1.0f) {
                engineCycleTime -= 1.0f;
                // ¡MOMENTO DE EXPLOSIÓN! Reiniciamos el tiempo del pulso
                pulseTimer = 0.0f; 
            }

            // 2. Generar el "Golpe" (Impulso)
            // Cuando pulseTimer es bajo (recién explotó), suena fuerte.
            // Luego se desvanece rápidamente.
            float sampleValue = 0.f;

            // Incrementamos timer del pulso
            pulseTimer += 1.0f / sampleRate;

            // Solo generamos sonido si el pulso es "fresco" (ej. menos de 0.15 seg)
            // Esto crea el silencio entre explosiones a bajas RPM.
            if (pulseTimer < 0.15f) {
                // A. Tono Base (El "Cuerpo" del sonido)
                // Usamos una onda senoidal grave que baja de tono ligeramente (golpe de tambor)
                // Frecuencia base: 60Hz + un poco de RPM para que "grite" al acelerar
                float bodyFreq = 60.f + (currentRPM * 0.03f);
                
                // Chirp: Hacemos que la frecuencia caiga rápido en el tiempo del pulso (efecto "Piuuum" muy sutil)
                float instantFreq = bodyFreq * (1.0f - pulseTimer * 2.f); 
                
                // Onda principal
                float wave = std::sin(pulseTimer * instantFreq * 2.f * 3.14159f);

                // B. Distorsión (El "Carácter")
                // Elevamos al cubo para recortar la onda y hacerla más "cuadrada/agresiva" sin ruido
                wave = wave * wave * wave; 

                // C. Envolvente de Volumen (Decay)
                // El sonido empieza fuerte y muere exponencialmente
                float decay = std::exp(-pulseTimer * 30.f); // Ajustar 30.f para duración del golpe

                sampleValue = wave * decay;
            }

            // 3. Salida Final
            // Escalamos a 16-bit con volumen
            float finalOut = sampleValue * targetVolume * 30000.f;
            
            // Hard Clip de seguridad
            if (finalOut > 32000.f) finalOut = 32000.f;
            if (finalOut < -32000.f) finalOut = -32000.f;

            samples[i] = static_cast<sf::Int16>(finalOut);
        }

        data.samples = &samples[0];
        data.sampleCount = samplesToStream;
        return true;
    }

    virtual void onSeek(sf::Time timeOffset) {}

private:
    float currentRPM;
    float targetVolume;
    
    // Variables de estado para la síntesis
    float engineCycleTime = 0.f; // Posición en el ciclo de 720 grados (0 a 1)
    float pulseTimer = 1.0f;     // Tiempo desde la última explosión (segundos)
};