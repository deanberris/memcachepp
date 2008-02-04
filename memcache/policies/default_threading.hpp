
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_POLICIES_DEFAULT_THREADING_HPP__
#define __MEMCACHE_POLICIES_DEFAULT_THREADING_HPP__

#include "tags.hpp"

namespace memcache { namespace policies {

    template <class tag = tags::default_tag>
    struct default_threading {
        default_threading() { };
        struct lock {
            explicit lock(default_threading &) { /* do nothing */ };
            ~lock() { };
        };
        protected: ~default_threading() { };
    };

}; // namespace policies

}; // namespace memcache

#endif // __MEMCACHE_POLICIES_DEFAULT_THREADING_HPP__

