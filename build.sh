#!/bin/sh

set -ex

mkdir -p bin

gcc -Wall -Wextra -o bin/peng src/main.c src/engine.c -lraylib -lm
