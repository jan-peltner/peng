#!/bin/bash

EXAMPLE_NAME=${1:-basic}
EXAMPLE_FILE="examples/$EXAMPLE_NAME.c"
DEFAULT_EXAMPLE_FILE="examples/basic.c"

CFLAGS="-Wall -Wextra"
PENG_SRC="src/peng/peng.c"
OUT="-o bin/peng"
LIBS="-lraylib -lm"

set -e

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

if [[ -f "$EXAMPLE_FILE" ]]; then
	SRC="$EXAMPLE_FILE $PENG_SRC"
	echo "Compiling example: $EXAMPLE_FILE"
else
	SRC="$DEFAULT_EXAMPLE_FILE $PENG_SRC"
	echo "Example \"$1\" not found. Compiling fallback: $DEFAULT_EXAMPLE_FILE"
fi

gcc $SRC $CFLAGS $INCLUDE_PATH $LIB_PATH $LIBS $OUT

# echo "Copying shaders..."
#
# cp src/peng/shaders/*.glsl bin/
