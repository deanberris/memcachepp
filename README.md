Memcache++ - a memcache client library
======================================

Requirements
------------

* Boost C++ Library (1.38 or SVN Trunk)
* GCC 4.4.x

Notes
-----

On Hacking Memcache++

* When you find a bug, write a unit test to reproduce the bug
  then make sure that any fixes will not cause regressions to
  earlier marked passing tests.
* Send email to <mikhailberis@gmail.com> for feature requests
  and patches.
* Use git and clone

      git://github.com/mikhailberis/memcachepp.git

On Testing Memcache++:

* You need at least 4 running instances of memcached on the
  local machine to run the tests. The provided test-setup.sh
  script assumes you have memcached installed and available
  from the path. To run it, you need administrator privileges
  on the local machine.
* Invoking 'bjam' in the home directory will require that you
  have properly installed and set up the Boost.Build version 2
  build system on your machine, and the BOOST_ROOT environment
  variable defined to point to the Boost C++ Library
  distribution root directory.

On Installing Memcache++:

* Memcache++ is header-only, meaning it can be made part of
  the application you''re building. This requires though that
  at build-time, the Boost C++ Library 1.38 or a checkout of
  the Boost subversion trunk is accessible.
* The required libraries are:
    - Boost.Serialization
    - Boost.Asio (and Boost.System)
    - Boost.Regex
    - Boost.Spirit
    - Boost.String_algo
    - Boost.Concept_check

