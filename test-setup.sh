#!/bin/sh

# ALWAYS RUN UNDER SUDO!
killall memcached
/usr/local/bin/memcached -d -m 128 -l 127.0.0.1 -p 11211 -u compute
/usr/local/bin/memcached -d -m 128 -l 127.0.0.1 -p 11212 -u compute
/usr/local/bin/memcached -d -m 128 -l 127.0.0.1 -p 11213 -u compute
/usr/local/bin/memcached -d -m 128 -l 127.0.0.1 -p 11214 -u compute

