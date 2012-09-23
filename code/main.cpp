//-------------------------------------------------------------------------
// File          : main.cpp
// Author        : Arun.C  <arunc@ee.iitb.ac.in>
//               : Sidharth Mohta <msiddharth@ee.iitb.ac.in>
// Guide         : Prof. Virendra Singh <viren@ee.iitb.ac.in>
//
// Dept of Electrical Engineering - IIT Bombay.
//
// This project is done as part of fulfilment of the course
// "Testing and Verification of VLSI Circuits - EE709"
// 
//-------------------------------------------------------------------------
// Main program.
// Generates atpg patterns for the verilog file given as input.
// The input should be netlist (structural only) with verilog built
// in primitives.
// The result file (pattern file) is "inputfile.v_fault.lst"
// Patterns are printed for SA-0 and SA-1 separately for each fault line.
// Fault line naming convention is component_net.
// eg: If the component is "or_gate1 or(out1, in1, in2);"
//     there will be 3 fault lines, or_gate1_out1, or_gate1_in1 & or_gate1_in2
//     SA-0 and SA-1 is printed for each of these fault lines.
// Detailed info abt the working is dumped into console.
// For details of the algorithm, approach and results for the experiments
// attempted refer the report and ppt.
//
// Dependancies:
// Depends on boost libraries (C++0.x standard). 
// The version used in this program is 1.49.0
//
// For details and other aspects, please go through the presentation.pdf
// and report.pdf given in ../docs directory.
//
// For instructions on compiling and generating binary, go through the 
// readMe provided.
//
// Note: No syntax or semantic checks are performed. The input verilog 
// file shud be clean in all aspects.
//-------------------------------------------------------------------------
#include <iostream>
#include <new>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cassert>
#include "verilogLineParser.hpp"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp> 
#include "global.hpp"
#include "buildGraph.hpp"
#include "atpg.hpp"
#include <cstdio>
#include <cstring>

using namespace std;
using namespace boost;
//-------------------------------------------------------------------------
int main (int argc, char **argv) {
   
   string faultfile_tmp = (string)argv[1]+"_"+"fault.lst";
   char *faultfile = (char *)faultfile_tmp.c_str();

   ifstream file1;
   file1.open(argv[1]);
   if (!file1.is_open()) {
      cout<<"Cannot open input file"<<endl; assert(0);
   }
   string txtline;
   int num_nodes = 0;
   while (file1.good()){ // get the number of txtlines (nodes in the graph)
      getline (file1, txtline);
      string txtline_tmp = txtline;
      //txtline_tmp.erase(
      //remove_if(txtline_tmp.begin(), txtline_tmp.end(), isspace), 
      //txtline_tmp.end()); //Remove(check) for all spaces.
      if (txtline_tmp.empty()) continue; //Check for blank txtline.
      num_nodes ++;
   }
   file1.close();

   node = new(nothrow) VerilogNode[num_nodes];
   BuildGraph graph1(node, argv[1], linelist, 
		     line_to_node_map, 
		     line_to_net_map);

   graph1.CreateFanoutLineMap(line_to_net_map, linelist, 
			      line_to_node_map,
			      line_to_fanoutline_map);
   graph1.CreateFaninLineMap(linelist, line_to_net_map, 
			     line_to_faninline_map);
   graph1.CreateInputLineMap(node, linelist, line_to_node_map, 
			     line_to_inputline_map);
   
   cout <<"#########################################################"<<endl;
   
   list<LineType> linelist_golden = linelist;
   list<LineType>::iterator i = linelist_golden.begin();
   int count = 1;
   int faultno = 1;
   for (; i != linelist_golden.end(); i++) {
      LineType curr_line = (*i);
      cout <<"---------------------------------------------------------"<<endl;
      cout <<"---------------------------------------------------------"<<endl;
      linelist = linelist_golden;

      // No test pattern if line is design input pin or output pin.
      // Corresponding gate input/output is generated separately.
      if (PI == curr_line.type || PO == curr_line.type) continue;
      cout <<"TG for line #"<<count<<" SA-1. Fault #"<<faultno++<<endl;
      //---cout <<"Output line and corresponding input lines of the component"<<endl;
      //---putLine(linelist, line_to_inputline_map, true);
      Atpg atpg_sa1(linelist);
      atpg_sa1.UpdateChanges (curr_line, linelist);
      atpg_sa1.UpdateMaps (node, linelist,
			   line_to_node_map, 
			   line_to_net_map,
			   line_to_fanoutline_map,
			   line_to_faninline_map,
			   line_to_inputline_map,
			   line_to_outputline_map);
      //putLine (curr_line);
      PutPiPatterns (linelist);
      bool gen_status;
      gen_status = atpg_sa1.GenerateTP (node, curr_line, linelist, 
					line_to_node_map,
					line_to_net_map,
					line_to_inputline_map,
					line_to_outputline_map,
					line_to_fanoutline_map,
					ZERO);
      if (gen_status) {
	 PutPiPatterns (linelist);
	 PutPiPatterns (linelist, (*i), "SA-1", faultfile);
      } 
      else cout <<"ERROR: Cannot generate the test patterns for this fault."<<endl;
      cout<<"Here !!!"<<endl;
      cout <<"---------------------------------------------------------"<<endl;
      cout <<"---------------------------------------------------------"<<endl;

      linelist = linelist_golden;
      curr_line = (*i);
      cout <<"TG for line #"<<count<<" SA-0. Fault #"<<faultno++<<endl;
      Atpg atpg_sa0(linelist);
      atpg_sa0.UpdateChanges (curr_line, linelist);
      atpg_sa0.UpdateMaps (node, linelist,
			   line_to_node_map, 
			   line_to_net_map,
			   line_to_fanoutline_map,
			   line_to_faninline_map,
			   line_to_inputline_map,
			   line_to_outputline_map);
      //putLine (curr_line);
      PutPiPatterns (linelist);
      gen_status = atpg_sa0.GenerateTP (node, curr_line, linelist, 
					line_to_node_map,
					line_to_net_map,
					line_to_inputline_map,
					line_to_outputline_map,
					line_to_fanoutline_map,
					ONE);
      if (gen_status) {
	 PutPiPatterns (linelist);
	 PutPiPatterns (linelist, (*i), "SA-0", faultfile);
      }
      else cout <<"ERROR: Cannot generate the test patterns for this fault."<<endl;
      count++;
   }
   cout <<"#########################################################"<<endl;
   
   //file1.close();
   return (0);
}
//-------------------------------------------------------------------------
