#!/bin/sh

set -ex

mkdir -p bin

gcc -Wall -Wextra -o bin/peng src/main.c src/peng/peng.c -lraylib -lm
