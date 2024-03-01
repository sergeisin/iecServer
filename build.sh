#!/bin/bash

if [ ! -d "./libiec61850" ]; then
    git clone https://github.com/mz-automation/libiec61850.git
fi

rm -rf ./build
mkdir build
cd build
cmake ..
cmake --build .
