
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DIRECTIVES_DELETE_HPP__
#define __MEMCACHE_DETAIL_DIRECTIVES_DELETE_HPP__

namespace memcache { namespace detail {

        template <typename T>
        struct delete_directive {
            explicit delete_directive(std::string const & key) :
                _key(key) { };

            template <typename _T>
            void operator() (_T & handle) const {
                handle.delete_(
                        handle.hash(
                            _key, 
                            handle.pool_count()
                            ), 
                        _key
                        );
            };

            private:

            mutable std::string _key;
        };

}; // namespace detail

    template <typename T>
    inline detail::delete_directive<T> delete_(T _key) {
        return detail::delete_directive<T>(std::string(_key));
    };

}; // namespace memcache

#endif // __MEMCACHE_DETAIL_DIRECTIVES_DELETE_HPP__

