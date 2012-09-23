//-------------------------------------------------------------------------
// File          : verilogLineParser.cpp
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
// Parse a given line of the verilog input file.
// txt-line is classified based on verilog generics and a verilogNode is 
// initialized based on that.
//
// Here "line" is a PI/PO/internal_wire, where fault can occur.
// "txtline" is a verilog line in the verilog file, that has to be parsed.
//
// INOUT ports treated as INPUT, as value can be forced.
//
// NAND and NOR inversion parity = 1.
// AND and OR, inversion parity = 0.

//-------------------------------------------------------------------------

#ifndef LINEPARSER_H
#define LINEPARSER_H

#include <iostream>
#include <new>
#include <string>
#include <map>
#include <algorithm>
#include "verilogNode.hpp"
#include "faultLine.hpp"
//#include "global.hpp"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp> 

using namespace std;
using namespace boost;

//-------------------------------------------------------------------------
class VerilogLineParser {
public:
   VerilogLineParser(string, VerilogNode&);
   VerilogLineParser(string, VerilogNode&, 
		     list<LineType> &, 
		     map<LineType,int> &, int);
   VerilogLineParser(string, VerilogNode&, 
		     list<LineType>&, 
		     map<LineType,int>&,
		     map<LineType,string>&,int);

private:
   void ParseAnd(string,VerilogNode&,list<LineType>&,
		 map<LineType,int>&,map<LineType,string>&,int);
   void ParseNot(string,VerilogNode&,list<LineType>&,
		 map<LineType,int>&,map<LineType,string>&,int);
   void ParseBuf(string,VerilogNode&,list<LineType>&,
		 map<LineType,int>&,map<LineType,string>&,int);
   void ParseOr(string,VerilogNode&,list<LineType>&,
		map<LineType,int>&,map<LineType,string>&,int);
   void ParseNand(string,VerilogNode&,list<LineType>&,
		  map<LineType,int>&,map<LineType,string>&,int);
   void ParseNor(string,VerilogNode&,list<LineType>&,
		 map<LineType,int>&,map<LineType,string>&,int);
   void ParseXor(string,VerilogNode&,list<LineType>&,
		 map<LineType,int>&,map<LineType,string>&,int);
   void ParseXnor(string,VerilogNode&,list<LineType>&,
		  map<LineType,int>&,map<LineType,string>&,int);
   void ParseInput(string,VerilogNode&,list<LineType>&,
		   map<LineType,int>&,map<LineType,string>&,int);
   void ParseOutput(string,VerilogNode&,list<LineType>&,
		    map<LineType,int>&,map<LineType,string>&,int);
   void ParseWire(string,VerilogNode&,list<LineType>&,
		  map<LineType,int>&,map<LineType,string>&,int);
   void ParseInout(string,VerilogNode&,list<LineType>&,
		   map<LineType,int>&,map<LineType,string>&,int);
   void ParseModule(string,VerilogNode&,list<LineType>&,
		    map<LineType,int>&,map<LineType,string>&,int);
   void ParseEndmodule(string,VerilogNode&,list<LineType>&,
		       map<LineType,int>&,map<LineType,string>&,int);
   void ParseUndefined(string,VerilogNode&,list<LineType>&,
		       map<LineType,int>&,map<LineType,string>&,int);

   void SplitLine(string,VerilogNode&,list<LineType>&, 
		  map<LineType,int>&,map<LineType,string>&,int);


};
//-------------------------------------------------------------------------
VerilogLineParser::VerilogLineParser (string txtline, 
				      VerilogNode &rnode,
				      list<LineType> &linelist, 
				      map<LineType,int> &line_to_node_map,
				      map<LineType,string> &line_to_net_map,
				      int nodeID) {

   // pattern : "0+ space->and->1+ space->1+ word->anychar->;"
   regex and_exp("\\s*and\\s+\\w+.*;");
   regex or_exp("\\s*or\\s+\\w+.*;");
   regex not_exp("\\s*not\\s+\\w+.*;");
   regex buf_exp("\\s*buf\\s+\\w+.*;");
   regex nand_exp("\\s*nand\\s+\\w+.*;");
   regex nor_exp("\\s*nor\\s+\\w+.*;");
   regex xor_exp("\\s*xor\\s+\\w+.*;");
   regex xnor_exp("\\s*xnor\\s+\\w+.*;");
   regex input_exp("\\s*input\\s+\\w+.*;");
   regex output_exp("\\s*output\\s+\\w+.*;");
   regex wire_exp("\\s*wire\\s+\\w+.*;");
   regex inout_exp("\\s*inout\\s+\\w+.*;");
   regex module_exp("\\s*module\\s+\\w+.*;");
   regex endmodule_exp("\\s*endmodule\\s*;");

   string txtline_tmp = txtline;
   if (txtline_tmp.empty()) return; //Check for blank line.
   
   if (regex_match(txtline, and_exp)) 
      ParseAnd(txtline, rnode, 
	       linelist, line_to_node_map, line_to_net_map, nodeID);
   else if (regex_match(txtline, or_exp)) 
      ParseOr(txtline, rnode, 
	      linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, not_exp)) 
      ParseNot(txtline, rnode, 
	       linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, buf_exp)) 
      ParseBuf(txtline, rnode, 
	       linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, nand_exp)) 
      ParseNand(txtline, rnode, 
		linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, nor_exp)) 
      ParseNor(txtline, rnode, 
	       linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, xor_exp)) 
      ParseXor(txtline, rnode, 
	       linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, xnor_exp)) 
      ParseXnor(txtline, rnode, 
		linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, input_exp)) 
      ParseInput(txtline, rnode, 
		 linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, output_exp)) 
      ParseOutput(txtline, rnode, 
		  linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, inout_exp)) 
      ParseInout(txtline, rnode, 
		 linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, module_exp)) 
      ParseModule(txtline, rnode, 
		  linelist, line_to_node_map, line_to_net_map, nodeID);	 
   else if (regex_match(txtline, endmodule_exp)) 
      ParseEndmodule(txtline, rnode, 
		     linelist, line_to_node_map, line_to_net_map, nodeID); 
   else 
      ParseUndefined(txtline, rnode, 
		     linelist, line_to_node_map, line_to_net_map, nodeID); 
 
   // We dont need to parse wire.
   // else if (regex_match(txtline, wire_exp)) 
   // ParseWire(txtline, rnode, nodeID);	 
   return;
}

//-------------------------------------------------------------------------
void VerilogLineParser::ParseAnd (string txtline, 
				  VerilogNode& rnode,
				  list<LineType> &linelist, 
				  map<LineType,int> &line_to_node_map,
				  map<LineType,string> &line_to_net_map,
				  int nodeID){

   LineType line_tmp; // To store the parsed contents. Then pushed to <list>

   //after and anything upto "("
   regex component_split ("\\s*and\\s+(.*)\\(.*;");
   string item_regexp("\\1");
   
   rnode.component_name = 
      regex_replace (txtline, component_split, item_regexp);
   
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();

   //first position mapped wire. (output)
   regex connection_split1 ("\\s*and\\s+.*\\(\\s*(.*)\\s*,.*,.*;");
   rnode.output_list.push_front(
      regex_replace (txtline, connection_split1, item_regexp));
   rnode.output_list_value.insert(pair<string, verilogValue>
			      (rnode.output_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.output_list.front();
   line_tmp.value = U;  line_tmp.direction = OUT;
   line_tmp.type = INTERNAL_WIRE;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.output_list.front()));

   //second position mapped wire. (input1)
   regex connection_split2 ("\\s*and\\s+.*\\(.*\\s*,\\s*(.*)\\s*,.*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split2, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));

   //third position mapped wire. (input3)
   regex connection_split3 ("\\s*and\\s+.*\\(.*\\s*,.*,\\s*(.*)\\s*\\).*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split3, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));

   // Other values.
   rnode.type = AND;
   rnode.num_inputs = rnode.input_list.size();
   rnode.num_outputs = rnode.output_list.size();
   rnode.cv = 0;
   rnode.inv_parity = 0;
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseNot(string txtline, 
				 VerilogNode &rnode,
				 list<LineType> &linelist, 
				 map<LineType,int> &line_to_node_map,
				 map<LineType,string> &line_to_net_map,
				 int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   //after not anything upto (
   regex component_split ("\\s*not\\s+(.*)\\(.*;");
   string item_regexp("\\1");
   
   rnode.component_name = 
      regex_replace (txtline, component_split, item_regexp);

   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();

   //first position mapped wire. (output)
   regex connection_split1 ("\\s*not\\s+.*\\(\\s*(.*)\\s*,.*;");
   rnode.output_list.push_front(
      regex_replace (txtline, connection_split1, item_regexp));
   rnode.output_list_value.insert(pair<string, verilogValue>
			      (rnode.output_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.output_list.front();
   line_tmp.value = U;  line_tmp.direction = OUT;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.output_list.front()));

   //second position mapped wire. (input1)
   regex connection_split2 ("\\s*not\\s+.*\\(.*\\s*,\\s*(.*)\\s*\\)\\s*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split2, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));

  // Other values.
   rnode.type = NOT;
   rnode.num_inputs = rnode.input_list.size();
   rnode.num_outputs = rnode.output_list.size();
   rnode.cv = 0; // No meaning.
   rnode.inv_parity = 1; // No meaning
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseBuf(string txtline, 
				 VerilogNode &rnode,
				 list<LineType> &linelist, 
				 map<LineType,int> &line_to_node_map,
				 map<LineType,string> &line_to_net_map,
				 int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   //after buf anything upto (
   regex component_split ("\\s*buf\\s+(.*)\\(.*;");
   string item_regexp("\\1");
   
   rnode.component_name = 
      regex_replace (txtline, component_split, item_regexp);

   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();

   //first position mapped wire. (output)
   regex connection_split1 ("\\s*buf\\s+.*\\(\\s*(.*)\\s*,.*;");
   rnode.output_list.push_front(
      regex_replace (txtline, connection_split1, item_regexp));
   rnode.output_list_value.insert(pair<string, verilogValue>
			      (rnode.output_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.output_list.front();
   line_tmp.value = U;  line_tmp.direction = OUT;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.output_list.front()));

   //second position mapped wire. (input1)
   regex connection_split2 ("\\s*buf\\s+.*\\(.*\\s*,\\s*(.*)\\s*\\)\\s*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split2, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));

   rnode.type = BUF;
   rnode.num_inputs = rnode.input_list.size();
   rnode.num_outputs = rnode.output_list.size();
   rnode.cv = 0;
   rnode.inv_parity = 0;
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseOr(string txtline, 
				VerilogNode &rnode,
				list<LineType> &linelist, 
				map<LineType,int> &line_to_node_map,
				map<LineType,string> &line_to_net_map,
				int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   //after or anything upto (
   regex component_split ("\\s*or\\s+(.*)\\(.*;");
   string item_regexp("\\1");
   
   rnode.component_name = 
      regex_replace (txtline, component_split, item_regexp);
   
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();

   //first position mapped wire. (output)
   regex connection_split1 ("\\s*or\\s+.*\\(\\s*(.*)\\s*,.*,.*;");
   rnode.output_list.push_front(
      regex_replace (txtline, connection_split1, item_regexp));
   rnode.output_list_value.insert(pair<string, verilogValue>
			      (rnode.output_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.output_list.front();
   line_tmp.value = U;  line_tmp.direction = OUT;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.output_list.front()));

   //second position mapped wire. (input1)
   regex connection_split2 ("\\s*or\\s+.*\\(.*\\s*,\\s*(.*)\\s*,.*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split2, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));
   
   //third position mapped wire. (input2)
   regex connection_split3 ("\\s*or\\s+.*\\(.*\\s*,.*,\\s*(.*)\\s*\\).*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split3, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));

   rnode.type = OR;
   rnode.num_inputs = rnode.input_list.size();
   rnode.num_outputs = rnode.output_list.size();
   rnode.cv = 1;
   rnode.inv_parity = 0;
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseNand(string txtline, 
				  VerilogNode &rnode,
				  list<LineType> &linelist, 
				  map<LineType,int> &line_to_node_map,
				  map<LineType,string> &line_to_net_map,
				  int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   //after nand anything upto (
   regex component_split ("\\s*nand\\s+(.*)\\(.*;");
   //regex component_split ("\\s*nand\\s+(.*)\\s*\\(.*;");
   string item_regexp("\\1");
   
   rnode.component_name = 
      regex_replace (txtline, component_split, item_regexp);
   
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();

   //first position mapped wire. (output)
   regex connection_split1 ("\\s*nand\\s+.*\\(\\s*(.*)\\s*,.*,.*;");
   rnode.output_list.push_front(
      regex_replace (txtline, connection_split1, item_regexp));
   rnode.output_list_value.insert(pair<string, verilogValue>
			      (rnode.output_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.output_list.front();
   line_tmp.value = U;  line_tmp.direction = OUT;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.output_list.front()));
   
   //second position mapped wire. (input1)
   regex connection_split2 ("\\s*nand\\s+.*\\(.*\\s*,\\s*(.*)\\s*,.*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split2, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));

   //third position mapped wire. (input2)
   regex connection_split3 ("\\s*nand\\s+.*\\(.*\\s*,.*,\\s*(.*)\\s*\\).*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split3, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));

   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));

   rnode.type = NAND;
   rnode.num_inputs = rnode.input_list.size();
   rnode.num_outputs = rnode.output_list.size();
   rnode.cv = 0;
   rnode.inv_parity = 1;
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseNor(string txtline, 
				 VerilogNode &rnode,
				 list<LineType> &linelist, 
				 map<LineType,int> &line_to_node_map,
				 map<LineType,string> &line_to_net_map,
				 int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   //after nor anything upto (
   regex component_split ("\\s*nor\\s+(.*)\\(.*;");
   string item_regexp("\\1");
   
   rnode.component_name = 
      regex_replace (txtline, component_split, item_regexp);
   
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();

   //first position mapped wire. (output)
   regex connection_split1 ("\\s*nor\\s+.*\\(\\s*(.*)\\s*,.*,.*;");
   rnode.output_list.push_front(
      regex_replace (txtline, connection_split1, item_regexp));
   rnode.output_list_value.insert(pair<string, verilogValue>
			      (rnode.output_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.output_list.front();
   line_tmp.value = U;  line_tmp.direction = OUT;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.output_list.front()));
   
   //second position mapped wire. (input1)
   regex connection_split2 ("\\s*nor\\s+.*\\(.*\\s*,\\s*(.*)\\s*,.*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split2, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));
   
   //third position mapped wire. (input2)
   regex connection_split3 ("\\s*nor\\s+.*\\(.*\\s*,.*,\\s*(.*)\\s*\\).*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split3, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));

   rnode.type = NOR;
   rnode.num_inputs = rnode.input_list.size();
   rnode.num_outputs = rnode.output_list.size();
   rnode.cv = 1;
   rnode.inv_parity = 1;
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseXor(string txtline, 
				 VerilogNode &rnode,
				 list<LineType> &linelist, 
				 map<LineType,int> &line_to_node_map,
				 map<LineType,string> &line_to_net_map,
				 int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   //after xor anything upto (
   regex component_split ("\\s*xor\\s+(.*)\\(.*;");
   string item_regexp("\\1");
   
   rnode.component_name = 
      regex_replace (txtline, component_split, item_regexp);
   
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();

   //first position mapped wire. (output)
   regex connection_split1 ("\\s*xor\\s+.*\\(\\s*(.*)\\s*,.*,.*;");
   rnode.output_list.push_front(
      regex_replace (txtline, connection_split1, item_regexp));
   rnode.output_list_value.insert(pair<string, verilogValue>
			      (rnode.output_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.output_list.front();
   line_tmp.value = U;  line_tmp.direction = OUT;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.output_list.front()));

   //second position mapped wire. (input1)
   regex connection_split2 ("\\s*xor\\s+.*\\(.*\\s*,\\s*(.*)\\s*,.*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split2, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));

   //third position mapped wire. (input2)
   regex connection_split3 ("\\s*xor\\s+.*\\(.*\\s*,.*,\\s*(.*)\\s*\\).*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split3, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));
   
   rnode.type = XOR;
   rnode.num_inputs = rnode.input_list.size();
   rnode.num_outputs = rnode.output_list.size();
   rnode.cv = 0; // There is NO controlling value for XOR gate. Need to do smthg.
   rnode.inv_parity = 1; //Need to see.
   // XXX
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseXnor(string txtline, 
				  VerilogNode &rnode,
				  list<LineType> &linelist, 
				  map<LineType,int> &line_to_node_map,
				  map<LineType,string> &line_to_net_map,
				  int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   //after xnor anything upto (
   regex component_split ("\\s*xnor\\s+(.*)\\(.*;");
   string item_regexp("\\1");
   
   rnode.component_name = 
      regex_replace (txtline, component_split, item_regexp);
   
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();

   //first position mapped wire. (output)
   regex connection_split1 ("\\s*xnor\\s+.*\\(\\s*(.*)\\s*,.*,.*;");
   rnode.output_list.push_front(
      regex_replace (txtline, connection_split1, item_regexp));
   rnode.output_list_value.insert(pair<string, verilogValue>
			      (rnode.output_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.output_list.front();
   line_tmp.value = U;  line_tmp.direction = OUT;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.output_list.front()));
   
   //second position mapped wire. (input1)
   regex connection_split2 ("\\s*xnor\\s+.*\\(.*\\s*,\\s*(.*)\\s*,.*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split2, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));
 
   //third position mapped wire. (input2)
   regex connection_split3 ("\\s*xnor\\s+.*\\(.*\\s*,.*,\\s*(.*)\\s*\\).*;");
   rnode.input_list.push_front(
      regex_replace (txtline, connection_split3, item_regexp));
   rnode.input_list_value.insert(pair<string, verilogValue>
			      (rnode.input_list.front(), U));
   // Store the fault-lines in the list.
   line_tmp.name = rnode.component_name+"_"+rnode.input_list.front();
   line_tmp.value = U;  line_tmp.direction = IN;
   line_tmp.type = INTERNAL_WIRE;
   line_tmp.isVisited = false;
   linelist.push_front(line_tmp);
   line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
   //Map the line to the corresponding net.
   line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));
 
   rnode.type = XNOR;
   rnode.num_inputs = rnode.input_list.size();
   rnode.num_outputs = rnode.output_list.size();
   rnode.cv =0; // There is NO controlling value for xnor gate.
   rnode.inv_parity = 0;
   // Need to do smthg. XXX
   
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseInput(string txtline, 
				   VerilogNode &rnode,
				   list<LineType> &linelist, 
				   map<LineType,int> &line_to_node_map,
				   map<LineType,string> &line_to_net_map,
				   int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   regex component_split ("\\s*input\\s+(.*)\\s*;");
   string item_regexp("\\1");
   string input_ports_tmp = regex_replace (
      txtline, component_split, item_regexp);
   
   regex connection_split ("\\s*"); // remove all the spaces.
   string input_ports = regex_replace (
      input_ports_tmp, connection_split, item_regexp);
   
   // clear output_list, input_list. (Remove NONET)
   rnode.output_list.clear();
   rnode.input_list.clear();
   
   vector<string> v;
   // split based on ";" or ","
   algorithm::split(v, input_ports, algorithm::is_punct());
   for (vector<string>::size_type i=0; i<v.size(); i++) {
      rnode.output_list.push_front (v[i]);
      rnode.output_list_value.insert(pair<string, verilogValue>
				    (rnode.output_list.front(), U));
      // Store the fault-lines in the list.
      line_tmp.name = "input_"+rnode.output_list.front();
      line_tmp.value = U;  line_tmp.direction = OUT;
      line_tmp.type = PI;
      line_tmp.isVisited = false;
      linelist.push_front(line_tmp);
      line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
      //Map the line to the corresponding net.
      line_to_net_map.insert (make_pair(line_tmp,rnode.output_list.front()));
   }

   rnode.component_name = "input_"+v[0];
   rnode.type = INPUT;
   rnode.num_inputs = rnode.input_list.size();
   rnode.num_outputs = rnode.output_list.size();
   rnode.cv = 0;
   rnode.inv_parity = 0;
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseOutput(string txtline, 
				    VerilogNode &rnode,
				    list<LineType> &linelist, 
				    map<LineType,int> &line_to_node_map,
				    map<LineType,string> &line_to_net_map,
				    int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>

   regex component_split ("\\s*output\\s+(.*)\\s*;");
   string item_regexp("\\1");
   string output_ports_tmp = regex_replace (
      txtline, component_split, item_regexp);
   
   regex connection_split ("\\s*"); // remove all the spaces.
   string output_ports = regex_replace (
      output_ports_tmp, connection_split, item_regexp);
   
   // clear input_list, bt not the output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();
   
   vector<string> v;
   // split based on ";" or ","
   algorithm::split(v, output_ports, algorithm::is_punct());
   rnode.component_name = "output_"+v[0];
   for (vector<string>::size_type i=0; i<v.size(); i++) {
      rnode.input_list.push_front (v[i]);
      rnode.input_list_value.insert(pair<string, verilogValue>
				    (rnode.input_list.front(), U));
      // Store the fault-lines in the list.
      line_tmp.name = "output_"+rnode.input_list.front();
      line_tmp.value = U;  line_tmp.direction = IN;
      line_tmp.type = PO;
      line_tmp.isVisited = false;
      linelist.push_front(line_tmp);
      line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
      //Map the line to the corresponding net.
      line_to_net_map.insert (make_pair(line_tmp,rnode.input_list.front()));
   }

   rnode.type = OUTPUT;
   rnode.num_inputs = rnode.input_list.size();
   rnode.num_outputs = rnode.output_list.size();
   rnode.cv = 0;
}
//-------------------------------------------------------------------------
// No use of this function as of now.
// Need not parse wire to understand the functionality.
void VerilogLineParser::ParseWire(string txtline, 
				  VerilogNode &rnode,
				  list<LineType> &linelist, 
				  map<LineType,int> &line_to_node_map,
				  map<LineType,string> &line_to_net_map,
				  int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   rnode.component_name = "wire";

   regex component_split ("\\s*wire\\s+(.*)\\s*;");
   string item_regexp("\\1");
   string wire_ports_tmp = regex_replace (
      txtline, component_split, item_regexp);
   
   regex connection_split ("\\s*"); // remove all the spaces.
   string wire_ports = regex_replace (
      wire_ports_tmp, connection_split, item_regexp);
   
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();
   
   vector<string> v;
   // split based on ";" or ","
   algorithm::split(v, wire_ports, algorithm::is_punct());
   for (vector<string>::size_type i=0; i<v.size(); i++) {
      rnode.input_list.push_front (v[i]);
      rnode.input_list_value.insert(pair<string, verilogValue>
				    (rnode.input_list.front(), U));
      // No need to enter in the fault-line list or line_to_node_map or line_to_net_map.
   }
   rnode.cv = 0;
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseInout(string txtline, 
				   VerilogNode &rnode,
				   list<LineType> &linelist, 
				   map<LineType,int> &line_to_node_map,
				   map<LineType,string> &line_to_net_map,
				   int nodeID)  {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   rnode.component_name = "inout";
   //cout<<"Component Name:"<<rnode.component_name;

   regex component_split ("\\s*inout\\s+(.*)\\s*;");
   string item_regexp("\\1");
   string inout_ports_tmp = regex_replace (
      txtline, component_split, item_regexp);
   
   regex connection_split ("\\s*"); // remove all the spaces.
   string inout_ports = regex_replace (
      inout_ports_tmp, connection_split, item_regexp);
   
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();

   
   vector<string> v;
   // split based on ";" or ","
   algorithm::split(v, inout_ports, algorithm::is_punct());
   // inout ports are saved in input_list, and output_list as well.
   for (vector<string>::size_type i=0; i<v.size(); i++) {
      rnode.input_list.push_front (v[i]);
      rnode.output_list.push_front (v[i]);
      rnode.output_list_value.insert(pair<string, verilogValue>
				    (rnode.output_list.front(), U));
      rnode.input_list_value.insert(pair<string, verilogValue>
				    (rnode.input_list.front(), U));
      // Store the fault-lines in the list.
      line_tmp.name = "inout_"+rnode.output_list.front();
      line_tmp.value = U;  line_tmp.direction = OUT;
      line_tmp.type = PI;
      line_tmp.isVisited = false;
      linelist.push_front(line_tmp);
      line_to_node_map.insert (make_pair(line_tmp,nodeID)); //Map the line to nodeID
      //Map the line to the corresponding net.
      line_to_net_map.insert (make_pair(line_tmp,rnode.output_list.front()));

   }
   rnode.cv = 0;
}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseModule(string txtline, 
				    VerilogNode &rnode,
				    list<LineType> &linelist, 
				    map<LineType,int> &line_to_node_map,
				    map<LineType,string> &line_to_net_map,
				    int nodeID)  {
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();


}
//-------------------------------------------------------------------------
void VerilogLineParser::ParseEndmodule(string txtline, 
				       VerilogNode &rnode,
				       list<LineType> &linelist, 
				       map<LineType,int> &line_to_node_map,
				       map<LineType,string> &line_to_net_map,
				       int nodeID)  {
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();


}
//-------------------------------------------------------------------------
// Assumed input verilog is correct.
void VerilogLineParser::ParseUndefined(string txtline,
				       VerilogNode &rnode,
				       list<LineType> &linelist, 
				       map<LineType,int> &line_to_node_map,
				       map<LineType,string> &line_to_net_map,
				       int nodeID)  {
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();


}
//-------------------------------------------------------------------------
// No use as of now.
//
void VerilogLineParser::SplitLine(string txtline, 
				  VerilogNode &rnode,
				  list<LineType> &linelist, 
				  map<LineType,int> &line_to_node_map,
				  map<LineType,string> &line_to_net_map,
				  int nodeID) {
   LineType line_tmp; // To store the parsed contents. Then pushed to <list>
   // clear the input_list and output_list. (Remove NONET)
   rnode.input_list.clear();
   rnode.output_list.clear();

   //after and anything upto (
   regex component_split ("\\s*and\\s+(.*)\\(.*;");
   string item_regexp("\\1");
   //cout<<"Component Name:";
   cout <<regex_replace (txtline, component_split, item_regexp)<<endl;
   
   //first position mapped wire. (output)
   regex connection_split1 ("\\s*and\\s+.*\\(\\s*(.*)\\s*,.*,.*;");
   //cout <<"Output:";
   cout <<regex_replace (txtline, connection_split1, item_regexp)<<endl;
   
   //second position mapped wire. (input1)
   regex connection_split2 ("\\s*and\\s+.*\\(.*\\s*,\\s*(.*)\\s*,.*;");
   //cout <<"Input1:";
   cout <<regex_replace (txtline, connection_split2, item_regexp)<<endl;
   
   //third position mapped wire. (input3)
   regex connection_split3 ("\\s*and\\s+.*\\(.*\\s*,.*,\\s*(.*)\\s*\\).*;");
   //cout <<"Input2:";
   cout <<regex_replace (txtline, connection_split3, item_regexp)<<endl;
   
   cout<<txtline<<endl;
}
//-------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------
