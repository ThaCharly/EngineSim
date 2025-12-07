#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

class SoundGenerator : public sf::SoundStream {
public:
    SoundGenerator() : currentRPM(0), amplitude(0) {
        initialize(1, 44100);
    }

    void setRPM(float rpm) {
        // Suavizado simple para que el tono no salte de golpe
        currentRPM = currentRPM * 0.9f + rpm * 0.1f;
    }

    void setVolume(float vol) {
        amplitude = vol;
    }

protected:
    virtual bool onGetData(Chunk& data) {
        const int samplesToStream = 4096;
        static std::vector<sf::Int16> samples(samplesToStream);
        const float sampleRate = 44100.f;

        for (int i = 0; i < samplesToStream; ++i) {
            time += 1.0f / sampleRate;

            // --- SÍNTESIS DE MOTOR DE COMBUSTIÓN ---
            
            // 1. Frecuencia de Disparo (Firing Frequency)
            // En un 4 tiempos, explota 1 vez cada 2 vueltas.
            // Hz = (RPM / 60) / 2 = RPM / 120.
            // Usamos un mínimo de 600 RPM para evitar divisiones por cero o infrasonidos raros.
            float effectiveRPM = (currentRPM < 100.f) ? 100.f : currentRPM;
            float fireFreq = effectiveRPM / 120.0f; 

            // 2. Oscilador Principal (Diente de Sierra - Sawtooth)
            // Esto marca el ritmo de las explosiones.
            // fmod genera un valor de 0.0 a 1.0 repetitivo.
            float periodPos = std::fmod(time * fireFreq, 1.0f);
            
            // Transformamos 0..1 en una onda diente de sierra que cae (-1 a 1)
            // Esta forma de onda tiene mucha energía en graves.
            float rawSaw = 1.0f - (2.0f * periodPos);

            // 3. Textura (Ruido)
            // El motor es metal golpeando y gas escapando. Eso es ruido blanco.
            float noise = ((std::rand() % 100) / 50.f - 1.f);

            // 4. Modulación (La Magia)
            // No queremos que el ruido suene todo el tiempo (shhhhh).
            // Queremos que suene fuerte cuando explota y baje.
            // Usamos el 'rawSaw' para controlar el volumen del ruido.
            // Cuando rawSaw es alto (inicio explosión), dejamos pasar el ruido.
            float envelope = rawSaw; 
            if (envelope < 0) envelope = 0; // Solo la parte positiva
            
            // Mezcla:
            // - Tono base (onda cuadrada distorsionada de la sierra) para el "cuerpo" grave.
            // - Ruido modulado para la "textura" del escape.
            
            float body = (rawSaw > 0) ? 1.0f : -1.0f; // Onda cuadrada (Square wave) = Sonido 8-bit gordo
            float engineTone = (body * 0.5f) + (noise * envelope * 0.8f);

            // 5. Filtro Low-Pass "Trucho" (Simulado)
            // A altas RPM, bajamos un poco el volumen de los agudos para que no chille.
            if (currentRPM > 4000) engineTone *= 0.5f;

            // 6. Output Final
            float finalSample = engineTone * amplitude * 8000.f;

            // Soft Clipping para distorsión analógica (evita el "clipping" digital feo)
            if (finalSample > 32000) finalSample = 32000;
            if (finalSample < -32000) finalSample = -32000;

            samples[i] = static_cast<sf::Int16>(finalSample);
        }

        data.samples = &samples[0];
        data.sampleCount = samplesToStream;
        return true;
    }

    virtual void onSeek(sf::Time timeOffset) {}

private:
    float currentRPM;
    float amplitude;
    float time = 0.f;
};