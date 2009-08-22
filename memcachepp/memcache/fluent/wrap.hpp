
// Copyright 2008 (c) Friendster, Inc.
// Copyright 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
  * wrap.hpp
  *
  * This tool allows for introducing the assignability
  * property to an object reference. This utility is
  * used to provide a wrapper class that contains a
  * custom assignment operator, which invokes the
  * right hand side directive.
  *
  * @author Dean Michael Berris <dmberris@friendster.com>
  */

#ifndef MEMCACHEPP_FLUENT_WRAP_20080220
#define MEMCACHEPP_FLUENT_WRAP_20080220

namespace memcache {

    namespace fluent {

        template <typename ContainedType>
            struct wrap_impl {
                explicit wrap_impl(ContainedType & container)
                    : _container(container) { };

                // assignment operator
                template <typename Directive>
                    wrap_impl const &
                    operator=(Directive const & directive) const {
                        directive(_container);
                        return *this;
                    };

                // conversion operator to return an lvalue reference
                operator ContainedType& () {
                    return _container;
                };

                // conversion operator to return a const object reference
                operator ContainedType const & () {
                    return _container;
                };
                
                private:
                    ContainedType & _container;
            };

        template <typename ContainedType>
            inline wrap_impl<ContainedType>
            wrap(ContainedType & contained) {
                return wrap_impl<ContainedType>(contained);
            };

    }; // namespace fluent

}; // namespace memcache

#endif // MEMCACHEPP_FLUENT_WRAP_20080220

