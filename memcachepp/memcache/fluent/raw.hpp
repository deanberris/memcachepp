// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
  * raw.hpp
  *
  * This is the fluent implementation of the raw directive.
  * The intention is to be able to provide a mod-assignment
  * operator, and a raw directive that takes the memcache
  * handle as the first argument, and the key given as the
  * second argument.
  *
  * @author Dean Michael Berris <dmberris@friendster.com>
  */

#ifndef MEMCACHEPP_FLUENT_RAW_20091212
#define MEMCACHEPP_FLUENT_RAW_20091212

#include <string>

namespace memcache { namespace fluent {

    using std::string;

    template <class Handle>
    struct raw_impl {
        raw_impl(Handle & handle, std::string const & key)
        : handle_(handle), key_(key) { };

        void operator() (string & container) const {
            handle_ << ::memcache::raw_get(key_, container);
        }

        private:

        Handle & handle_;
        std::string const & key_;
    };

    template <class Handle>
    inline raw_impl<Handle>
    raw(Handle & handle, string const & key) {
        return raw_impl<Handle>(handle, key);
    }

} // namespace fluent

} // namespace memcache

#endif // MEMCACHEPP_FLUENT_RAW_20091212

