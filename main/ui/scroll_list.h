/*
 * author : Shuichi TAKANO
 * since  : Sat Feb 09 2019 17:25:31
 */
#ifndef A99E7AA1_6134_13FE_1074_0DEB31AD1CE5
#define A99E7AA1_6134_13FE_1074_0DEB31AD1CE5

#include "widget.h"
#include "widget_list.h"

namespace ui
{

class ScrollList : public Widget, public WidgetList
{
    bool vertical_     = true;
    int displayOffset_ = 0;
    int selectIndex_   = 0;

public:
    void setDirectionIsVertical(bool v) { vertical_ = v; }
    // スクロール方向のWidgetサイズを取得
    virtual uint16_t getBaseItemSize() const = 0;

    void onUpdate(UpdateContext& ctx) override;
    void onRender(RenderContext& ctx) override;
};

} // namespace ui

#endif /* A99E7AA1_6134_13FE_1074_0DEB31AD1CE5 */
