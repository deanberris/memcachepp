
// Copyright 2008 (c) Friendster, Inc.
// Copyright 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE Memcache Fluent Interface Test
#define BOOST_AUTO_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <memcachepp/memcache.hpp>
#include <memcachepp/fluent/wrap.hpp>
#include <memcachepp/fluent/get.hpp>
#include <memcachepp/fluent/key.hpp>

BOOST_AUTO_TEST_CASE ( assignment_test ) {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211) << memcache::connect;

    BOOST_CHECK ( mc.is_connected("localhost:11211") );

    // The idea here is that it should be easy to assign data
    // to the left-hand-side object using a "get(mc, 'key')"
    mc << memcache::set("hello", std::string("Hello, World!"));
    std::string container;
    try {
        using namespace memcache::fluent;
        wrap(container) = get(mc, "hello");
    } catch (std::exception & e) {
        BOOST_FAIL(e.what());
    } catch (...) {
        BOOST_FAIL("Unknown non-standard exception caught.");
    };

    BOOST_CHECK_EQUAL ( "Hello, World!", container );

    try { mc << memcache::delete_("hello"); } catch (...) { };
};

BOOST_AUTO_TEST_CASE ( key_set_test ) {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211) << memcache::connect;

    BOOST_CHECK ( mc.is_connected("localhost:11211") );

    // The idea here is that it should be easy to set data
    // by just using the assignment operator, once the
    // left hand side is termed to be a "key".
    try {
        using namespace memcache::fluent;
        key(mc, "hello") = std::string("Hello, World! Again!");
        
        std::string container;
        wrap(container) = get(mc, "hello");
        BOOST_CHECK_EQUAL ( std::string("Hello, World! Again!"), container );

    } catch (std::exception & e) {
        BOOST_FAIL(e.what());
    } catch (...) {
        BOOST_FAIL("Unknown non-standard exception caught.");
    };

    try { mc << memcache::delete_("hello"); } catch (...) { };
};

