/*
 * author : Shuichi TAKANO
 * since  : Sat Feb 09 2019 17:25:31
 */
#ifndef A99E7AA1_6134_13FE_1074_0DEB31AD1CE5
#define A99E7AA1_6134_13FE_1074_0DEB31AD1CE5

#include "widget.h"
#include "widget_list.h"
#include <functional>
#include <string>
#include <system/mutex.h>

namespace ui
{

class ScrollList : public Widget, public WidgetList
{
    bool vertical_             = true;
    int displayOffset_         = 0;
    int selectIndex_           = 0;
    bool needRefresh_          = true;
    bool needRefreshScrollBar_ = true;

    using Func = std::function<void(UpdateContext& ctx, int)>;

    Func decideFunc_;
    Func longPressFunc_;

    std::string decideText_;

    sys::Mutex mutex_;

public:
    ScrollList();
    void setDirectionIsVertical(bool v) { vertical_ = v; }

    // スクロール方向のWidgetサイズを取得
    virtual uint16_t getBaseItemSize() const = 0;

    void onUpdate(UpdateContext& ctx) override;
    void onRender(RenderContext& ctx) override;

    void setDecideFunc(Func&& f) { decideFunc_ = std::move(f); }
    void setLongPressFunc(Func&& f) { longPressFunc_ = std::move(f); }
    void setDecideText(const std::string& t) { decideText_ = t; }

    int getIndex() const { return selectIndex_; }
    void setIndex(int i);

    void refresh() { needRefresh_ = true; }
    void reset()
    {
        displayOffset_ = 0;
        selectIndex_   = 0;
        needRefresh_   = true;
    }

    void listInserted(int i);

    sys::Mutex& getMutex() { return mutex_; }

protected:
    virtual void selectChanged() {}
    void touchSelectWidget();
};

} // namespace ui

#endif /* A99E7AA1_6134_13FE_1074_0DEB31AD1CE5 */
