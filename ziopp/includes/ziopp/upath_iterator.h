#pragma once

#include <functional>
#include <ziopp/upath.h>

#ifndef _NODISCARD
#if __cplusplus >= 201703L
#define _NODISCARD [[nodiscard]]
#else
#define _NODISCARD
#endif
#endif

namespace ziopp {
    class upath_iterator {
    public:
        using value_type = upath;
        using difference_type = ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        upath_iterator() = default;
        upath_iterator(const upath_iterator&) = default;
        upath_iterator(upath_iterator&&) = default;
        upath_iterator& operator=(const upath_iterator&) = default;
        upath_iterator& operator=(upath_iterator&&) = default;

        _NODISCARD reference operator*() const noexcept;
        _NODISCARD pointer operator->() const noexcept;

        upath_iterator& operator++();
        upath_iterator operator++(int);

        upath_iterator& operator--();
        upath_iterator operator--(int);

        friend bool operator==(const upath_iterator& lhs, const upath_iterator& rhs);
        friend bool operator!=(const upath_iterator& lhs, const upath_iterator& rhs);

    private:
        upath_iterator(const std::function<pointer()>& begin, const std::function<pointer()>& end);
        const std::function<pointer()>& _begin;
        const std::function<pointer()>& _end;
    };
}