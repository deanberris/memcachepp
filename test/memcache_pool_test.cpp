#define BOOST_TEST_MODULE Memcache Pool Test
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <memcache++/memcache.hpp>
#include <boost/fusion/tuple.hpp>

BOOST_AUTO_TEST_CASE ( initializer_test ) {
    namespace fusion = boost::fusion ;
    
    memcache::handle mc;
    mc << memcache::pool(
            "primary", 3,
            fusion::make_tuple(
                memcache::server("localhost", 11211),
                memcache::server("localhost", 11212)
                )
            )
        << memcache::pool(
                "secondary", 3,
                fusion::make_tuple(
                    memcache::server("localhost", 11213),
                    memcache::server("localhost", 11214)
                    )
                )
        << memcache::connect
        ;
    
    try { mc << memcache::delete_("blah+blah,blah?"); }
    catch (...) { };
};

BOOST_AUTO_TEST_CASE ( set_replication_test ) {
    namespace fusion = boost::fusion;

    memcache::handle mc, mc1, mc2;
    mc << memcache::pool(
            "pool",
            fusion::make_tuple(
                memcache::server("localhost", 11211),
                memcache::server("localhost", 11212)
                )
            )
        << memcache::connect;

    mc1 << memcache::server("localhost", 11211)
        << memcache::connect;

    mc2 << memcache::server("localhost", 11212)
        << memcache::connect;

    BOOST_CHECK_NO_THROW ( mc << memcache::set("blah+blah,blah?", 1) ; );
    int container = 0;
    BOOST_CHECK_NO_THROW ( mc1 << memcache::get("blah+blah,blah?", container) ; );
    BOOST_CHECK_EQUAL ( container, 1 );
    BOOST_CHECK_NO_THROW ( mc2 << memcache::get("blah+blah,blah?", container) ; );
    BOOST_CHECK_EQUAL ( container, 1 );
};

BOOST_AUTO_TEST_CASE ( get_failover_test ) {
    namespace fusion = boost::fusion;

    memcache::handle mc, mc1, mc2;
    mc << memcache::pool(
            "pool1",
            fusion::make_tuple(
                memcache::server("localhost", 11211),
                memcache::server("localhost", 11212)
                )
            )
        << memcache::pool(
                "pool2",
                fusion::make_tuple(
                    memcache::server("localhost", 11213),
                    memcache::server("localhost", 11214)
                    )
                )
        << memcache::connect;
    
    BOOST_CHECK_NO_THROW ( mc << memcache::set("1", std::string("some_value")) << memcache::set("4", std::string("some_other_value")); );
    std::string value, other_value;
    BOOST_CHECK_NO_THROW ( mc << memcache::get("1", value) << memcache::get("4", other_value); );
    BOOST_CHECK_EQUAL ( value, "some_value" );
    BOOST_CHECK_EQUAL ( other_value, "some_other_value" );
};

BOOST_AUTO_TEST_CASE ( get_raw_failover_test ) {
    namespace fusion = boost::fusion;

    memcache::handle mc, mc1, mc2;
    mc << memcache::pool(
            "pool1",
            fusion::make_tuple(
                memcache::server("localhost", 11211),
                memcache::server("localhost", 11212)
                )
            )
        << memcache::pool(
                "pool2",
                fusion::make_tuple(
                    memcache::server("localhost", 11213),
                    memcache::server("localhost", 11214)
                    )
                )
        << memcache::connect;
    
    BOOST_CHECK_NO_THROW ( mc << memcache::raw_set("1", std::string("some_value")) << memcache::raw_set("4", std::string("some_other_value")); );
    std::string value, other_value;
    BOOST_CHECK_NO_THROW ( mc << memcache::raw_get("1", value) << memcache::raw_get("4", other_value); );
    BOOST_CHECK_EQUAL ( value, "some_value" );
    BOOST_CHECK_EQUAL ( other_value, "some_other_value" );
};

BOOST_AUTO_TEST_CASE ( delete_completeness_test ) {
    namespace fusion = boost::fusion;

    memcache::handle mc, mc1, mc2;
    mc << memcache::pool(
            "pool1",
            fusion::make_tuple(
                memcache::server("localhost", 11211),
                memcache::server("localhost", 11212)
                )
            )
        << memcache::pool(
                "pool2",
                fusion::make_tuple(
                    memcache::server("localhost", 11213),
                    memcache::server("localhost", 11214)
                    )
                )
        << memcache::connect;

    try { mc << memcache::delete_("1"); } catch (...) { };
    try { mc << memcache::delete_("4"); } catch (...) { };
    std::string value, other_value;
    BOOST_CHECK_THROW ( mc << memcache::raw_get("1", value); , memcache::key_not_found );
    BOOST_CHECK_THROW ( mc << memcache::raw_get("4", other_value); , memcache::key_not_found );
    BOOST_CHECK_EQUAL ( value, "" );
    BOOST_CHECK_EQUAL ( other_value, "" );
};

BOOST_AUTO_TEST_CASE ( server_pool_test ) {
    memcache::handle mc;
    memcache::server_pool pool("pool");
    for (unsigned int i = 0; i < 4; ++i)
        pool.add_server("localhost", 11211 + i);

    mc << memcache::pool(pool) << memcache::connect;

    BOOST_CHECK_NO_THROW ( mc << memcache::set("server_pool_test_key", std::string("a value for testing")); );
    std::string value;
    BOOST_CHECK_NO_THROW ( mc << memcache::get("server_pool_test_key", value); );
    BOOST_CHECK_EQUAL ( value, "a value for testing" );
    BOOST_CHECK_NO_THROW ( mc << memcache::delete_("server_pool_test_key"); );
    BOOST_CHECK_THROW ( mc << memcache::get("server_pool_test_key", value); , memcache::key_not_found );
};
