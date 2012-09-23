#!/bin/bash
cd code
g++ -g -Wall -Wno-deprecated main.cpp -lboost_regex -lboost_graph -o ../bin/atpgGenerator &&
echo "Build Successfull"
cd ../examples
./../bin/atpgGenerator ex1.v |& tee ex1.log
./../bin/atpgGenerator ex2.v |& tee ex2.log
./../bin/atpgGenerator ex3.v |& tee ex3.log
./../bin/atpgGenerator ex4_reconvergent.v |& tee ex4_reconvergent.log
