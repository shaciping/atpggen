
-------------------------------------------------------------------
atpgGenerator - Combinational ATPG generator.
-------------------------------------------------------------------
Arun. C (arunc@ee.iitb.ac.in)
EE-709 - Testing and Verification for VLSI Circuits.
Project Guide : Prof. Virender Singh.
-------------------------------------------------------------------
Directories and files:
compile.bash - compilation script.
code/ - source code.
docs/ - report and presentation.
bin/ - Directory where the final executable will go after compiling.
examples/ - sample circuits (verilog) and results.
benchmarks/- Bench mark circuits. ISCAS-85.
-------------------------------------------------------------------
Compiling:
cd ./code
g++ -g -Wall -Wno-deprecated main.cpp -lboost_regex -lboost_graph -o ../bin/atpgGenerator 
-------------------------------------------------------------------
Usage: ${PATH}/atpgGenerator ex1.v | tee ex1.log
Result: cat ex1.v_fault.lst 
-------------------------------------------------------------------
Dept of Electrical Engineering - IIT Bombay
-------------------------------------------------------------------
