#!/bin/bash

# build shared library
premake4 gmake
make clean
make all -j8

# build src 
cd src
premake4 gmake
make clean
make all -j8

mv src/game ../game