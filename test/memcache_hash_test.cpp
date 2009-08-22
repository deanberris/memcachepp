#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <memcachepp/memcache.hpp>

struct hash_test : memcache::policies::default_hash<> {
    size_t operator() (std::string const & key) const {
        return memcache::policies::default_hash<>::hash(key, 2);
    };
};

BOOST_AUTO_TEST_CASE ( hash_difference_test ) {
    hash_test instance;
    std::cerr << "hash for '1': " << instance("1") << '\n';
    std::cerr << "hash for '4': " << instance("4") << '\n';
    BOOST_CHECK ( instance("1") != instance("4") );
};

