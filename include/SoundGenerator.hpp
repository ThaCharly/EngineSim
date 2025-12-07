#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>

class SoundGenerator : public sf::SoundStream {
public:
    SoundGenerator() : currentRPM(0), amplitude(0) {
        // Inicializamos el stream: 1 canal (mono), 44100Hz
        initialize(1, 44100);
    }

    void setRPM(float rpm) {
        currentRPM = rpm;
    }

    void setVolume(float vol) {
        amplitude = vol; // 0.0 a 1.0 (aprox)
    }

protected:
    // SFML nos pide rellenar 'samples' cada vez que se queda sin audio
    virtual bool onGetData(Chunk& data) {
        const int samplesToStream = 4096; // Tamaño del buffer
        static std::vector<sf::Int16> samples(samplesToStream);

        // Frecuencia de muestreo
        const float sampleRate = 44100.f;

        for (int i = 0; i < samplesToStream; ++i) {
            // Frecuencia base del motor:
            // 4 tiempos = 1 explosión cada 2 vueltas.
            // Frecuencia (Hz) = (RPM / 60) / 2.
            // Para que suene más "gordo", multiplicamos por armónicos.
            float baseFreq = (currentRPM / 60.f); 
            if (baseFreq < 1.0f) baseFreq = 1.0f; // Evitar div 0

            time += 1.0f / sampleRate;

            // SÍNTESIS: Onda diente de sierra (agresiva para motores)
            // fmod(time * freq, 1.0) genera 0..1 repetitivo
            float wave = 0.f;
            
            // Fundamental (grave)
            wave += 0.5f * (std::fmod(time * baseFreq * 25.0f, 1.0f) * 2.0f - 1.0f);
            // Armónico agudo (valvuleo)
            wave += 0.3f * (std::fmod(time * baseFreq * 50.0f, 1.0f) * 2.0f - 1.0f);
            
            // Ruido blanco (aire/escape)
            float noise = ((rand() % 100) / 50.f - 1.f) * 0.2f;

            float finalSample = (wave + noise) * amplitude * 10000.f; // Escalar a Int16
            
            // Hard clip para distorsión de motor sucio
            if (finalSample > 30000) finalSample = 30000;
            if (finalSample < -30000) finalSample = -30000;

            samples[i] = static_cast<sf::Int16>(finalSample);
        }

        data.samples = &samples[0];
        data.sampleCount = samplesToStream;
        return true;
    }

    virtual void onSeek(sf::Time timeOffset) {
        // No necesario para generadores procedurales
    }

private:
    float currentRPM;
    float amplitude;
    float time = 0.f;
};