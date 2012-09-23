//-------------------------------------------------------------------------
// File          : global.cpp
// Author        : Arun.C  <arunc@ee.iitb.ac.in>
//               : Sidharth Mohta <msiddharth@ee.iitb.ac.in>
// Guide         : Prof. Virendra Singh <viren@ee.iitb.ac.in>
//
// Dept of Electrical Engineering - IIT Bombay.
//-------------------------------------------------------------------------
// All the global datastructure variables are declared here.
// Includes one array of nodes and 6 maps.
// The graph is traversed with the help nodes and maps.
//-------------------------------------------------------------------------
#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <new>
#include <map>
#include <list>
#include "verilogNode.hpp"
#include "faultLine.hpp"

using namespace std;
using namespace boost;
//-------------------------------------------------------------------------
VerilogNode *node; // Array of nodes (components)
int TOTAL_NODES = 0; // Stores total nodes(components) in the netlist.

list <LineType> linelist; // List of all fault lines.

map <LineType, int> line_to_node_map; // map faultline -> node

// map faultline -> corresponding netname
map <LineType, string> line_to_net_map;

// map faultline->fanout fault line list.(multiple fanouts possible. hence list)
map <LineType, LineList> line_to_fanoutline_map;

map <LineType, LineType> line_to_faninline_map;// map faultline->driver faultline

// map faultline->input lines of the corresponding component. 
// multiple inputs are there except for buf and not.
map <LineType, LineList> line_to_inputline_map;

// map faultline->output lines of the corresponding component. 
// multiple outputs may be there.
map <LineType, LineList> line_to_outputline_map;
//-------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------

