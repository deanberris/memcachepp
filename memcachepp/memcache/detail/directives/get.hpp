
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DIRECTIVES_GET_HPP__
#define __MEMCACHE_DETAIL_DIRECTIVES_GET_HPP__

namespace memcache { namespace detail {

    template <typename T>
    struct get_directive {
        explicit get_directive(std::string const & key, T & holder) :
            _key(key), _holder(holder) { };

        template <typename _T>
        void operator() (_T & handle) const {
            size_t pools = handle.pool_count();
            assert(pools != 0);
            handle.get(
                    handle.hash(
                        _key, 
                        pools
                        ), 
                    _key, 
                    _holder
                    );
        };
        
        private:
        
        mutable std::string _key;
        T & _holder;
    };

}; // namespace detail

    template <typename _T>
    inline detail::get_directive<_T> get(std::string const & _key, _T & holder) {
        return detail::get_directive<_T>(_key, holder);
    };

}; // namespace detail

#endif // __MEMCACHE_DETAIL_DIRECTIVES_GET_HPP__

