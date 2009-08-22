#ifndef MEMCACHEPP_MEMCACHE_FLUENT_REMOVE_HPP_
#define MEMCACHEPP_MEMCACHE_FLUENT_REMOVE_HPP_

// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
  * remove.hpp
  *
  * This is the fluent implementation of the delete_ directive.
  * The intention is to be able to prove a simple delete_ function
  * call that provided the key as the second argument will delete
  * the key from memcache.
  *
  * @author Dean Michael Berris <mikhailberis@gmail.com>
  */

namespace memcache {

    template <class Handle>
        inline void
        remove(Handle & mc, std::string const & key) {
            mc << memcache::delete_(key);
        }

} // namespace memcache

#endif

