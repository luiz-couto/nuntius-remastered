#pragma once

#define _USE_MATH_DEFINES
#include <fmod.hpp>
#include <fmod_errors.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <conio.h>

#pragma comment(lib, "fmod\\lib\\x64\\fmod_vc.lib")
void playMusic(FMOD::System* system, const char* soundFilePath = "sounds/alert.wav") {
    FMOD::Sound* sound = NULL;
    FMOD::Channel* channel = NULL;
    system->createSound(soundFilePath, FMOD_DEFAULT, NULL, &sound);
    system->playSound(sound, NULL, false, &channel);

    // create thread and release sound when the music is done playing
    std::thread([sound, channel]() {
        bool isPlaying = true;
        while (isPlaying) {
            channel->isPlaying(&isPlaying);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        sound->release();
    }).detach();
}