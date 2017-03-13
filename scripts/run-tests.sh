#!/bin/sh
set -ex
mkdir build && cd build
cmake .. && make && make test