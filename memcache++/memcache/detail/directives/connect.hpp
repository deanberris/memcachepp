
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DIRECTIVES_CONNECT_HPP__
#define __MEMCACHE_DETAIL_DIRECTIVES_CONNECT_HPP__

namespace memcache { 

	namespace helper {
		struct connect_directive;
	};

	helper::connect_directive connect(helper::connect_directive);

	namespace helper {
		struct connect_directive {
		private:
			connect_directive() { };
			connect_directive(const connect_directive &) { };
			friend connect_directive memcache::connect(connect_directive);
		};
	};

	typedef helper::connect_directive (*connect_directive_t)(helper::connect_directive);

	inline helper::connect_directive connect(helper::connect_directive) {
		return helper::connect_directive();
	};

}; // namespace memcache

#endif // __MEMCACHE_DETAIL_DIRECTIVES_CONNECT_HPP__

