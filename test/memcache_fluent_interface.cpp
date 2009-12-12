
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
    std::string container;
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
    BOOST_CHECK_NO_THROW( wrap(container) = get(mc, "hello") );
    BOOST_CHECK_EQUAL ( std::string("Hello, World! Again!"), container );
};

BOOST_AUTO_TEST_CASE ( key_raw_test ) {
    // We want to set the raw contents of a key in memcache
    // using the %= directive.
    using namespace memcache::fluent;
    BOOST_CHECK_NO_THROW ( key(mc, "hello") %= std::string("raw_data") );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL ( std::string("raw_data"), container );
}

BOOST_AUTO_TEST_CASE ( key_replace_test ) {
    // Now we want to be able to replace the contents of a key
    // that should already be in memcache.
    using namespace memcache::fluent;
    BOOST_CHECK_NO_THROW ( key(mc, "hello") = std::string("The quick brown fox...") );
    BOOST_CHECK_NO_THROW ( wrap(container) = get(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("The quick brown fox..."), container );
    BOOST_CHECK_NO_THROW ( key(mc, "hello") ^= std::string("The quick brown fox jumps!") );
    BOOST_CHECK_NO_THROW ( wrap(container) = get(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("The quick brown fox jumps!"), container );
}

BOOST_AUTO_TEST_CASE ( key_add_test ) {
    // What we want to do is add a key which should not already be
    // in memcache.
    using namespace memcache::fluent;
    BOOST_CHECK_NO_THROW ( key(mc, "hello") /= std::string("I added this.") );
    BOOST_CHECK_NO_THROW ( wrap(container) = get(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("I added this."), container );
    BOOST_CHECK_THROW    ( key(mc, "hello") /= std::string("I tried..."), memcache::key_not_stored );
    BOOST_CHECK_EQUAL    ( std::string("I added this."), container );
}

BOOST_AUTO_TEST_CASE ( key_raw_add_test ) {
    // What we want to do is add a key which should not already be
    // in memcache, but set the raw contents of it instead of having
    // the object serialized.
    using namespace memcache::fluent;
    BOOST_CHECK_NO_THROW ( key(mc, "hello") *= std::string("I added this.") );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("I added this."), container );
    BOOST_CHECK_THROW    ( key(mc, "hello") *= std::string("I tried..."), memcache::key_not_stored );
    BOOST_CHECK_EQUAL    ( std::string("I added this."), container );
}

BOOST_AUTO_TEST_CASE ( key_raw_append_test ) {
    // What we want to allow is to append to already raw-set values
    // associated with a key in memcache using the APPEND operation.
    using namespace memcache::fluent;
    BOOST_CHECK_NO_THROW ( key(mc, "hello") *= "Hello" );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("Hello"), container );
    BOOST_CHECK_NO_THROW ( key(mc, "hello") >>= " World!" );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("Hello World!"), container );
}

BOOST_AUTO_TEST_CASE ( key_raw_prepend_test ) {
    // What we want to allow is to prepend to already raw-set values
    // associated with a key in the memcache using the PREPEND operation.
    using namespace memcache::fluent;
    BOOST_CHECK_NO_THROW ( key(mc, "hello") *= "World!" );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("World!"), container );
    BOOST_CHECK_NO_THROW ( key(mc, "hello") <<= "Hello " );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("Hello World!"), container );
}

BOOST_AUTO_TEST_CASE ( key_increment_decrement_test ) {
    // We want to support incrementing and decrementing raw-set keys.
    using namespace memcache::fluent;
    boost::uint64_t value = 0u;
    BOOST_CHECK_NO_THROW ( key(mc, "hello") *= "0" );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("0"), container );
    BOOST_CHECK_NO_THROW ( value = key(mc, "hello")++ );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("1"), container );
    BOOST_CHECK_EQUAL    ( 1u, value );
    BOOST_CHECK_NO_THROW ( value = key(mc, "hello")-- );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("0"), container );
    BOOST_CHECK_EQUAL    ( 0u, value );
    BOOST_CHECK_NO_THROW ( value = key(mc, "hello") += 10u );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("10"), container );
    BOOST_CHECK_EQUAL    ( 10u, value );
    BOOST_CHECK_NO_THROW ( value = key(mc, "hello") -= 10u );
    BOOST_CHECK_NO_THROW ( wrap(container) = raw(mc, "hello") );
    BOOST_CHECK_EQUAL    ( std::string("0 "), container );
    BOOST_CHECK_EQUAL    ( 0u, value );
}

BOOST_AUTO_TEST_SUITE_END()

