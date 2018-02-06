#!/bin/sh

export PKG_CONFIG_PATH=/usr/local/opt/opencv3/lib/pkgconfig
#g++ -o sm main.cpp sharedmatting.cpp `pkg-config --cflags --libs opencv`
g++ -o mix mix.cpp sharedmatting.cpp `pkg-config --cflags --libs opencv`
