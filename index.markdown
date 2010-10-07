---
layout: default
title: memcache++
top: README
---

# Memcache++

## Description

A header-only memcache client for C++ leveraging the 
Boost C++ Library.

## Documentation

You can read the documentation for different versions of memcache++ through
the following links:

* [0.12.1](http://mikhailberis.github.com/memcachepp/docs/0.12.1/)

## Notes

### On Hacking Memcache++:

* When you find a bug, write a unit test to reproduce the bug then make sure 
  that any fixes will not cause regressions to earlier marked passing tests.
* Send email to <mikhailberis@gmail.com> for feature requests and patches.
* Go to http://github.com/mikhailberis/memcachepp and [fork](http://help.github.com/forking/) it.
* Implement your stuff on your own port, then when you're ready submit a
  [pull request](http://help.github.com/pull-request/).

### On Testing Memcache++:

* You need at least 4 running instances of memcached on the local machine to 
  run the tests. The provided test-setup.sh script assumes you have memcached 
  installed and available from the path. To run it, you need administrator 
  privileges on the local machine.
* Invoking 'bjam' in the home directory will require that you have properly 
  installed and set up the Boost.Build version 2 build system on your machine, 
  and the BOOST_ROOT environment variable defined to point to the Boost C++ 
  Library distribution root directory.
* If you prefer CMake, you can also choose to build using your native Build 
  Tool of choice by generating the project build files using CMake (at least version 2.6).

### On Installing Memcache++:

* Memcache++ is header-only, meaning it can be made part of
  the application you're building. This requires though that
  at build-time, the Boost C++ Library 1.44 or a checkout of
  the Boost subversion trunk is accessible.
* The required libraries are:
  - Boost.Serialization
  - Boost.Asio (and Boost.System)
  - Boost.Regex
  - Boost.Spirit
  - Boost.String_algo
  - Boost.Concept_check

## Download

You can download this project in either [zip](http://github.com/mikhailberis/memcachepp/zipball/master)
or [tar](http://github.com/mikhailberis/memcachepp/tarball/master) formats.

You can also clone this project with [Git](http://git-scm.com) by running:

    $ git clone git://github.com/mikhailberis/memcachepp

