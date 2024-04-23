#pragma once

#include <SFML/Audio.hpp>
#include <thread>
#include <string>
#include <windows.h>

class Audio;

extern vector<Audio> Audios;
extern int currentAudioId;

class Audio
{
public:

    std::string path = "";
    int id = -1;
    int volume = 100;

    sf::Sound sound;
    sf::SoundBuffer buffer;

    Audio() {};

    Audio(std::string filePath, bool wholePath, int vol)
    {
        if (wholePath)
        {
            path = filePath;
        }
        else
        {
            path = "res/SFX/" + filePath + ".wav";
        }

        if (!buffer.loadFromFile(path))
        {
            // Si hay un error al cargar el archivo, muestra un mensaje
            std::cout << "Error al cargar el audio del archivo " << path << std::endl;
            //return;
        }

        volume = vol;

        currentAudioId++;
        id = currentAudioId;
    }

    void Play()
    {
        std::thread soundThread([this]()
        {
            // Asignar el buffer de sonido al objeto de sonido
            sound.setBuffer(buffer);
            sound.setVolume(volume);
            
            // Reproducir el sonido
            sound.play();

            // Esperar hasta que se termine de reproducir el sonido
            while (sound.getStatus() == sf::Sound::Playing) {
                // Pausa el hilo durante un breve tiempo para evitar un uso excesivo de la CPU
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            buffer = sf::SoundBuffer(); // Reasignar un buffer vacío para liberar el recurso
            sound = sf::Sound();

            id = -1;
        });

        // Desconectar el hilo del hilo principal
        soundThread.detach();
    }
};
