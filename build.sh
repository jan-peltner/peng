#!/bin/bash

CFLAGS="-Wall -Wextra"
SRC="src/example.c src/peng/peng.c src/peng/threadpool.c"
OUT="-o bin/peng"
LIBS="-lraylib -lm"

set -ex

mkdir -p bin

if [[ "$OSTYPE" == "darwin"* ]]; then
	#macOS build
	echo "Building for macOS..."
	INCLUDE_PATH="-I/opt/homebrew/include"
	LIB_PATH="-L/opt/homebrew/lib"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
	#linux build
	echo "Building for linux..."
	INCLUDE_PATH="-I/usr/local/include"
	LIB_PATH="-L/usr/local/lib"
else
	#unsupported os
	echo "Unsupported OS. Exiting compilation."
	exit 1
fi


gcc $SRC $CFLAGS $INCLUDE_PATH $LIB_PATH $LIBS $OUT
