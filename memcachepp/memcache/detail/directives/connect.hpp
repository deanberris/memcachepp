
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DIRECTIVES_CONNECT_HPP__
#define __MEMCACHE_DETAIL_DIRECTIVES_CONNECT_HPP__

namespace memcache { 

	namespace detail {
		struct connect_directive;
	}

	detail::connect_directive connect(boost::uint64_t timeout);

	namespace detail {
		struct connect_directive {
			connect_directive(boost::uint64_t timeout) : timeout_(timeout) { };
            
            template <class Handle>
            void operator()(Handle & handle) const {
                handle.connect( timeout_ );
            }
            
        private:
            boost::uint64_t timeout_;
		};
	}
    
    typedef detail::connect_directive (*connect_directive_t)(boost::uint64_t);

	inline detail::connect_directive connect(boost::uint64_t timeout = MEMCACHE_TIMEOUT) {
		return detail::connect_directive(timeout);
	}

} // namespace memcache

#endif // __MEMCACHE_DETAIL_DIRECTIVES_CONNECT_HPP__

