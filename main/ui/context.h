/*
 * author : Shuichi TAKANO
 * since  : Mon Jan 28 2019 3:22:8
 */
#ifndef _5E8DAF43_4134_1395_3126_2D4BFDB63CCC
#define _5E8DAF43_4134_1395_3126_2D4BFDB63CCC

#include "text_align.h"
#include "types.h"
#include "window_setting.h"
#include <graphics/font_manager.h>
#include <graphics/framebuffer.h>

namespace graphics
{
class FrameBufferBase;
class Texture;
} // namespace graphics

namespace ui
{

class KeyState;
class ButtonTip;
class UIManager;

class Context
{
    Vec2 curPos_{0, 0};
    BBox curClipRegion_ = {Vec2{0, 0}, Dim2{320, 240}};
    int curIndex_{};
    int selectIndex_{};
    graphics::FontManager fontManager_;

protected:
    template <class CTX, class T, void (CTX::*F)(const T&)>
    struct ScopeRestoreParam
    {
        CTX* ctx_;
        T p_;

        ScopeRestoreParam(CTX* ctx, const T& p)
            : ctx_(ctx)
            , p_(p)
        {
        }
        ~ScopeRestoreParam()
        {
            if (ctx_)
            {
                (ctx_->*F)(p_);
            }
        }
        ScopeRestoreParam(ScopeRestoreParam&& s)
        {
            ctx_   = s.ctx_;
            p_     = s.p_;
            s.ctx_ = nullptr;
        }
    };

public:
    const Vec2& getCurrentPosition() const { return curPos_; }
    void setCurrentPosition(const Vec2& p) { curPos_ = p; }

    const BBox& getCurrentClipRegion() const { return curClipRegion_; }
    void setCurrentClipRegion(const BBox& b) { curClipRegion_ = b; }

    int getCurrentIndex() const { return curIndex_; }
    void setCurrentIndex(const int& i) { curIndex_ = i; }

    int getSelectIndex() const { return selectIndex_; }
    void setSelectIndex(const int& i) { selectIndex_ = i; }

    graphics::FontManager& getFontManager() { return fontManager_; }
    void setFontRegion(const Vec2& pos, const Dim2& size);

protected:
    using ScopeRestorePos =
        ScopeRestoreParam<Context, Vec2, &Context::setCurrentPosition>;
    using ScopeRestoreClip =
        ScopeRestoreParam<Context, BBox, &Context::setCurrentClipRegion>;
    using ScopeRestoreIndex =
        ScopeRestoreParam<Context, int, &Context::setCurrentIndex>;
    using ScopeRestoreSelectIndex =
        ScopeRestoreParam<Context, int, &Context::setSelectIndex>;

public:
    ScopeRestorePos updatePosition(const Vec2& p)
    {
        auto prev = curPos_;
        curPos_ += p;
        return {this, prev};
    }

    ScopeRestoreClip updateClipRegion(const Dim2& s)
    {
        auto prev = curClipRegion_;
        curClipRegion_.intersect(BBox(curPos_, s));
        return {this, prev};
    }

    ScopeRestoreIndex updateIndex(int i)
    {
        auto prev = curIndex_;
        setCurrentIndex(i);
        return {this, prev};
    }
    ScopeRestoreIndex makeRestoreIndex() { return {this, curIndex_}; }

    ScopeRestoreSelectIndex updateSelectIndex(int i)
    {
        auto prev    = selectIndex_;
        selectIndex_ = i;
        return {this, prev};
    }
};

class UpdateContext : public Context
{
    UIManager* uiManager_{};
    KeyState* keyState_{};
    ButtonTip* buttonTip_{};
    bool enableInput_ = true;
    float dt_         = 0;

public:
    UpdateContext(float dt, UIManager* uiManager, KeyState* ks, ButtonTip* bt)
        : uiManager_(uiManager)
        , keyState_(ks)
        , buttonTip_(bt)
        , dt_(dt)
    {
    }

    float getDeltaT() const { return dt_; }

    UIManager* getUIManager() { return uiManager_; }
    void popManagedUI(int n = 1);

    void setKeyState(KeyState* ks) { keyState_ = ks; }
    const KeyState* getKeyState() const
    {
        return enableInput_ ? keyState_ : nullptr;
    }
    const KeyState* _getKeyState() const { return keyState_; }

    ButtonTip* getButtonTip() { return buttonTip_; }

    bool isEnableInput() const { return enableInput_; }
    void disableInput() { enableInput_ = false; }
    void acceptLongPress();
};

class RenderContext : public Context
{
    BBox invalidatedRegion_;
    graphics::FrameBufferBase* frameBuffer_{};
    const graphics::Texture* texture_{};

    WindowSettings windowSettings_;

    graphics::FrameBuffer temporaryFrameBuffer_;

public:
    RenderContext();

    const BBox& getInvalidatedRegion() const { return invalidatedRegion_; }
    void updateInvalidatedRegion(const Dim2& size);

    const WindowSettings& getWindowSettings() const { return windowSettings_; }

    graphics::FrameBufferBase* getFrameBuffer() { return frameBuffer_; }
    void setFrameBuffer(graphics::FrameBufferBase* b);

    const graphics::Texture* getTexture() const { return texture_; }
    void setTexture(const graphics::Texture* t) { texture_ = t; }

    graphics::FrameBuffer&
    getTemporaryFrameBuffer(uint32_t w, uint32_t h, int bpp = 16);

    graphics::FrameBuffer& getTemporaryFrameBuffer(const Dim2& size,
                                                   int bpp = 16);

    // utility

    // { currentPos, size } の Rectが無効化領域にかかっているか調べる
    bool isInvalidated(const Dim2& size) const;
    bool isInvalidated(const Vec2& pos, const Dim2& size) const;

    void applyClipRegion();
    uint32_t makeColor(uint32_t c) const;
    void setFontColor(uint32_t c);
    void setFontBGColor(uint32_t c);

    void putText(const char* str,
                 Vec2 pos,
                 Dim2 size,
                 TextAlignH alignH = TextAlignH::LEFT,
                 TextAlignV alignV = TextAlignV::TOP);

    void putText(const char* str,
                 const Rect& rect,
                 TextAlignH alignH = TextAlignH::LEFT,
                 TextAlignV alignV = TextAlignV::TOP)
    {
        putText(str, rect.pos, rect.size, alignH, alignV);
    }

    void drawBits(Vec2 pos, int w, int h, const uint8_t* bits, uint32_t color);

    void fill(Vec2 pos, const Dim2& size, uint32_t color);
    void fill(const Rect& rect, uint32_t color)
    {
        fill(rect.pos, rect.size, color);
    }
    void drawRect(Vec2 pos, const Dim2& size, uint32_t color);
    void drawRect(const Rect& rect, uint32_t color)
    {
        drawRect(rect.pos, rect.size, color);
    }

    void put(Vec2 pos, const graphics::FrameBufferBase& fb);
    void put(Vec2 pos, const graphics::FrameBufferBase& fb, const BBox& bb);

    void putTexture(Vec2 pos, const Rect& src);
    void putTextureTrans(Vec2 pos, const Rect& src);
    void putTextureText(const char* text,
                        int charOfs,
                        Vec2 pos,
                        const Rect& font,
                        uint32_t color,
                        uint32_t bg);

protected:
    struct FBRestoreState
    {
        Vec2 pos_;
        BBox clipRegion_;
        graphics::FrameBufferBase* fb_;
    };

    void setFBRestoreState(const FBRestoreState& s)
    {
        setCurrentPosition(s.pos_);
        setCurrentClipRegion(s.clipRegion_);
        setFrameBuffer(s.fb_);
    }

    using ScopeRestoreFrameBuffer =
        ScopeRestoreParam<RenderContext,
                          FBRestoreState,
                          &RenderContext::setFBRestoreState>;

public:
    ScopeRestoreFrameBuffer updateFrameBuffer(graphics::FrameBufferBase* b);
};

// キー入力をとるwidgetを一つだけ選択できる
// アクティブなroot widgetを１つ選択、その下だけ描画される
// Widgetの重なりは描画領域は狭める範囲でしか指定できない

} // namespace ui

#endif /* _5E8DAF43_4134_1395_3126_2D4BFDB63CCC */
