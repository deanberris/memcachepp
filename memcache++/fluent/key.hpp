
// Copyright 2008 (c) Friendster, Inc.
// Copyright 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
  * key.hpp
  *
  * This wraps the handle and key and additional information
  * about the 'setting' of the data -- like flags, expiration,
  * expiration on failure -- in a class, whose assignment operator
  * effectively sets the data using the appropriate set directive.
  *
  * @author Dean Michael Berris <dmberris@friendster.com>
  */

#ifndef MEMCACHEPP_FLUENT_KEY_20080220
#define MEMCACHEPP_FLUENT_KEY_20080220

namespace memcache {

    namespace fluent {

        template <typename Handle>
            struct key_impl {

                explicit key_impl
                    (
                     Handle & handle, 
                     std::string const & key
                    )
                    :
                _handle(handle),
                _key(key)
                {
                    _expiration.timeout = 0;
                    _failover_expiration.timeout = 0;
                };

                explicit key_impl
                    (
                     Handle & handle,
                     std::string const & key,
                     detail::expire_type const & expiration
                    )
                    :
                _handle(handle),
                _key(key),
                _expiration(expiration)
                {
                    _failover_expiration.timeout = 0;
                };

                explicit key_impl
                    (
                     Handle & handle,
                     std::string const & key,
                     detail::failover_expire_type const & failover_expiration
                    )
                    :
                _handle(handle),
                _key(key),
                _expiration(),
                _failover_expiration(failover_expiration)
                {
                    _expiration.timeout = 0;
                };

                explicit key_impl
                    (
                     Handle & handle,
                     std::string const & key,
                     detail::expire_type const & expiration,
                     detail::failover_expire_type const & failover_expiration
                    )
                    :
                _handle(handle),
                _key(key),
                _expiration(expiration),
                _failover_expiration(failover_expiration)
                { };

                explicit key_impl
                    (
                     Handle & handle,
                     std::string const & key,
                     detail::failover_expire_type const & failover_expiration,
                     detail::expire_type const & expiration
                    )
                    :
                _handle(handle),
                _key(key),
                _expiration(expiration),
                _failover_expiration(failover_expiration)
                { };

                template <typename DataType>
                    key_impl const &
                    operator=(DataType const & data) const {
                        _handle << 
                            ::memcache::set(_key, data, _expiration, _failover_expiration);
                        return *this;
                    };
                
                private:
                    Handle & _handle;
                    mutable std::string _key;
                    mutable detail::expire_type _expiration;
                    mutable detail::failover_expire_type _failover_expiration;
            };

        template <typename Handle>
            inline key_impl<Handle>
            key(Handle & handle, std::string const & key) {
                return key_impl<Handle>(handle, key);
            };

    }; // namespace fluent

}; // namespace memcache

#endif // MEMCACHEPP_FLUENT_KEY_20080220

