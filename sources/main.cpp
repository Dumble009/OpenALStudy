// ----------STLのinclude----------
#include <iostream>
#include <string>

// ----------OpenALのinclude-------
#include "al.h"
#include "alc.h"

// ----------WavAgentのinclude-----
#include "WavAgent.h"

constexpr std::string_view WAV_FILE_PATH = "assets/beep.wav";

int main(void)
{
    std::cout << "Start!!" << std::endl;

    auto Device = alcOpenDevice(NULL);

    if (Device)
    {
        auto Context = alcCreateContext(Device, NULL);
    }

    auto defaultDeviceSpecifier = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    std::string deviceName = std::string(defaultDeviceSpecifier);
    std::cout << "Device Name is : " << deviceName << std::endl;

    auto wavSoundData = wavAgent::SoundData();
    auto wavAgentResult = wavAgent::Load(WAV_FILE_PATH, &wavSoundData);

    if (!wavAgent::IsWavAgentActionSucceeded(wavAgentResult))
    {
        std::cout << "WavAgent Load is failed.\n"
                  << wavAgent::GetDescriptionOfErrorCode(wavAgentResult) << std::endl;
    }

    alGetError(); // エラーコードをクリア

    auto Context = alcGetCurrentContext();
    Device = alcGetContextsDevice(Context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(Context);
    alcCloseDevice(Device);

    std::cout << "Finish!!" << std::endl;
    return 0;
}