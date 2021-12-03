#!/bin/bash
cd ML_Proc1
python train.py &
cd ..
cd ML_Proc2
sleep 1
python train.py &
cd ..
cd test_proc1
sleep 1
./userOOMtest &
cd ..
sleep 1
cd test_proc2
./userOOMtest & 

