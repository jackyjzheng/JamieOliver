#!/bin/bash

rm -rf JamieOliver
git clone https://github.com/jackyjzheng/JamieOliver.git
cd JamieOliver/cyclic
make clean
make
python FileUploadListener.py &
nohup ./RamseySearchServer 325 35 &
disown