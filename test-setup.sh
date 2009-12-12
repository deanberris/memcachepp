#!/bin/sh

MEMCACHED=`which memcached`

if [ ! -x $MEMCACHED ]; then
    echo "'memcached' not found in PATH. Bailing out."
    exit 1
fi

# ALWAYS RUN UNDER SUDO!

$MEMCACHED -d -m 128 -l 127.0.0.1 -p 11211 
$MEMCACHED -d -m 128 -l 127.0.0.1 -p 11212 
$MEMCACHED -d -m 128 -l 127.0.0.1 -p 11213 
$MEMCACHED -d -m 128 -l 127.0.0.1 -p 11214 

