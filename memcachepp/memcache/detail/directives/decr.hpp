#ifndef MEMCACHE_DETAIL_DIRECTIVES_DECR_HPP_
#define MEMCACHE_DETAIL_DIRECTIVES_DECR_HPP_

// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

namespace memcache { namespace detail {

    struct decr_directive {
        explicit decr_directive(std::string const & key, boost::uint64_t & holder, boost::uint64_t value)
            : key_(key), holder_(holder), value_(value) {}

        template <class Handle>
            void operator()(Handle & handle) const {
                size_t pools = handle.pool_count();
                assert(pools != 0);
                handle.decr(
                        handle.hash(
                            key_,
                            pools
                            ),
                        key_,
                        holder_,
                        value_
                        );
            }

        private:

        std::string key_;
        boost::uint64_t & holder_;
        boost::uint64_t value_;
    };

} // namespace detail

    inline detail::decr_directive decr(
            std::string const & key,
            boost::uint64_t & holder,
            boost::uint64_t value = 1
            ) {
        return detail::decr_directive(key, holder, value);
    }

} // namespace memcache

#endif // MEMCACHE_DETAIL_DIRECTIVES_DECR_HPP_


