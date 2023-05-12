#!/bin/bash
set -xe
docker run -it --rm -w /mod -v $PWD:/mod devkitpro/devkita64 ./exlaunch.sh build
