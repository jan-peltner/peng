#!/bin/sh

EXAMPLE_NAME=${1}

set -e

./build.sh $EXAMPLE_NAME && cd bin && ./peng
