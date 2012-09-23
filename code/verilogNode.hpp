//-------------------------------------------------------------------------
// File          : verilogNode.cpp
// Author        : Arun.C  <arunc@ee.iitb.ac.in>
//               : Sidharth Mohta <msiddharth@ee.iitb.ac.in>
// Guide         : Prof. Virendra Singh <viren@ee.iitb.ac.in>
//
// Dept of Electrical Engineering - IIT Bombay.
//-------------------------------------------------------------------------
// Basic Node datatype for the data-structure.
// Datastructure is a graph with vertex element as VerilogNode.
// verilogDataType = Type of component it represents.
// verilogValue = 5 valued algebra values.
//-------------------------------------------------------------------------
#ifndef VERILOGNODE_H
#define VERILOGNODE_H

#include <iostream>
#include <new>
#include <list>
#include <map>
using namespace std;

// All the generics that we need.
enum verilogDataType {AND, OR, NOT, BUF,
		      NAND, NOR, XOR, XNOR, 
		      INPUT, OUTPUT, WIRE, INOUT, 
		      MODULE, ENDMODULE, UNDEFINED};

// 5-valued algebra.
enum verilogValue {ZERO, ONE, X, U, D, D_BAR};
//-------------------------------------------------------------------------
class VerilogNode {
public:
   string component_name;
   int nodeID;
   int level;
   verilogDataType type;
   int cv; // Contollability value
   int inv_parity; // inversion value.
   int num_inputs;
   int num_outputs;
   list<string> input_list ;
   list<string> output_list;
   map <string, verilogValue> input_list_value; 
   map <string, verilogValue> output_list_value; 

   void put();

   VerilogNode (int num) {
      component_name = "NONAME";
      nodeID = num;
      level = 0; // level is always initialized as 0, upon creation.
      type = UNDEFINED;
      cv = 0; // Need to see.
      inv_parity = 0;
      num_inputs = 0;
      num_outputs = 0;
      input_list.push_front("NONET");
      output_list.push_front("NONET");
      input_list_value.insert(pair<string, verilogValue>
			      (input_list.front(), U));
      output_list_value.insert(pair<string, verilogValue>
			      (output_list.front(), U));
      //value = U;
   }
   VerilogNode () {
      component_name = "NONAME";
      nodeID = 0;
      level = 0; // level is always initialized as 0, upon creation.
      type = UNDEFINED;
      cv = 0; // Need to see.
      inv_parity = 0;
      num_inputs = 0;
      num_outputs = 0;
      input_list.push_front("NONET");
      output_list.push_front("NONET");
      input_list_value.insert(pair<string, verilogValue>
			      (input_list.front(), U));
      output_list_value.insert(pair<string, verilogValue>
			      (output_list.front(), U));
      //value = U;
   }

   ~VerilogNode () {}
};
//-------------------------------------------------------------------------
//VerilogNode::VerilogNode 
//-------------------------------------------------------------------------
void VerilogNode::put () {
   cout <<"component_name :"<<component_name<<endl;
   cout <<"nodeID :"<<nodeID<<endl;
   cout <<"level :"<<level<<endl;
   cout <<"type :"<<type<<endl;
   cout <<"cv :"<<cv<<endl;
   cout <<"inv_parity :"<<inv_parity<<endl;
   cout <<"num_inputs :"<<num_inputs<<endl;
   cout <<"num_outputs :"<<num_outputs<<endl;
   list <string> tmp1 = input_list;
   for (list<string>::size_type i=0; i<input_list.size(); i++) {
      map<string, verilogValue>::iterator p;
      map<string, verilogValue> m = input_list_value;
      string input_term = tmp1.front();
      p = m.find(input_term);
      verilogValue value = p->second;
      if (p == m.end()) {
	 cout<<"ERROR: Value does not exist for key"<<input_term<<endl;
      }
      cout <<"["<<i<<"th input:"<<tmp1.front()
	   <<" value:"<<value<<"] ";
      tmp1.pop_front();
   }
   cout <<endl;
   list <string> tmp2 = output_list;
   for (list<string>::size_type i=0; i<output_list.size(); i++) {
      map<string, verilogValue>::iterator p;
      map<string, verilogValue> m = output_list_value;
      string output_term = tmp2.front();
      p = m.find(output_term);
      verilogValue value = p->second;
      if (p == m.end()) {
	 cout<<"ERROR: Value does not exist for key"<<output_term<<endl;
      }

      cout <<"["<<i<<"th output:"<<tmp2.front()
	   <<" value:"<<value<<"] ";

      tmp2.pop_front();
   }
   cout <<endl;
}
//-------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------
