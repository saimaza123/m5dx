#include "target.h"
#include <Adafruit_NeoPixel.h>
#include <SD.h>
#include <array>
#include <audio/audio.h>
#include <audio/audio_out.h>
#include <graphics/display.h>
#include <graphics/framebuffer.h>
#include <io/ble_manager.h>
#include <io/ble_midi.h>
#include <io/bluetooth.h>
#include <io/file_util.h>
#include <utility/Config.h>
#include <utility>

#include <utility/MPU9250.h>

#include <dirent.h>

#include "application.h"
#include "m5dx_module.h"

#include <sys/timer.h>

#include "debug.h"

#undef min
#include <io/bt_a2dp_source_manager.h>
#include <sys/job_manager.h>

#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(
    M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

constexpr uint16_t
makeColor(int r, int g, int b)
{
    return ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
}

// void
// spriteTest()
// {
//     using Sprite = TFT_eSprite;

//     Sprite img(&lcd);

//     img.setColorDepth(1);
//     img.createSprite(128, 64);
//     img.fillSprite(0);

//     img.fillTriangle(35, 0, 0, 59, 69, 59, 1);
//     img.fillTriangle(35, 79, 0, 20, 69, 20, 1);

//     img.setBitmapColor(makeColor(255, 0, 255), 0);
//     img.pushSprite(10, 20, 0);

//     img.deleteSprite();
// }

namespace
{
graphics::FrameBuffer waveViewBuffer_;
MPU9250 IMU;
} // namespace

// The setup routine runs once when M5Stack starts up
void
setup()
{
    Serial.begin(115200);
    Serial.flush();
    Serial.print("M5Stack initializing...\n");

    sys::getDefaultJobManager().start(0, 4096, "JobManager0");

    graphics::getDisplay().initialize();
    //    graphics::getDisplay().setWindow(120, 30, 20, 20);
    //    graphics::getDisplay().fill(100, 50, 250, 150, 0xffff);

    //    waveViewBuffer_.initialize(128, 64, 16);
    waveViewBuffer_.initialize(128, 128, 16);

    if (!SD.begin(TFCARD_CS_PIN, SPI, 40000000, ""))
    {
        Serial.println("Card Mount Failed");
    }

    pixels.begin();

    //    delay(500);

    target::initGPIO();
    target::restoreBus();

    dacWrite(25, 0);
    audio::AudioOutDriverManager::instance().start();
    audio::initialize();

    // i2c test
    Wire.begin(21, 22);

    IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);
    IMU.initMPU9250();
    IMU.initAK8963(IMU.magCalibration);

    auto moduleID = M5DX::readModuleID();
    DBOUT(("module id = %d\n", (int)moduleID));

    // bt

    if (!io::initializeBluetooth() || !io::BLEManager::instance().initialize())
    {
        DBOUT(("bluetooth initialize error."));
    }
    io::setBluetoothDeviceName("M5DX");

    static io::MidiMessageQueue midiIn;
    io::BLEMidiClient::instance().setMIDIIn(&midiIn);
    io::BLEManager::instance().registerClientProfile(
        io::BLEMidiClient::instance());

    //    io::BLEManager::instance().removeAllBondedDevices();
    // io::BLEManager::instance().startScan();

    //    io::removeAllBondedClassicBTDevices();
    // io::dumpBondedClassicBTDevices();
    // io::BTA2DPSourceManager a2dpManager_;
    // a2dpManager_.initialize(&jobManager_);
    // a2dpManager_.start();

    //

    M5DX::initialize();

    ///

    //    target::setupBus();
    //    target::restoreBus();
    // return;

#if 0
    sys::initTimer(1000000);
    sys::setTimerPeriod(5376, true);
    sys::startTimer();
    sys::enableTimerInterrupt();
    return;
#endif

#if 0
    auto mdxFilename = "/mdx/Arsys/Knight Arms/KNA08.MDX";

    if (mdxPlayer.loadMDX(mdxFilename))
    {
        DBOUT(("load success.\n"));
    }

    //
    mdxPlayer.start();
    mdxPlayer.play(-1);
#endif
}

void
testIMU()
{
    if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
    {
        IMU.readAccelData(IMU.accelCount);
        IMU.getAres();

        IMU.ax = (float)IMU.accelCount[0] * IMU.aRes; // - accelBias[0];
        IMU.ay = (float)IMU.accelCount[1] * IMU.aRes; // - accelBias[1];
        IMU.az = (float)IMU.accelCount[2] * IMU.aRes; // - accelBias[2];

        IMU.readGyroData(IMU.gyroCount); // Read the x/y/z adc values
        IMU.getGres();

        IMU.gx = (float)IMU.gyroCount[0] * IMU.gRes;
        IMU.gy = (float)IMU.gyroCount[1] * IMU.gRes;
        IMU.gz = (float)IMU.gyroCount[2] * IMU.gRes;

        IMU.readMagData(IMU.magCount); // Read the x/y/z adc values
        IMU.getMres();

        IMU.mx = (float)IMU.magCount[0] * IMU.mRes * IMU.magCalibration[0] -
                 IMU.magbias[0];
        IMU.my = (float)IMU.magCount[1] * IMU.mRes * IMU.magCalibration[1] -
                 IMU.magbias[1];
        IMU.mz = (float)IMU.magCount[2] * IMU.mRes * IMU.magCalibration[2] -
                 IMU.magbias[2];

        IMU.tempCount   = IMU.readTempData(); // Read the adc values
        IMU.temperature = ((float)IMU.tempCount) / 333.87 + 21.0;

        DBOUT(("(%d, %d, %d)mg (%d, %d, %d)o/s (%d, %d, %d)mG %fdeg\n",
               (int)(IMU.ax * 1000),
               (int)(IMU.ay * 1000),
               (int)(IMU.az * 1000),
               (int)(IMU.gx),
               (int)(IMU.gy),
               (int)(IMU.gz),
               (int)(IMU.mx),
               (int)(IMU.my),
               (int)(IMU.mz),
               IMU.temperature));
    }
}

void
testPlus()
{
    Wire.requestFrom(0x62, 2);
    while (Wire.available())
    {
        auto ct    = Wire.read();
        auto press = Wire.read();
        DBOUT(("count %d, pressed %d\n", ct, press));
    }
}

// The loop routine runs over and over again forever
void
loop()
{
    M5DX::tick();

    //    testIMU();
    //    testPlus();

    static int counter = 0;
    ++counter;

    if ((counter % 100) == 0 && 0)
    {
        static uint8_t note = 0x40;
        {
            io::MidiMessage m(0x80, note & 0x7f, 0);
            io::BLEMidiClient::instance().put(m);
        }
        ++note;
        {
            io::MidiMessage m(0x90, note & 0x7f, 0x4f);
            io::BLEMidiClient::instance().put(m);
        }
    }

#if 1
    waveViewBuffer_.fill(waveViewBuffer_.makeColor(0, 0, 128));
    auto red   = waveViewBuffer_.makeColor(255, 0, 0);
    auto green = waveViewBuffer_.makeColor(0, 255, 0);

    audio::AudioOutDriverManager::instance().lockHistoryBuffer();
    static std::array<int16_t, 2> tmpWave[128];
    audio::AudioOutDriverManager::instance().getHistoryBuffer().copyLatest(
        tmpWave, 128);
    audio::AudioOutDriverManager::instance().unlockHistoryBuffer();

    auto* wave = tmpWave;
    for (int i = 0; i < 128; ++i)
    {
        int y0 = std::min(127, std::max(0, 64 + (int)((*wave)[0] >> 9)));
        int y1 = std::min(127, std::max(0, 64 + (int)((*wave)[1] >> 9)));
        waveViewBuffer_.setPixel(i, y0, red);
        waveViewBuffer_.setPixel(i, y1, green);
        ++wave;
    }

    graphics::getDisplay().setWindow(0, 0, 320, 240);
    graphics::getDisplay().blit(waveViewBuffer_, 320 - 128, 240 - 128);
#endif

#if 0
    static int pixelNumber = 0; // = random(0, M5STACK_FIRE_NEO_NUM_LEDS - 1);
    pixelNumber++;
    if (pixelNumber > 9)
        pixelNumber = 0;
    int r = 1 << random(0, 7);
    int g = 1 << random(0, 7);
    int b = 1 << random(0, 7);

    pixels.setPixelColor(pixelNumber, pixels.Color(r, g, b));
    pixels.show();
#endif

    delay(1);
}

#if 0
// The arduino task
void
loopTask(void* pvParameters)
{
    setup();
    for (;;)
    {
        micros(); // update overflow
        loop();
    }
}
#endif

extern "C" void
app_main()
{
#if 0
    initArduino();
    xTaskCreatePinnedToCore(
        loopTask, "loopTask", 8192, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
#else
    setup();
    while (1)
    {
        loop();
    }

#endif
}
