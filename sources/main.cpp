// ----------STLのinclude----------
#include <iostream>
#include <string>

// ----------OpenALのinclude-------
#include "al.h"
#include "alc.h"

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

    auto g_bEAX = alIsExtensionPresent("EAX2.0");

    alGetError();

    auto Context = alcGetCurrentContext();
    Device = alcGetContextsDevice(Context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(Context);
    alcCloseDevice(Device);

    std::cout << "Finish!!" << std::endl;
    return 0;
}