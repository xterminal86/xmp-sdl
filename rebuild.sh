#!/bin/bash

BuildDir="cli-build"

mkdir -p "${BuildDir}"
cd "${BuildDir}" || exit 1
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug ../
make

