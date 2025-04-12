#!/bin/bash

CFLAGS="-Wall -Wextra"
SRC="src/main.c src/peng/peng.c"
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
	INCLUDE_PATH="/usr/local/include"
	LIB_PATH="/usr/local/lib"
else
	echo "Unsupported OS. Exiting compilation."
	exit 1
fi


gcc $SRC $CFLAGS $INCLUDE_PATH $LIB_PATH $LIBS $OUT
