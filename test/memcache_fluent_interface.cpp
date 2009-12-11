
// Copyright 2008 (c) Friendster, Inc.
// Copyright 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE Memcache Fluent Interface Test
#define BOOST_AUTO_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <memcachepp/memcache.hpp>

struct fixtures {
    memcache::handle mc;
    fixtures() {
        mc << memcache::server("localhost", 11211) << memcache::connect;
        BOOST_REQUIRE ( mc.is_connected("localhost:11211") );
    }
    ~fixtures() {
        try { remove(mc, "hello"); } catch (...) { }
    }
};

BOOST_FIXTURE_TEST_SUITE ( fluent_memcache_interface, fixtures )

BOOST_AUTO_TEST_CASE ( assignment_test ) {
    // The idea here is that it should be easy to assign data
    // to the left-hand-side object using a "get(mc, 'key')"
    mc << memcache::set("hello", std::string("Hello, World!"));
    std::string container;
    using namespace memcache::fluent;
    BOOST_CHECK_NO_THROW( wrap(container) = get(mc, "hello") );
    BOOST_CHECK_EQUAL ( "Hello, World!", container );
};

BOOST_AUTO_TEST_CASE ( key_set_test ) {
    // The idea here is that it should be easy to set data
    // by just using the assignment operator, once the
    // left hand side is termed to be a "key".
    using namespace memcache::fluent;
    BOOST_CHECK_NO_THROW( key(mc, "hello") = std::string("Hello, World! Again!") );
    std::string container;
    BOOST_CHECK_NO_THROW( wrap(container) = get(mc, "hello") );
    BOOST_CHECK_EQUAL ( std::string("Hello, World! Again!"), container );
};

BOOST_AUTO_TEST_CASE ( key_raw_test ) {
    // We want to set the raw contents of a key in memcache
    // using the %= directive.
    using namespace memcache::fluent;
    BOOST_CHECK_NO_THROW ( key(mc, "hello") %= std::string("raw_data") );
    std::string container;
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL ( std::string("raw_data"), container );
}

BOOST_AUTO_TEST_CASE ( key_replace_test ) {
    // Now we want to be able to replace the contents of a key
    // that should already be in memcache.
    using namespace memcache::fluent;
    BOOST_CHECK_NO_THROW ( key(mc, "hello") = std::string("The quick brown fox...") );
    std::string container;
    BOOST_CHECK_NO_THROW ( wrap(container) = get(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("The quick brown fox..."), container );
    BOOST_CHECK_NO_THROW ( key(mc, "hello") ^= std::string("The quick brown fox jumps!") );
    BOOST_CHECK_NO_THROW ( wrap(container) = get(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("The quick brown fox jumps!"), container );
}

BOOST_AUTO_TEST_SUITE_END()

