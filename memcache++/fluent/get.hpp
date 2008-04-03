
// Copyright 2008 (c) Friendster, Inc.
// Copyright 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
  * get.hpp
  *
  * This is the fluent implementation of the get directive.
  * The intention is to be able to provide an assignment
  * operator, and a get directive that takes the memcache
  * handle as the first argument, and the key given as the
  * second argument.
  *
  * @author Dean Michael Berris <dmberris@friendster.com>
  */

#ifndef MEMCACHEPP_FLUENT_GET_20080220
#define MEMCACHEPP_FLUENT_GET_20080220

namespace memcache { 
    
    namespace fluent { 

        template <typename Handle>
        struct get_impl {
            get_impl(Handle & handle, std::string const & key)
                : _handle(handle), _key(key) { };

            template <typename Container>
                void operator() (Container & container) const {
                    _handle << ::memcache::get(_key, container);
                };
            
            private:
                Handle & _handle;
                mutable std::string _key;
        };

        template <typename Handle>
            inline get_impl<Handle>
            get(Handle & handle, std::string const & key) {
                return get_impl<Handle>(handle, key);
            };

    }; // namespace fluent

}; // namespace memcache

#endif // MEMCACHEPP_FLUENT_GET_20080220
