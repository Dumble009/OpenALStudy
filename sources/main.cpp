// ----------STLのinclude----------
#include <iostream>
#include <string>
#include <vector>

// ----------OpenALのinclude-------
#include "al.h"
#include "alc.h"

// ----------WavAgentのinclude-----
#include "WavAgent.h"

constexpr std::string_view WAV_FILE_PATH = "assets/beep.wav";

void loadWaveData(std::vector<wavAgent::SampleUnsigned8bit> &wave,
                  const wavAgent::SoundData &soundData)
{
    wavAgent::MetaData metaData{};
    auto ret = soundData.GetMetaData(&metaData);
    if (!wavAgent::IsWavAgentActionSucceeded(ret))
    {
        std::cout << "Getting MetaData failed. : " << wavAgent::GetDescriptionOfErrorCode(ret) << std::endl;
        return;
    }

    int channelCount;
    ret = metaData.GetChannelCount(channelCount);
    if (!wavAgent::IsWavAgentActionSucceeded(ret))
    {
        std::cerr << "Getting ChannnelCount failed. : " << wavAgent::GetDescriptionOfErrorCode(ret) << std::endl;
        return;
    }

    std::vector<void *> pWaves(channelCount, nullptr);
    for (int channel = 0; channel < channelCount; channel++)
    {
        ret = soundData.GetWave(&pWaves[channel], channel);
        if (!wavAgent::IsWavAgentActionSucceeded(ret))
        {
            std::cerr << "Getting WaveData failed. : " << wavAgent::GetDescriptionOfErrorCode(ret) << std::endl;
            return;
        }
    }

    wavAgent::SampleFormatType sampleFormatType;
    ret = metaData.GetSampleFormat(sampleFormatType);
    if (!wavAgent::IsWavAgentActionSucceeded(ret))
    {
        std::cerr << "Getting SampleFormat failed. : " << wavAgent::GetDescriptionOfErrorCode(ret) << std::endl;
        return;
    }

    int sampleCount;
    ret = metaData.GetSampleCount(sampleCount);
    if (!wavAgent::IsWavAgentActionSucceeded(ret))
    {
        std::cerr << "Getting SampleCount failed. : " << wavAgent::GetDescriptionOfErrorCode(ret) << std::endl;
        return;
    }

    int bytesPerSample = wavAgent::GetByteSizeOfFormat(sampleFormatType);

    // pWavesの内容を、チャンネル1のサンプル1->チャンネル2のサンプル1->チャンネル1のサンプル2->チャンネル2のサンプル2->...といった形に並び替え
    for (int sample = 0; sample < sampleCount; sample++)
    {
        for (int channel = 0; channel < channelCount; channel++)
        {
            for (int bytes = 0; bytes < bytesPerSample; bytes++)
            {
                wave.push_back(
                    ((wavAgent::SampleUnsigned8bit *)pWaves[channel])[sample * bytesPerSample + bytes]);
            }
        }
    }
}

ALenum convertSampleFormatTypeToALenum(const wavAgent::MetaData &metaData)
{
    wavAgent::SampleFormatType sampleFormatType;
    int channelCount;
    auto ret = metaData.GetSampleFormat(sampleFormatType);
    if (!wavAgent::IsWavAgentActionSucceeded(ret))
    {
        std::cerr << "Getting SampleFormat failed. : " << wavAgent::GetDescriptionOfErrorCode(ret) << std::endl;
        return AL_INVALID_ENUM;
    }

    ret = metaData.GetChannelCount(channelCount);
    if (!wavAgent::IsWavAgentActionSucceeded(ret))
    {
        std::cerr << "Getting ChannelCount failed. : " << wavAgent::GetDescriptionOfErrorCode(ret) << std::endl;
        return AL_INVALID_ENUM;
    }

    if (channelCount == 1)
    {
        if (sampleFormatType == wavAgent::SampleFormatType::WAV_AGENT_SAMPLE_STRUCTURE_UNSIGNED_8_BIT)
        {
            return AL_FORMAT_MONO8;
        }

        if (sampleFormatType == wavAgent::SampleFormatType::WAV_AGENT_SAMPLE_STRUCTURE_SIGNED_16_BIT)
        {
            return AL_FORMAT_MONO16;
        }
    }
    else if (channelCount == 2)
    {
        if (sampleFormatType == wavAgent::SampleFormatType::WAV_AGENT_SAMPLE_STRUCTURE_UNSIGNED_8_BIT)
        {
            return AL_FORMAT_STEREO8;
        }

        if (sampleFormatType == wavAgent::SampleFormatType::WAV_AGENT_SAMPLE_STRUCTURE_SIGNED_16_BIT)
        {
            return AL_FORMAT_STEREO16;
        }
    }

    return AL_INVALID_ENUM;
}

#define CHECK_WAV_AGENT_RESULT(val)                                                                            \
    if (!wavAgent::IsWavAgentActionSucceeded(val))                                                             \
    {                                                                                                          \
        std::cerr << "WavAgent operation failed. : " << wavAgent::GetDescriptionOfErrorCode(val) << std::endl; \
    }

int main(void)
{
    std::cout << "Start!!" << std::endl;

    auto Device = alcOpenDevice(NULL);

    if (Device)
    {
        auto Context = alcCreateContext(Device, NULL);
    }

    alGetError(); // エラーコードをクリア

    auto defaultDeviceSpecifier = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    std::string deviceName = std::string(defaultDeviceSpecifier);
    std::cout << "Device Name is : " << deviceName << std::endl;

    auto wavSoundData = wavAgent::SoundData();
    auto wavAgentResult = wavAgent::Load(WAV_FILE_PATH, &wavSoundData);
    CHECK_WAV_AGENT_RESULT(wavAgentResult)

    // バッファの作成
    constexpr int BUFFER_COUNT = 1;
    std::vector<ALuint> alBuffers(BUFFER_COUNT);
    alGenBuffers(BUFFER_COUNT, alBuffers.data());

    auto alResult = alGetError();
    if (alResult != AL_NO_ERROR)
    {
        std::cerr << "alGenBuffers failed." << std::endl;
        return -1;
    }

    // SoundDataからの波形データの取得
    std::vector<wavAgent::SampleUnsigned8bit> wave;
    loadWaveData(wave, wavSoundData);

    wavAgent::MetaData metaData{};
    wavAgentResult = wavSoundData.GetMetaData(&metaData);
    CHECK_WAV_AGENT_RESULT(wavAgentResult)

    // OpenAL上でのフォーマットの取得
    ALenum format = convertSampleFormatTypeToALenum(metaData);
    if (format == AL_INVALID_ENUM)
    {
        std::cerr << "Converting SampleFormatType To ALenum is failed." << std::endl;
        return -1;
    }

    // サンプリング周波数の取得
    int freqHz;
    wavAgentResult = metaData.GetSamplingFreqHz(freqHz);
    CHECK_WAV_AGENT_RESULT(wavAgentResult)

    // バッファへのデータの格納
    alBufferData(alBuffers[0], format, wave.data(), (ALsizei)wave.size(), freqHz);

    alResult = alGetError();
    if (alResult != AL_NO_ERROR)
    {
        std::cerr << "alBufferData failed." << std::endl;
        return -1;
    }

    // ソースの作成
    constexpr int SOURCE_COUNT = 1;
    std::vector<ALuint> alSources(SOURCE_COUNT);
    alGenSources(SOURCE_COUNT, alSources.data());

    alResult = alGetError();
    if (alResult != AL_NO_ERROR)
    {
        std::cerr << "alGetSources failed." << std::endl;
        return -1;
    }

    // バッファをソースにアタッチ
    alSourcei(alSources[0], AL_BUFFER, alBuffers[0]);
    alResult = alGetError();
    if (alResult != AL_NO_ERROR)
    {
        std::cerr << "alSourcei failed." << std::endl;
        return -1;
    }

    alSourcePlay(alSources[0]);
    alResult = alGetError();
    if (alResult != AL_NO_ERROR)
    {
        std::cerr << "alSourcePlay failed." << std::endl;
        return -1;
    }

    std::cout << "Please Press Key to Exit." << std::endl;
    char tmpC;
    std::cin >> tmpC;

    alDeleteBuffers(BUFFER_COUNT, alBuffers.data());
    alDeleteSources(SOURCE_COUNT, alSources.data());

    auto Context = alcGetCurrentContext();
    Device = alcGetContextsDevice(Context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(Context);
    alcCloseDevice(Device);

    std::cout << "Finish!!" << std::endl;
    return 0;
}