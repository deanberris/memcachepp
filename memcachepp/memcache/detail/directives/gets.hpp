
// Copyright 2010 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_GET_HPP_20100331
#define MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_GET_HPP_20100331

#include <boost/cstdint.hpp>

namespace memcache { namespace detail {
    
    template <class T>
    struct gets_directive {
        explicit gets_directive(std::string const & key, T & holder, boost::uint64_t & cas_value)
        : key_(key), holder_(holder), cas_value_(cas_value)
        {}

        template <class _T>
        void operator() (_T & handle) const {
            size_t pools = handle.pool_count();
            assert(pools != 0);
            handle.gets(
                handle.hash(
                    key_,
                    pools
                    ),
                key_,
                holder_,
                cas_value_
                );
        }

        private:

        std::string const & key_;
        T & holder_;
        boost::uint64_t & cas_value_;
    };

} /* detail */

template <class _T>
inline detail::gets_directive<_T>
gets(std::string const & key, _T & holder, boost::uint64_t & cas_value) {
    return detail::gets_directive<_T>(key, holder, cas_value);
}

} /* memcache */

#endif /* MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_GET_HPP_20100331 */
