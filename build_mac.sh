#!/bin/bash

# build shared library
premake5 gmake
make clean
make all -j8

# build src 
cd src
premake5 gmake
make clean
make all -j8
