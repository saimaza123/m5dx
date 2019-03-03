/*
 * author : Shuichi TAKANO
 * since  : Sun Mar 03 2019 2:26:6
 */

#include "player_window.h"
#include "button_tip.h"
#include "context.h"
#include "file_window.h"
#include "key.h"
#include "strings.h"
#include "system_setting.h"
#include "ui_manager.h"
#include <algorithm>
#include <audio/audio_out.h>
#include <music_player/music_player_manager.h>
#include <mutex>
#include <sys/mutex.h>

namespace ui
{

namespace
{
constexpr Dim2 widgetSize_ = {WindowSettings::SCREEN_WIDTH,
                              WindowSettings::SCREEN_HEIGHT};
}

void
PlayerWindow::onUpdate(UpdateContext& ctx)
{
    auto* bt        = ctx.getButtonTip();
    auto* uiManager = ctx.getUIManager();
    auto* key       = ctx.getKeyState();
    if (key && bt && uiManager && ctx.isEnableInput())
    {
        ctx.disableInput();
        std::lock_guard<sys::Mutex> lock(music_player::getMutex());

        auto* mp       = music_player::getActiveMusicPlayer();
        bool playing   = mp && !mp->isFinished() && !mp->isPaused();
        bool longLeft  = key->isLongPress(0);
        bool longRight = key->isLongPress(2);

        longLeftCaptured_ =
            longLeft && (longLeftCaptured_ || !longRightCaptured_);
        longRightCaptured_ =
            longRight && (longRightCaptured_ || !longLeftCaptured_);

        auto& ss = SystemSettings::instance();

        if (longLeftCaptured_)
        {
            bt->set(0, get(strings::volumeAdj));
            bt->set(1, get(strings::volDown));
            bt->set(2, get(strings::volUp));

            auto v                      = ss.getVolume();
            static constexpr int minVol = -30;
            static constexpr int maxVol = 6;
            if (key->isTrigger(1))
            {
                v = std::max(v - 1, minVol);
                ss.setVolume(v);
            }
            if (key->isTrigger(2))
            {
                v = std::min(v + 1, maxVol);
                ss.setVolume(v);
            }
        }
        else if (longRightCaptured_)
        {
            bt->set(0, get(strings::prev));
            bt->set(1, get(strings::next));
            bt->set(2, get(strings::selectSong));
        }
        else if (playing)
        {
            bt->set(0, get(strings::settings));
            bt->set(1, get(strings::stop));
            bt->set(2, get(strings::next));

            if (key->isReleaseEdge(1))
            {
                if (mp)
                {
                    mp->stop();
                }
            }
        }
        else
        {
            bt->set(0, get(strings::settings));
            bt->set(1, get(strings::play));
            bt->set(2, get(strings::selectFile));

            if (key->isReleaseEdge(1))
            {
                if (mp)
                {
                    mp->play();
                }
            }
            if (key->isReleaseEdge(2))
            {
                uiManager->push(std::make_shared<ui::FileWindow>("/"));
            }
        }
    }
}

void
PlayerWindow::onRender(RenderContext& ctx)
{
    if (needRefresh_ || ctx.isInvalidated(widgetSize_))
    {
        ctx.applyClipRegion();
        ctx.updateInvalidatedRegion(getSize());

        ctx.fill({0, 0}, widgetSize_, 0x400000);
        needRefresh_ = false;
    }

    auto& fm = ctx.getFontManager();
    fm.setEdgedMode(false);
    fm.setTransparentMode(false);

    auto& ss = SystemSettings::instance();
    char buf[100];
    snprintf(buf, sizeof(buf), "vol %d ", ss.getVolume());
    ctx.setFontColor(0xffffff);
    ctx.putText(buf, {128, 0}, {100, 8});

    auto& tmpFB = ctx.getTemporaryFrameBuffer({128, 128});
    {
        auto recoverFB = ctx.updateFrameBuffer(&tmpFB);
        ctx.applyClipRegion();
        tmpFB.fill(tmpFB.makeColor(0x0, 0, 128));
        auto red   = tmpFB.makeColor(255, 0, 0);
        auto green = tmpFB.makeColor(0, 255, 0);

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
            tmpFB.setPixel(i, y0, red);
            tmpFB.setPixel(i, y1, green);
            ++wave;
        }
    }
    ctx.applyClipRegion();
    ctx.put({0, 0}, tmpFB);
}

Dim2
PlayerWindow::getSize() const
{
    return widgetSize_;
}

} // namespace ui
