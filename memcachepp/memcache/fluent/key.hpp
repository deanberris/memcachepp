
// Copyright 2008 (c) Friendster, Inc.
// Copyright 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
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
                handle_(handle),
                key_(key)
                {
                    expiration_.timeout = 0;
                    failover_expiration_.timeout = 0;
                };

                explicit key_impl
                    (
                     Handle & handle,
                     std::string const & key,
                     detail::expire_type const & expiration
                    )
                    :
                handle_(handle),
                key_(key),
                expiration_(expiration)
                {
                    failover_expiration_.timeout = 0;
                };

                explicit key_impl
                    (
                     Handle & handle,
                     std::string const & key,
                     detail::failover_expire_type const & failoverexpiration_
                    )
                    :
                handle_(handle),
                key_(key),
                expiration_(),
                failover_expiration_(failoverexpiration_)
                {
                    expiration_.timeout = 0;
                };

                explicit key_impl
                    (
                     Handle & handle,
                     std::string const & key,
                     detail::expire_type const & expiration,
                     detail::failover_expire_type const & failoverexpiration_
                    )
                    :
                handle_(handle),
                key_(key),
                expiration_(expiration),
                failover_expiration_(failoverexpiration_)
                { };

                explicit key_impl
                    (
                     Handle & handle,
                     std::string const & key,
                     detail::failover_expire_type const & failoverexpiration_,
                     detail::expire_type const & expiration
                    )
                    :
                handle_(handle),
                key_(key),
                expiration_(expiration),
                failover_expiration_(failoverexpiration_)
                { };

                template <typename DataType>
                    key_impl const &
                    operator=(DataType const & data) const {
                        handle_ << 
                            ::memcache::set(key_, data, expiration_, failover_expiration_);
                        return *this;
                    };

                key_impl const &
                operator %=(string const & data) const {
                    handle_ <<
                        ::memcache::raw_set(key_, data, expiration_, failover_expiration_);
                    return *this;
                }

                template <class DataType>
                key_impl const &
                operator ^=(DataType const & data) const {
                    handle_ <<
                        ::memcache::replace(key_, data, expiration_, failover_expiration_);
                    return *this;
                }

                template <class DataType>
                key_impl const &
                operator /=(DataType const & data) const {
                    handle_ <<
                        ::memcache::add(key_, data, expiration_, failover_expiration_);
                    return *this;
                }

                key_impl const &
                operator *=(string const & data) const {
                    handle_ <<
                        ::memcache::raw_add(key_, data, expiration_, failover_expiration_);
                    return *this;
                }

                key_impl const & 
                operator >>=(string const & data) const {
                    handle_ <<
                        ::memcache::raw_append(key_, data, expiration_, failover_expiration_);
                    return *this;
                }

                key_impl const &
                operator <<=(string const & data) const {
                    handle_ <<
                        ::memcache::raw_prepend(key_, data, expiration_, failover_expiration_);
                    return *this;
                }

                key_impl const &
                operator ++(int) const {
                    handle_ <<
                        ::memcache::incr(key_, value_, 1u);
                    return *this;
                }

                key_impl const &
                operator +=(boost::uint64_t const & increment) const {
                    handle_ <<
                        ::memcache::incr(key_, value_, increment);
                    return *this;
                }

                key_impl const & 
                operator --(int) const {
                    handle_ <<
                        ::memcache::decr(key_, value_, 1u);
                    return *this;
                }

                key_impl const &
                operator -=(boost::uint64_t const & decrement) const {
                    handle_ <<
                        ::memcache::decr(key_, value_, decrement);
                    return *this;
                }

                operator boost::uint64_t const & () const {
                    return value_;
                }
                
                private:
                    Handle & handle_;
                    mutable std::string key_;
                    mutable detail::expire_type expiration_;
                    mutable detail::failover_expire_type failover_expiration_;
                    mutable boost::uint64_t value_;
            };

    }; // namespace fluent

    template <typename Handle>
        inline fluent::key_impl<Handle>
        key(Handle & handle, std::string const & key) {
            return fluent::key_impl<Handle>(handle, key);
        };

}; // namespace memcache

#endif // MEMCACHEPP_FLUENT_KEY_20080220

