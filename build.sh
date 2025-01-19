#!/bin/bash

if [ ! -d "./libiec61850" ]; then
    git clone https://github.com/mz-automation/libiec61850.git
    cd ./libiec61850
    git checkout v1.5.3
    cd ..
fi

rm -rf ./build
mkdir build
cd build
cmake ..
cmake --build .
