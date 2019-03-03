/*
 * author : Shuichi TAKANO
 * since  : Sun Jan 13 2019 21:45:17
 */
#ifndef EA3337E2_0134_1394_1475_68D3301F3EEE
#define EA3337E2_0134_1394_1475_68D3301F3EEE

#include "audio_stream.h"
#include <memory>
#include <util/simple_ring_buffer.h>

namespace audio
{

class AudioOutDriver
{
public:
    virtual bool isDriverUseUpdate() const = 0;
    virtual void onAttach()                = 0;
    virtual void onDetach()                = 0;
    virtual void onUpdate(const std::array<int32_t, 2>* data, size_t n) {}

    virtual uint32_t getSampleRate() const = 0;
    virtual void setVolume(float v)        = 0;
    virtual float getVolume() const        = 0;

    // isDriverUseUpdate() = true のドライバは onUpdate() によって更新される
    // isDriverUseUpdate() = false のドライバは
    // AudioOutDriverManager::lock()/unlock()/generateSamples()
    // を使って自律的にサンプルを更新する.
};

class AudioOutDriverManager
{
    struct Impl;
    std::unique_ptr<Impl> pimpl_;

public:
    using Sample            = std::array<int32_t, 2>; // 16.8 L/R
    using HistorySample     = std::array<int16_t, 2>; // 16.0 L/R
    using HistoryRingBuffer = util::SimpleRingBuffer<HistorySample>;

public:
    void start();

    void setAudioStreamOut(AudioStreamOut*);
    void setDriver(AudioOutDriver*);
    void setVolume(float v);

    bool lock(const AudioOutDriver*);
    void unlock();
    size_t generateSamples(size_t n);
    const Sample* getSampleBuffer();

    const HistoryRingBuffer& getHistoryBuffer() const;
    void lockHistoryBuffer();
    void unlockHistoryBuffer();

    static constexpr size_t getUnitSampleCount() { return 128; }
    static constexpr size_t getSampleRate() { return 44100; }

    static AudioOutDriverManager& instance();

private:
    AudioOutDriverManager();
    ~AudioOutDriverManager();
};

} // namespace audio

#endif /* EA3337E2_0134_1394_1475_68D3301F3EEE */
