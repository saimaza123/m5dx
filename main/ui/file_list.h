/*
 * author : Shuichi TAKANO
 * since  : Sat Feb 09 2019 22:50:40
 */
#ifndef _07D526DB_9134_13FE_1576_3EA1586E6814
#define _07D526DB_9134_13FE_1576_3EA1586E6814

#include "scroll_list.h"
#include "widget.h"
#include <music_player/file_format.h>
#include <string>
#include <sys/mutex.h>
#include <utility>
#include <vector>

namespace ui
{

class FileList : public ScrollList
{
    using super = ScrollList;

    struct Item : public Widget
    {
        bool updated_  = true;
        bool selected_ = false;

    public:
        Dim2 getSize() const override;
        void onRender(RenderContext& ctx) override;
        void touch() override { updated_ = true; }

        virtual void _render(RenderContext& ctx) = 0;
    };

    struct File : public Item
    {
        std::string filename_;
        std::string title_;
        size_t size_ = 0;
        music_player::FileFormat format_{};

    public:
        File(std::string&& filename, size_t size, music_player::FileFormat fmt)
            : filename_(std::move(filename))
            , size_(size)
            , format_(fmt)
        {
        }

        void onUpdate(UpdateContext& ctx) override {}
        void _render(RenderContext& ctx) override;
    };

    struct Directory : public Item
    {
        std::string name_;

    public:
        Directory(std::string&& name) { name_ = std::move(name); }
        void onUpdate(UpdateContext& ctx) override {}
        void _render(RenderContext& ctx) override;
    };

    sys::Mutex mutex_;

    std::vector<Directory> directories_;
    std::vector<File> files_;

    std::string path_;

    size_t parseIndex_ = 0;

public:
    FileList();

    void setPath(const std::string& path);
    std::pair<std::string, bool> getItem(size_t i);
    const std::string& getPath() const { return path_; }
    std::string makeAbsPath(const std::string& name) const;

    Dim2 getSize() const override;

    uint16_t getBaseItemSize() const override;

    size_t getWidgetCount() const override;
    const Widget* getWidget(size_t i) const override;
    Widget* getWidget(size_t i) override;
    void onUpdate(UpdateContext& ctx) override;

    sys::Mutex& getMutex() { return mutex_; }

protected:
    Widget* _getWidget(size_t i);
}; // namespace ui

} // namespace ui

#endif /* _07D526DB_9134_13FE_1576_3EA1586E6814 */
