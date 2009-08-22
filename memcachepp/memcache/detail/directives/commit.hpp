
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DIRECTIVES_COMMIT_HPP__
#define __MEMCACHE_DETAIL_DIRECTIVES_COMMIT_HPP__

namespace memcache { 

	namespace helper {
		struct commit_directive;
	};

	helper::commit_directive commit(helper::commit_directive);

	namespace helper {
		struct commit_directive {
		private:
			commit_directive() { };
			commit_directive(const commit_directive &) { };
			friend commit_directive memcache::commit(commit_directive);
		};
	};

	typedef helper::commit_directive (*commit_directive_t)(helper::commit_directive);

	inline helper::commit_directive commit(helper::commit_directive) {
		return helper::commit_directive();
	};

}; // namespace memcache

#endif // __MEMCACHE_DETAIL_DIRECTIVES_commit_HPP__

