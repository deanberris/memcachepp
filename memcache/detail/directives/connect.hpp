
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DIRECTIVES_CONNECT_HPP__
#define __MEMCACHE_DETAIL_DIRECTIVES_CONNECT_HPP__

namespace memcache { namespace detail {
    
    template <typename tag>
    struct connect_directive {
        template <typename T>
        void operator() (T & handle) const {
            handle.connect();
        };
    };

}; // namespace detail

}; // namespace memcache

#endif // __MEMCACHE_DETAIL_DIRECTIVES_CONNECT_HPP__

