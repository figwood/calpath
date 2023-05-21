#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

make clean
rm -rf ../orbitTools/core/*.o ../orbitTools/core/*.a ../orbitTools/orbit/*.o ../orbitTools/orbit/*.a
cd ../orbitTools/core && make
cd ../orbit && make
cd $SCRIPT_DIR && make

./calpath 