//-------------------------------------------------------------------------
// File          : faultLine.cpp
// Author        : Arun.C  <arunc@ee.iitb.ac.in>
//               : Sidharth Mohta <msiddharth@ee.iitb.ac.in>
// Guide         : Prof. Virendra Singh <viren@ee.iitb.ac.in>
//
// Dept of Electrical Engineering - IIT Bombay.
//-------------------------------------------------------------------------
// Basic line datatype for the data-structure.
// Other helper functions to print the attributes are provided.
// No constructor as such. Uses default constructor.
//-------------------------------------------------------------------------
#ifndef LINE_H
#define LINE_H

#include <iostream>
#include <new>
#include <map>
#include <list>
#include "verilogNode.hpp"

using namespace std;

enum lineAttribute {PI, PO, INTERNAL_WIRE};
enum lineDirection {IN, OUT, INANDOUT};
//-------------------------------------------------------------------------
class LineType{
public:
   string name;
   verilogValue value;
   lineDirection direction;
   lineAttribute type;
   bool isVisited;
   LineType DriverLine(); // Get the driver line, if line.direction=IN
// Get the driver line, if line.direction=IN
   LineType DriverLine(map<LineType,string>,list<LineType>); 
   //LineType DriverLine(const map<LineType,string>&,list<LineType>,
   //map<LineType,int>); 
   void put();

// Overloaded operators (for map usage)
   friend bool operator<(const LineType, const LineType);
   friend bool operator>(const LineType, const LineType);
   friend bool operator==(const LineType, const LineType);
   friend bool operator!=(const LineType, const LineType);
   //friend LineType operator=(LineType);

   //friend bool operator<(const LineType, const LineType);

// list<LineType> print functions.
//---   friend void putLine (list<LineType>);
//---   friend void putLine (list<LineType>, map<LineType,int>);
};
//-------------------------------------------------------------------------
// Work around to map<LineType,list<LineType>>& line_to_inputline_map)
// Template inside a template is NOT OK. Its treated as right shift operator.
// Soln is to define a class as this template element.
class LineList {
public:
   list <LineType> mlines;

};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool operator< (const LineType line1, LineType line2) {
   return (line1.name < line2.name);
}
//-------------------------------------------------------------------------
bool operator> (const LineType line1, LineType line2) {
   return (line1.name > line2.name);
}
//-------------------------------------------------------------------------
bool operator== (const LineType line1, LineType line2) {
   return (line1.name == line2.name);
}
//-------------------------------------------------------------------------
bool operator!= (const LineType line1, LineType line2) {
   return (line1.name != line2.name);
}
//-------------------------------------------------------------------------
//---LineType operator= (LineType line1) {
//---   return (line1);
//---}
//-------------------------------------------------------------------------
// Helper functions.
inline string printDir (LineType curr_line) {
   if (curr_line.direction == IN) return "IN";
   else if (curr_line.direction == OUT) return "OUT";
   else return "INANDOUT";
}
//-------------------------------------------------------------------------
// Helper functions.
inline string printType (LineType curr_line) {
   if (curr_line.type == PI) return "PI";
   else if (curr_line.type == PO) return "PO";
   else return "INT_WIRE";
}
//-------------------------------------------------------------------------
// Helper functions.
inline string printValue (LineType curr_line) {
   if (curr_line.value == ZERO) return "0";
   if (curr_line.value == ONE) return "1";
   if (curr_line.value == X) return "X";
   if (curr_line.value == U) return "U";
   if (curr_line.value == D) return "D";
   if (curr_line.value == D_BAR) return "Dbar";
   return "ERROR";
}
//-------------------------------------------------------------------------
// Helper functions.
inline string printValue (verilogValue value) {
   if (value == ZERO) return "0";
   if (value == ONE) return "1";
   if (value == X) return "X";
   if (value == U) return "U";
   if (value == D) return "D";
   if (value == D_BAR) return "Dbar";
   return "ERROR";
}
//-------------------------------------------------------------------------
// Print all the fault-lines
void putLine (list<LineType> linelist) {
   list<LineType>::size_type i, linelist_size = linelist.size();
   for (i=0; i<linelist_size; i++) {
      cout <<"        [Line:"<<(linelist.front()).name;
      cout <<" Val:"<<printValue(linelist.front());
      cout <<" Dir:"<<printDir(linelist.front());
      cout <<" Visited: "<<linelist.front().isVisited;
      cout <<" Type:"<<printType(linelist.front())<<"]"<<endl;
      linelist.pop_front();
   }
}
//-------------------------------------------------------------------------
// Print all the fault-lines in a compact way.
void putLine (list<LineType> linelist, bool compact) {
   //int j=1;
   list<LineType>::size_type i, linelist_size = linelist.size();
   for (i=0; i<linelist_size; i++) {
      cout <<"        ["<<(linelist.front()).name;
      cout <<" "<<printValue(linelist.front());
      cout <<" "<<printDir(linelist.front());
      cout <<" V="<<linelist.front().isVisited;
      //cout <<" Type:"<<printType(linelist.front())<<"]"<<endl;
      cout <<"] "<<endl;
      //if (4==j++) {cout<<endl;j=1;}
      linelist.pop_front();
   }
}
//-------------------------------------------------------------------------
// Print the details of the given fault-line.
void putLine (LineType curr_line) {
      cout <<"[Line:"<<curr_line.name;
      cout <<" Val:"<<printValue(curr_line);
      cout <<" Dir:"<<printDir(curr_line);
      cout <<" Visited: "<<curr_line.isVisited;
      cout <<" Type:"<<printType(curr_line)<<"]";
}
//-------------------------------------------------------------------------
// Print each line and corresponding component details.
void putLine (list<LineType> linelist, 
	      map<LineType,int> line_to_node_map, 
	      VerilogNode *rnode) {


   list<LineType>::size_type i, num_lines=linelist.size();
   //list<lineType>::size_type num_lines=line.size();
   for (i=0; i<num_lines; i++) {
      cout <<"        [Line:"<<(linelist.front()).name;
      cout <<" Val:"<<printValue(linelist.front());
      cout <<" Dir:"<<printDir(linelist.front());
      cout <<" Visited: "<<linelist.front().isVisited;
      cout <<" Type:"<<printType(linelist.front())<<"] ";
      
      map<LineType,int>::iterator p;
      p = line_to_node_map.find(linelist.front());
      if (p == line_to_node_map.end()) 
	 cout<<" [NO MATCHING COMPONENT !]"<<endl;
      else {
	 int nodeID = p->second;
	 VerilogNode node_tmp = rnode[nodeID];
	 cout <<" [Component:"<<node_tmp.component_name;
	 cout <<" ID:"<<node_tmp.nodeID;
	 cout <<" Level:"<<node_tmp.level;
	 cout <<" Type:"<<node_tmp.type;
	 cout <<" CV:"<<node_tmp.cv;
	 cout <<" IV:"<<node_tmp.inv_parity;
	 cout <<"]";
      }
      cout <<endl;
      linelist.pop_front();
   }
}
//-------------------------------------------------------------------------
// Print the driver lines for all input fault lines.
void putLine(list<LineType>linelist, 
	     map<LineType,string> line_to_net_map){

   list<LineType> linelist_i = linelist, linelist_tmp = linelist;
   list<LineType>::size_type i;
   const list<LineType>::size_type linelist_size = linelist.size();

   
   for (i=0; i<linelist_size; i++) {
      LineType curr_line = linelist_i.front();
      linelist_i.pop_front();

      //putLine(curr_line);
      if (curr_line.direction == OUT) continue; //No driver line for OUT
      if (curr_line.type == PI) continue; //No driver line for PI
      
      LineType driver_line = 
	 curr_line.DriverLine (line_to_net_map, linelist_tmp);
      cout <<"        [Line:"<<curr_line.name;
      cout <<" Val:"<<printValue(curr_line);
      cout <<" Dir:"<<printDir(curr_line);
      cout <<" Visited: "<<curr_line.isVisited;
      cout <<" Type:"<<printType(curr_line)<<"]";

      cout <<" [Driver Line:"<<driver_line.name;
      cout <<" Val:"<<printValue(driver_line);
      cout <<" Dir:"<<printDir(driver_line);
      cout <<" Visited: "<<driver_line.isVisited;
      cout <<" Type:"<<printType(driver_line)<<"]"<<endl;
   }
}
//-------------------------------------------------------------------------
// Print the driver lines of the component of all the lines.
void putLine(list<LineType>linelist, 
	     map<LineType,LineList> line_to_inputline_map){
   
   list<LineType>::size_type i;
   const list<LineType>::size_type linelist_size = linelist.size();

   for (i=0; i<linelist_size;i++) {
      LineType curr_line = linelist.front();
      linelist.pop_front();
      if (curr_line.direction == IN) continue;
      if (curr_line.type == PI) continue;
      
      cout <<"        [Line:"<<curr_line.name;
      cout <<"]";

      map<LineType,LineList>::iterator p = 
	 line_to_inputline_map.find (curr_line);
      if (p == line_to_inputline_map.end()) 
	 cout<<" [ERROR: NO MATCHING LINES !]"<<endl;
      else {
	 list<LineType> linelist_tmp = (p->second).mlines;
	 list<LineType>::size_type j=0, 
	    linelist_tmp_size = linelist_tmp.size();
	 for (j=0; j<linelist_tmp_size; j++) {
	    LineType curr_line_j = linelist_tmp.front();
	    linelist_tmp.pop_front();
	    cout <<"        [Line:"<<curr_line_j.name;
	    cout <<"]";
	 }
      }
      cout<<endl;
   }
}
//-------------------------------------------------------------------------
// Print the driver lines of the component of a particular out line.
void putLine(LineType given_line, list<LineType>linelist, 
	     map<LineType,LineList> line_to_inputline_map){
   
   list<LineType>::size_type i;
   const list<LineType>::size_type linelist_size = linelist.size();
   
   if (IN == given_line.direction || PI == given_line.type) {
      cout<<" [ERROR: NO INPUT LINES FOR THIS KIND OF LINE (TYPE PI/ DIR IN)!]"<<endl;
      return;
   }

   for (i=0; i<linelist_size;i++) {
      LineType curr_line = linelist.front();
      linelist.pop_front();
      if (curr_line.name != given_line.name) continue;
      //if (curr_line.direction == IN) continue;
      //if (curr_line.type == PI) continue;
      
      cout <<"        [Line:"<<curr_line.name;
      cout <<"] --> ";

      map<LineType,LineList>::iterator p = 
	 line_to_inputline_map.find (curr_line);
      if (p == line_to_inputline_map.end()) 
	 cout<<" [ERROR: NO INPUT MATCHING LINES !]"<<endl;
      else {
	 list<LineType> linelist_tmp = (p->second).mlines;
	 list<LineType>::size_type j=0, 
	    linelist_tmp_size = linelist_tmp.size();
	 for (j=0; j<linelist_tmp_size; j++) {
	    LineType curr_line_j = linelist_tmp.front();
	    linelist_tmp.pop_front();
	    cout <<" [Line:"<<curr_line_j.name;
	    cout <<"]";
	 }
      }
      cout<<endl;
   }
}
//-------------------------------------------------------------------------
// Print the driver lines of the component of all lines.
void putLine(list<LineType>linelist, 
	     map<LineType,LineList> line_to_inputline_map,
	     bool verbose){
   
   list<LineType>::size_type i;
   const list<LineType>::size_type linelist_size = linelist.size();

   for (i=0; i<linelist_size;i++) {
      LineType curr_line = linelist.front();
      linelist.pop_front();
      if (curr_line.direction == IN) continue;
      if (curr_line.type == PI) continue;
      
      cout <<"        [Line:"<<curr_line.name;
      cout <<" Val:"<<printValue(curr_line);
      cout <<" Dir:"<<printDir(curr_line);
      cout <<" Visited: "<<curr_line.isVisited;
      cout <<" Type:"<<printType(curr_line)<<"]";

      map<LineType,LineList>::iterator p = 
	 line_to_inputline_map.find (curr_line);
      //cout <<" #inputlines = "<<(p->second).mlines.size();
      if (p == line_to_inputline_map.end()) 
	 cout<<" [ERROR: NO MATCHING LINES !]"<<endl;
      else {
	 list<LineType> linelist_tmp = (p->second).mlines;
	 list<LineType>::size_type j=0, 
	    linelist_tmp_size = linelist_tmp.size();
	 for (j=0; j<linelist_tmp_size; j++) {
	    LineType curr_line_j = linelist_tmp.front();
	    linelist_tmp.pop_front();
	    cout <<"   [Line:"<<curr_line_j.name;
	    cout <<" Val:"<<printValue(curr_line_j);
	    cout <<" Dir:"<<printDir(curr_line_j);
	    cout <<" Visited: "<<curr_line_j.isVisited;
	    cout <<" Type:"<<printType(curr_line_j)<<"]";
	 }
      }
      cout<<endl;
   }
}
//-------------------------------------------------------------------------
// Print the driver lines of the component of a particular out line.
void putLine(LineType given_line, list<LineType>linelist, 
	     map<LineType,LineList> line_to_inputline_map,
	     bool verbose){
   
   list<LineType>::size_type i;
   const list<LineType>::size_type linelist_size = linelist.size();

   if (IN == given_line.direction || PI == given_line.type) {
      cout<<" [ERROR: NO INPUT LINES FOR THIS KIND OF LINE (TYPE PI/ DIR IN)!]"<<endl;
      return;
   }

   for (i=0; i<linelist_size;i++) {
      LineType curr_line = linelist.front();
      linelist.pop_front();
      if (curr_line.name != given_line.name) continue;
      //if (curr_line.direction == IN) continue;
      //if (curr_line.type == PI) continue;
      
      cout <<"        [Line:"<<curr_line.name;
      cout <<" Val:"<<printValue(curr_line);
      cout <<" Dir:"<<printDir(curr_line);
      cout <<" Visited: "<<curr_line.isVisited;
      cout <<" Type:"<<printType(curr_line)<<"]";

      map<LineType,LineList>::iterator p = 
	 line_to_inputline_map.find (curr_line);
      if (p == line_to_inputline_map.end()) 
	 cout<<" [ERROR: NO MATCHING LINES !]"<<endl;
      else {
	 list<LineType> linelist_tmp = (p->second).mlines;
	 list<LineType>::size_type j=0, 
	    linelist_tmp_size = linelist_tmp.size();
	 for (j=0; j<linelist_tmp_size; j++) {
	    LineType curr_line_j = linelist_tmp.front();
	    linelist_tmp.pop_front();
	    cout <<"        [Line:"<<curr_line_j.name;
	    cout <<" Val:"<<printValue(curr_line_j);
	    cout <<" Dir:"<<printDir(curr_line_j);
	    cout <<" Visited: "<<curr_line_j.isVisited;
	    cout <<" Type:"<<printType(curr_line_j)<<"]";
	 }
      }
      cout<<endl;
   }
}
//-------------------------------------------------------------------------
LineType LineType::DriverLine() {
   LineType line_tmp;
   return line_tmp;
}
//-------------------------------------------------------------------------
// To find the driver-line of an internal wire that is IN.
// All other cases, behaviour is undefined.
LineType LineType::DriverLine(map<LineType,string> line_to_net_map,
			      list<LineType>linelist){
   LineType line_dummy;
   //if (this->direction == OUT) return (*this); // May result in infi loops
   // Better this way..
   if (this->direction == OUT) return (line_dummy);
   if (this->type == PI)  return (line_dummy);
   //if (this->type == PO)  return (line_dummy); // PO has a driver line.

   map<LineType,string>::iterator p;
   p = line_to_net_map.find(*this);
   if (p == line_to_net_map.end()) cout<<" [NO MATCHING DRIVER NET !]"<<endl;
   else {
      string net_name = p->second;
      list<LineType>::size_type i, linelist_size=linelist.size();
      
      for (i=0; i<linelist_size; i++) {
	 LineType curr_line = linelist.front();
	 linelist.pop_front();
	 if (curr_line.name == this->name) continue; //skip checking the same line.
	 if (curr_line.direction == OUT) {
	    // if the direction is OUT and connected to same-net => driver line.
	    map<LineType,string>::iterator p;
	    p = line_to_net_map.find(curr_line);
	    if(net_name == (p->second)) return (curr_line);
	 }
      }
   }
   return line_dummy;
}
//-------------------------------------------------------------------------
//---// To find the driver-line of an internal wire that is IN.
//---// All other cases, behaviour is undefined.
//---LineType LineType::DriverLine(const map<LineType,string> &line_to_net_map,
//---			      list<LineType>linelist,
//---			      map<LineType,int> line_to_node_map){
//---   LineType line_dummy;
//---   if (direction == OUT) return (line_dummy);
//---   if (type == PI)  return (line_dummy);
//---   if (type == PO)  return (line_dummy);
//---
//---   //map<LineType,string>::iterator p;
//---   map<LineType,int>::iterator p;
//---   p = line_to_node_map.find(*this);
//---   //p = line_to_net_map.find(line_dummy);
//---   if (p == line_to_node_map.end()) cout<<" [NO MATCHING DRIVER NET !]"<<endl;
//---   if (false) return line_dummy;
//---   else {
//---      string net_name = p->second;
//---      for (list<LineType>::size_type i=0; i<linelist.size(); i++) {
//---	 LineType curr_line = linelist.front();
//---	 linelist.pop_front();
//---	 if (curr_line.name == name) continue; //skip checking the same line.
//---	 if (curr_line.direction == OUT) {
//---	    // if the direction is OUT and connected to same-net => driver line.
//---	    map<LineType,string>::iterator p;
//---	    p = line_to_net_map.find(curr_line);
//---	    if(net_name == (p->second)) return (curr_line);
//---	 }
//---      }
//---   }
//---   return line_dummy;
//---}
//-------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------
