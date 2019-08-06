#pragma once

#include <ziopp/upath.h>

namespace ziopp {
    class filesystem;

    class filesystem_watcher {
    public:
        virtual const filesystem& filesystem() const = 0;
        virtual const upath& path() const = 0;

        virtual bool include_subdirectories() const = 0;
        virtual void include_subdirectories(bool value) = 0;
    };
}