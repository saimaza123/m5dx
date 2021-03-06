/*
 * author : Shuichi TAKANO
 * since  : Mon Feb 11 2019 18:51:14
 */

#include "file_window.h"
#include "context.h"
#include "strings.h"
#include <debug.h>
#include <mutex>

#include <music_player/music_player_manager.h>

namespace ui
{

FileWindow::FileWindow(const std::string& path)
{
    setChild(&list_);

    {
        auto p   = path.rfind('/');
        auto dir = p == std::string::npos || p == 0 ? "/" : path.substr(0, p);
        auto fn  = p == std::string::npos ? "" : path.substr(p + 1);
        list_.setFollowFile(fn);
        setPath(dir);
    }

    list_.setLongPressFunc([this](UpdateContext& ctx, int) {
        if (list_.getPath() == "/")
        {
            ctx.popManagedUI();
            return;
        }
        auto pathSet = list_.getSeparatePath();
        list_.setFollowFile(pathSet.second);
        setPath(pathSet.first);
    });

    list_.setDecideText(get(strings::select));
    list_.setDecideFunc([this](UpdateContext& ctx, int i) {
        DBOUT(("decide index %d\n", i));

        list_.getMutex().lock();
        auto item  = list_.getItem(i);
        auto& name = item.first;
        auto fn    = list_.makeAbsPath(name);
        list_.getMutex().unlock();

        if (item.first.empty())
        {
            return;
        }

        if (item.second)
        {
            // dir
            DBOUT(("select directory %s\n", name.c_str()));
            if (name == "..")
            {
                auto pathSet = list_.getSeparatePath();
                fn           = pathSet.first;
                list_.setFollowFile(pathSet.second);
            }
            else
            {
                list_.setFollowFile("");
            }
            setPath(fn);
        }
        else
        {
            // file
            DBOUT(("select file %s\n", name.c_str()));
            // list_.cancelAndWaitIdle();
            // music_player::playMusicFile(fn.c_str(), 0, true);

            music_player::playPlayList(list_.makeDefaultPlayList());
            ctx.popManagedUI();
        }
    });
}

void
FileWindow::setPath(const std::string& path)
{
    {
        std::lock_guard<sys::Mutex> lock(list_.getMutex());

        if (path == "/" || path == "")
        {
            setTitle(get(strings::loadFile));
        }
        else
        {
            setTitle(path);
        }
    }

    list_.reset();
    list_.setPath(path);
    refresh();
}

} // namespace ui
