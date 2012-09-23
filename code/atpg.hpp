//-------------------------------------------------------------------------
// File          : atpg.hpp
// Author        : Arun.C  <arunc@ee.iitb.ac.in>
//-------------------------------------------------------------------------
// The main ATPG generation algorithm class.
// Main functions are GenerateTP(), Justify() and Propagate().
// Lot of redundant and unnecessary code. 
// Need to cleanup thoroughly.
//-------------------------------------------------------------------------
#ifndef ATPG_H
#define ATPG_H

#include <iostream>
#include <new>
#include <cassert>
#include <iomanip>
using namespace std;

// To get the filesize. cplusplus.com :)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "buildGraph.hpp"
//-------------------------------------------------------------------------
class Atpg {
private:
   int msg_count;
public:
   //map<LineType, string> line_to_net_map;
   //list<LineType> linelist;
   
   Atpg (VerilogNode *, string, verilogValue);
   Atpg (VerilogNode *);
   Atpg (list<LineType>&);

   // const map as func argument is wrong.
   // Main func tht generates the ATPG.
   bool GenerateTP (VerilogNode *, 
		    LineType &, list<LineType>&,
		    map<LineType,int>, 
		    map<LineType,string>, 
		    map<LineType,LineList>,
		    map<LineType,LineList>,
		    map<LineType, LineList>,
		    verilogValue);

   bool Justify (VerilogNode *, 
		 LineType&, list<LineType> &,
		 map<LineType,int>, 
		 map<LineType,string>, 
		 map<LineType,LineList>,
		 verilogValue);

   inline void EqualizeFaninFanoutValues (LineType &line,
					  list<LineType> &);


   // Overloaded operator ^ for XOR of verilogValue's
   friend verilogValue operator^(const verilogValue v1, const verilogValue v2);
   friend verilogValue operator^(const verilogValue v1, const int v2);
   friend bool operator==(const verilogValue v1, const int v2);

   bool Propagate (VerilogNode *, 
		   LineType&, list<LineType>&,
		   map<LineType, int>, 
		   map<LineType, LineList>,
		   map<LineType, LineList>,
		   verilogValue);


   bool Propagate (VerilogNode *rnode, 
		   LineType &, list<LineType>&,
		   map<LineType, int>,
		   map<LineType, LineList>,
		   map<LineType, LineList>,
		   map<LineType, LineList>,
		   verilogValue error_val);


   void UpdateChanges (list<LineType>&, LineType);
   void UpdateChanges (LineType&, list<LineType>&);

   // Did not expect!!. Need to update for every modification.
   void UpdateMaps (VerilogNode *rnode, list<LineType> linelist,
		    map<LineType,int> &line_to_node_map, 
		    map<LineType,string> &line_to_net_map,
		    map<LineType,LineList> &line_to_fanoutline_map,
		    map<LineType,LineType> &line_to_faninline_map,
		    map<LineType,LineList> &line_to_inputline_map,
		    map<LineType,LineList> &line_to_outputline_map);


   ~Atpg() {}
};
//-------------------------------------------------------------------------
verilogValue operator^(const verilogValue v1, const verilogValue v2) {
   switch (v1) {
   case ZERO:
      switch (v2) {
      case ZERO: return ZERO;
      case ONE: return ONE;
      case X: return X;
      case U: return X; // U is essentially same as X. ?
      case D: return D;
      case D_BAR: return D_BAR;
      }
   case ONE:
      switch (v2) {
      case ZERO: return ONE;
      case ONE: return ZERO;
      case X: return X;
      case U: return X; // U is essentially same as X. ?
      case D: return D_BAR;
      case D_BAR: return D;
      }
   case X:
      switch (v2) {
      case ZERO: return X;
      case ONE: return X;
      case X: return X;
      case U: return X; // U is essentially same as X. ?
      case D: return X;
      case D_BAR: return X;
      }
   case U: // Treated same way as X. Essentially same as X. ?
      switch (v2) {
      case ZERO: return X;
      case ONE: return X;
      case X: return X;
      case U: return X;
      case D: return X;
      case D_BAR: return X;
      }
   case D:
      switch (v2) {
      case ZERO: return D;
      case ONE: return D_BAR;
      case X: return X;
      case U: return X; // U is essentially same as X. ?
      case D: return ZERO;
      case D_BAR: return ONE;
      }
   case D_BAR:
      switch (v2) {
      case ZERO: return D_BAR;
      case ONE: return D;
      case X: return X;
      case U: return X; // U is essentially same as X. ?
      case D: return ONE;
      case D_BAR: return ZERO;
      }
   }
   return U; // Never happens.
}
//-------------------------------------------------------------------------
verilogValue operator^(const verilogValue v1, const int v2) {
   switch (v1) {
   case ZERO:
      switch (v2) {
      case 0: return ZERO;
      case 1: return ONE;
      }
   case ONE:
      switch (v2) {
      case 0: return ONE;
      case 1: return ZERO;
      }
   case X:
      switch (v2) {
      case 0: return X;
      case 1: return X;
      }
   case U:
      switch (v2) {
      case 0: return X;
      case 1: return X;
      }
   case D:
      switch (v2) {
      case 0: return D;
      case 1: return D_BAR;
      }
   case D_BAR:
      switch (v2) {
      case 0: return D_BAR;
      case 1: return D;
      }
   }
   return U; // Never happens.
}
//-------------------------------------------------------------------------
bool operator==(const verilogValue v1, const int v2) {
   if ( (v1==ZERO) && (v2==0)) return true;
   if ( (v1==ONE) && (v2==1)) return true;
   return false;
}
//-------------------------------------------------------------------------
// Initialize all output values to X
// Modify isVisited arguments to false.
Atpg::Atpg (list<LineType> &linelist) {
   list<LineType>::iterator i=linelist.begin();
   for (; i != linelist.end(); i++) {
      i->value = X;
      i->isVisited = false;
   }
   msg_count = 0;
   cout <<"[Msg"<<msg_count++<<"] Initialzed all line values to X and isVisited = false"
	<<endl;
   //line_to_net_map = ::line_to_net_map;
}
//-------------------------------------------------------------------------
// TOO MANY ARGUMENTS. Recode everything.
static LineType error_line;
static LineType dummy_line;
bool Atpg::GenerateTP (VerilogNode *rnode, 
		       LineType &curr_line, 
		       list<LineType> &linelist,
		       map<LineType,int> line_to_node_map,
		       map<LineType,string> line_to_net_map,
		       map<LineType,LineList> line_to_inputline_map,
		       map<LineType,LineList> line_to_outputline_map,
		       map<LineType,LineList> line_to_fanoutline_map,
		       verilogValue val) {

   if (PI == curr_line.type || PO == curr_line.type) {
      cout<<"[ERROR: WRONG FAULT LINE PROVIDED FOR TEST GENERATION."
	  <<" GIVEN LINE-TYPE IS PI/PO]"<<endl;
      
   }
   cout <<"[Msg"<<msg_count++<<"] Current linelist status before Justify()"<<endl;
   putLine (linelist);cout<<endl;

   cout <<"[Msg"<<msg_count++<<"] Generating test patterns for line : ";
   putLine(curr_line); cout<<endl;
   //---cout <<"[Msg"<<msg_count++<<"] line_to_inputline_map before Justify() :"<<endl;
   //---putLine(linelist, line_to_inputline_map, true);

   //UpdateChanges (linelist, curr_line);
   LineType initial_line = curr_line;
   error_line = dummy_line;
   list<LineType> initial_linelist = linelist;
   map<LineType,int> initial_line_to_node_map = line_to_node_map;
   map<LineType,string> initial_line_to_net_map = line_to_net_map;
   map<LineType,LineList> initial_line_to_inputline_map = line_to_inputline_map;
   map<LineType,LineList> initial_line_to_outputline_map = initial_line_to_outputline_map;
   map<LineType,LineList> initial_line_to_fanoutline_map = line_to_fanoutline_map;
   
   //cout <<"[Msg] Generating test patterns for line : "<<putLine(curr_line)<<endl;
   bool justify_status = Justify (rnode, curr_line, linelist,
				  line_to_node_map, line_to_net_map, 
				  line_to_inputline_map, 
				  val);
   if (!justify_status) {

      linelist = initial_linelist;
      line_to_node_map = initial_line_to_node_map;
      line_to_net_map = initial_line_to_net_map;
      line_to_inputline_map = initial_line_to_inputline_map;
      initial_line_to_outputline_map =  initial_line_to_outputline_map;
      line_to_fanoutline_map = initial_line_to_fanoutline_map;
      curr_line = initial_line;

      bool justify_status = Justify (rnode, curr_line, linelist,
				     line_to_node_map, line_to_net_map, 
				     line_to_inputline_map, 
				     val);
      if (!justify_status) {
	 cout <<"[Msg"<<msg_count++<<"] Error: Test Pattern Generation FAILED. "<<endl;
	 cout <<"[Msg"<<msg_count++<<"] 0) Cannot justify line : ";
	 putLine (curr_line);
	 cout<<endl;
	 return false;
      }
   }

   cout <<"[Msg"<<msg_count++<<"] All lines justified "<<endl;
   UpdateChanges (linelist, curr_line);
   UpdateMaps (rnode, linelist,
	       line_to_node_map, line_to_net_map,
	       line_to_fanoutline_map,
	       line_to_faninline_map,
	       line_to_inputline_map,
	       line_to_outputline_map);
   
   cout <<"[Msg"<<msg_count++<<"] Current linelist status before Propagate()";
   cout <<" and after Justify()"<<endl;
   putLine (linelist);cout<<endl;
   //---cout <<"[Msg"<<msg_count++<<"] line_to_inputline_map before Propagate()";
   //---cout <<" and after Justify()"<<endl;
   //---putLine(linelist, line_to_inputline_map, true);
   
   curr_line = initial_line;
   if (val == ZERO) {
//--No need--      UpdateChanges (linelist, curr_line);
//--No need--      UpdateMaps (rnode, linelist,
//--No need--		  line_to_node_map, line_to_net_map,
//--No need--		  line_to_fanoutline_map,
//--No need--		  line_to_faninline_map,
//--No need--		  line_to_inputline_map,
//--No need--		  line_to_outputline_map);

      bool prop_status = Propagate (rnode, curr_line, linelist,
				    line_to_node_map,
				    line_to_outputline_map,
				    line_to_fanoutline_map,
				    line_to_inputline_map,
				    D);

      if (!prop_status)  {
	 linelist = initial_linelist;
	 line_to_node_map = initial_line_to_node_map;
	 line_to_net_map = initial_line_to_net_map;
	 line_to_inputline_map = initial_line_to_inputline_map;
	 initial_line_to_outputline_map =  initial_line_to_outputline_map;
	 line_to_fanoutline_map = initial_line_to_fanoutline_map;
	 curr_line = initial_line;

	 bool prop_status = Propagate (rnode, curr_line, linelist,
				       line_to_node_map,
				       line_to_outputline_map,
				       line_to_fanoutline_map,
				       line_to_inputline_map,
				       D);
	 if (!prop_status)  {
	    cout <<"[Msg"<<msg_count++<<"] Error: Test Pattern Generation FAILED. "<<endl;
	    cout <<"[Msg"<<msg_count++<<"] Cannot propagate line : ";
	    putLine (curr_line);
	    cout<<" with value D"<<endl;
	    return false;
	 }
      }
   }  else {   // if (val == ONE)
//--No need--      UpdateChanges (linelist, curr_line);
//--No need--      UpdateMaps (rnode, linelist,
//--No need--		  line_to_node_map, line_to_net_map,
//--No need--		  line_to_fanoutline_map,
//--No need--		  line_to_faninline_map,
//--No need--		  line_to_inputline_map,
//--No need--		  line_to_outputline_map);

      bool prop_status = Propagate (rnode, curr_line, linelist,
				   line_to_node_map, 
				   line_to_outputline_map,
				   line_to_fanoutline_map,
				   line_to_inputline_map, 
				   D_BAR);
      if (!prop_status)  {

	 linelist = initial_linelist;
	 line_to_node_map = initial_line_to_node_map;
	 line_to_net_map = initial_line_to_net_map;
	 line_to_inputline_map = initial_line_to_inputline_map;
	 initial_line_to_outputline_map =  initial_line_to_outputline_map;
	 line_to_fanoutline_map = initial_line_to_fanoutline_map;
	 curr_line = initial_line;

	 bool prop_status = Propagate (rnode, curr_line, linelist,
				       line_to_node_map, 
				       line_to_outputline_map,
				       line_to_fanoutline_map,
				       line_to_inputline_map, 
				       D_BAR);
	 if (!prop_status)  {
	    cout <<"[Msg"<<msg_count++<<"] Test Pattern Generation FAILED. "<<endl;
	    cout <<"[Msg"<<msg_count++<<"] Cannot propagate line : ";
	    putLine (curr_line);
	    cout<<" with value D_BAR"<<endl;
	    return false;
	 }
      }
   }

   cout <<"[Msg"<<msg_count++<<"] Current linelist status after Propagate()"<<endl;
   putLine (linelist);cout<<endl;

   UpdateChanges (linelist, curr_line);
   UpdateMaps (rnode, linelist,
	       line_to_node_map, line_to_net_map,
	       line_to_fanoutline_map,
	       line_to_faninline_map,
	       line_to_inputline_map,
	       line_to_outputline_map);

   cout <<"[Msg"<<msg_count++
	<<"] Current linelist status after pattern generation()"<<endl;
   putLine (linelist);cout<<endl;
   //---cout <<"[Msg"<<msg_count++<<"] line_to_inputline_map after pattern generation()"
   //---<<endl;
   //---putLine(linelist, line_to_inputline_map, true);
   
   return true;
}
//-------------------------------------------------------------------------
// recode. split separately IN and OUT.
//
static int error_count = 0;
bool Atpg::Justify (VerilogNode *rnode, 
		    LineType &curr_line, 
		    list<LineType> &linelist,
		    map<LineType,int> line_to_node_map,
		    map<LineType,string> line_to_net_map,
		    map<LineType,LineList> line_to_inputline_map,
		    verilogValue val) {

   cout <<"[Msg"<<msg_count++<<"] Justifying line : ";
   putLine(curr_line); cout <<" with value "<<printValue(val);cout<<endl;

   if (error_count++ == 200) { // To get rid of infinite loop
      cout <<"STOPS AT 200 faults"<<endl;
      assert(false);
   }

//----   UpdateChanges (linelist, curr_line);
//----   UpdateMaps (rnode, linelist,
//----	       line_to_node_map, line_to_net_map,
//----	       line_to_fanoutline_map,
//----	       line_to_faninline_map,
//----	       line_to_inputline_map,
//----	       line_to_outputline_map);
   //----curr_line.value = val;
   //----curr_line.isVisited = true;
   //----EqualizeFaninFanoutValues (curr_line, linelist);
   //UpdateChanges (curr_line, linelist);
//----   UpdateChanges (linelist, curr_line);
//----   UpdateMaps (rnode, linelist,
//----	       line_to_node_map, line_to_net_map,
//----	       line_to_fanoutline_map,
//----	       line_to_faninline_map,
//----	       line_to_inputline_map,
//----	       line_to_outputline_map);
   
   cout <<"Here "<<error_count<<endl;
   //cout<<endl<<endl;putLine(linelist, true);
   // to statisfy curr_line is a gate o/p when not PI... :)
   if (curr_line.direction == IN) 
      curr_line = curr_line.DriverLine(line_to_net_map, linelist);

   if (curr_line.type == PI) {
      if (curr_line.isVisited) {
	 if (curr_line.value == val) {
	    cout <<"[Msg"<<msg_count++<<"] 2) Line : ";	       
	    putLine(curr_line); cout <<" already justified"
				<<" with value"<<printValue(val)<<endl;
	    return true;
	 }
	 // if already visited and value is opposite.
	 cout <<"[Msg"<<msg_count++<<"] 1) Cannot justify line : ";
	 putLine (curr_line);
	 cout<<endl;
	 return false;
      }
      curr_line.value = val;
      curr_line.isVisited = true;
      EqualizeFaninFanoutValues (curr_line, linelist);

      curr_line.isVisited = true;
      UpdateChanges (linelist, curr_line);
      UpdateMaps (rnode, linelist,
		  line_to_node_map, line_to_net_map,
		  line_to_fanoutline_map,
		  line_to_faninline_map,
		  line_to_inputline_map,
		  line_to_outputline_map);
      cout <<"[Msg"<<msg_count++<<"] Concluding. Reached PI line : ";
      putLine(curr_line);cout<<endl;
      return true;
   }
   // curr_line is a gate output. Hw come!!!


   //----if (curr_line.isVisited) {
   //----   if (curr_line.value == val) {
   //----	 cout <<"[Msg"<<msg_count++<<"] 2) Line : ";	       
   //----	 putLine(curr_line); cout <<" already justified"
   //----				  <<" with value"<<printValue(inval)<<endl;
   //----	 return true;
   //----   }
   //----   return false;
   //----}

   curr_line.value = val;
   curr_line.isVisited = true;
   EqualizeFaninFanoutValues (curr_line, linelist);

   //putLine (curr_line);
   map<LineType,int>::iterator p;
   p = line_to_node_map.find(curr_line);
   if (p == line_to_node_map.end()) {
      cout<<" [NO MATCHING COMPONENT TO JUSTIFY!]"<<endl;
      return false;
   }
   int nodeID = p->second;
   VerilogNode node_tmp = rnode[nodeID];
   int cv = node_tmp.cv;
   int iv = node_tmp.inv_parity;
   verilogValue inval = val ^ iv;
   
   //cout <<"inval = "<<printValue(inval)<<" !cv = "<<!cv<<endl;
   if (inval == !cv) { // Justify all the input lines.
      cout <<"[Msg"<<msg_count++<<"] inval = "<<printValue(inval)
	   <<"] Justifying all input lines for the line : ";
      putLine(curr_line);cout<<endl;

      map<LineType,LineList>::iterator p;
      p = line_to_inputline_map.find(curr_line);
      if (p==line_to_inputline_map.end()) {
	 cout<<"[ERROR: NO MATCHING INPUT LINES FOR THE FAULT LINE!]";
	 return false;
      }

      list<LineType> linelist_tmp = (p->second).mlines;
      list<LineType>::iterator i=linelist_tmp.begin();
//---      cout <<"[Msg"<<msg_count++<<"] No:of input lines to be justified : "
//---	   <<(int)linelist_tmp.size()<<endl;
      cout <<"[Msg"<<msg_count++<<"] Following lines need to be justified. (Total "
	   <<(int)linelist_tmp.size()<<" lines)"<<endl;
      putLine (curr_line, linelist, line_to_inputline_map);

      for (; i != linelist_tmp.end(); i++) {
	 
	 if ((*i) == error_line) continue;
	 if (i->value == inval) {
	    cout <<"[Msg"<<msg_count++<<"] 1) Line : ";	       
	    putLine((*i)); cout <<" already justified"
				<<" with value"<<printValue(inval)<<endl;
	 } else {
	    if (i->isVisited) {
	       cout <<"[ERROR: ALL INPUT LINES NEED TO BE JUSTIFIED. CANNOT JUSTIFY ";
	       cout <<"LINE : ";putLine(curr_line);
	       cout <<" TO VALUE "<<printValue(inval)
		    <<" . ALREADY JUSTIFIED TO OPPOSITE VALUE]"<<endl;
	       return false;
	    }

	    bool justify_status = Justify (rnode, (*i), linelist,
					   line_to_node_map, line_to_net_map, 
					   line_to_inputline_map,
					   inval);
	    if (!justify_status) {
	       cout <<"[Msg"<<msg_count++<<"] 2) Cannot justify line : ";
	       putLine (curr_line);
	       cout<<endl;
	       return false;
	    }
	 }
	 UpdateChanges (linelist, curr_line);
	 UpdateMaps (rnode, linelist,
		     line_to_node_map, line_to_net_map,
		     line_to_fanoutline_map,
		     line_to_faninline_map,
		     line_to_inputline_map,
		     line_to_outputline_map);
	 
      }
   } //if !(inval == !cv)
   else { // Select one of the input lines and justify
      cout <<"[Msg"<<msg_count++<<"] One of the input lines of line : ";
      putLine(curr_line); cout<<" to be justified"<<endl;
      map<LineType,LineList>::iterator p;
      p = line_to_inputline_map.find(curr_line);
      if (p==line_to_inputline_map.end()){
	 cout<<"[ERROR: NO MATCHING INPUT LINES FOR THE FAULT LINE!]";
	 return false;
      }
      list<LineType> linelist_tmp = (p->second).mlines;
      LineType curr_line_j; 
      //LineType curr_line_j = linelist_tmp.front();
      //linelist_tmp.pop_front();
      //cout <<"First input"<<endl;putLine(curr_line_j);
      // XXX Need to put proper looping here.
      while (!linelist_tmp.empty()) {
	 curr_line_j = linelist_tmp.front();
	 linelist_tmp.pop_front();
	 
	 if (curr_line_j.value == inval) {
	    cout <<"[Msg"<<msg_count++<<"] Only ";
	    putLine(curr_line_j); 
	    cout<<" of line : "<<curr_line.name<<"to be justified"<<endl;
	    cout <<"[Msg"<<msg_count++<<"] Line : ";	       
	    putLine(curr_line_j); cout<<" already justified"<<endl;
	    return true;
	 }
	 cout <<"[Msg"<<msg_count++<<"] Trying line : ";
	 putLine(curr_line_j); cout<<" to justify"<<endl;

	 if (curr_line_j.name == error_line.name) {
	    cout <<"[Msg"<<msg_count++<<"] 4) Causes Justification problems further : ";
	    putLine(curr_line_j); 
	    cout <<" Trying next line"<<endl;
	    continue;
	 }
	 if (curr_line_j.isVisited) {
	    cout <<"[Msg"<<msg_count++<<"] 3) Cannot Justify line : ";
	    putLine(curr_line_j); 
	    cout <<" - already visited and justified with opposite value.";
	    cout <<" Trying next line"<<endl;
	    continue;
	 }
	 // Case whr the line is NOT visited.
	 cout <<"[Msg"<<msg_count++<<"] Only ";
	 putLine(curr_line_j); 
	 cout<<" of line : "<<curr_line.name<<" to be justified"<<endl;
	 
	 error_line = curr_line_j; // save for future backtracking.
	 cout <<"[Msg"<<msg_count++<<"] Saving ";
	 putLine(error_line); 
	 cout<<" for future backtracking."<<endl;
	 
	 bool justify_status = Justify (rnode, curr_line_j, linelist,
					line_to_node_map, line_to_net_map, 
					line_to_inputline_map,
					inval);
	 if (justify_status) {
	    UpdateChanges (linelist, curr_line);
	    UpdateMaps (rnode, linelist,
			line_to_node_map, line_to_net_map,
			line_to_fanoutline_map,
			line_to_faninline_map,
			line_to_inputline_map,
			line_to_outputline_map);
	    return true;
	 }
	 if (linelist_tmp.empty()) {
	    cout <<"[ERROR: NO MORE LINES TO TRY. CANNOT JUSTIFY ";
	    cout <<"LINE : ";putLine(curr_line);
	    cout <<" TO VALUE "<<printValue(inval)<<" ]"<<endl;
	    return false;
	 }
      }
      
      
//----      cout <<"[Msg"<<msg_count++<<"] Only ";
//----      putLine(curr_line_j); 
//----      cout<<" of line : "<<curr_line.name<<" to be justified"<<endl;
//----      
//----      Justify (rnode, curr_line_j, linelist,
//----	       line_to_node_map, line_to_net_map, line_to_inputline_map,
//----	       inval);
//----      UpdateChanges (linelist, curr_line);
//----      UpdateMaps (rnode, linelist,
//----		  line_to_node_map, line_to_net_map,
//----		  line_to_fanoutline_map,
//----		  line_to_faninline_map,
//----		  line_to_inputline_map,
//----		  line_to_outputline_map);
      
   } //if !(inval == !cv)
   
//----   UpdateChanges (linelist, curr_line);
//----   UpdateMaps (rnode, linelist,
//----	       line_to_node_map, line_to_net_map,
//----	       line_to_fanoutline_map,
//----	       line_to_faninline_map,
//----	       line_to_inputline_map,
//----	       line_to_outputline_map);
   
   return true;
}
//-------------------------------------------------------------------------
inline void Atpg::EqualizeFaninFanoutValues (LineType &line, 
					     list<LineType>&linelist) {
   cout <<"[Msg"<<msg_count++<<"] Equalizing fanin/fanout values line : ";
   putLine(line);cout<<endl;

   if (line.direction == IN) {
      verilogValue val = line.value;
      LineType driver_line = line.DriverLine(line_to_net_map, linelist);
      for (list<LineType>::iterator i=linelist.begin(); i!=linelist.end();i++) {
	 if ((*i).name == driver_line.name) i->value  = val; // use i->name
      }
   }
   if (line.type == PO) {
      verilogValue val = line.value;
      LineType driver_line = line.DriverLine(line_to_net_map, linelist);
      for (list<LineType>::iterator i=linelist.begin(); i!=linelist.end();i++) {
	 if ((*i).name == driver_line.name) i->value  = val; // use i->name
      }
   }
   
}
//-------------------------------------------------------------------------
bool Atpg::Propagate (VerilogNode *rnode, 
		      LineType &curr_line, 
		      list<LineType> &linelist,
		      map<LineType, int>line_to_node_map, 
		      map<LineType, LineList> line_to_outputline_map,
		      map<LineType, LineList> line_to_fanoutline_map,
		      map<LineType, LineList> line_to_inputline_map,
		      verilogValue error_val) {

   cout <<"[Msg"<<msg_count++<<"] Propagating "<<printValue(error_val)
	<<" for the line : "<<curr_line.name<<endl;

   // error_val is D or D_BAR.
   curr_line.value = error_val;
   //EqualizeFaninFanoutValues (curr_line, linelist); Shud NOT equalize.
   UpdateChanges (linelist, curr_line);
   UpdateMaps (rnode, linelist,
	       line_to_node_map, line_to_net_map,
	       line_to_fanoutline_map,
	       line_to_faninline_map,
	       line_to_inputline_map,
	       line_to_outputline_map);

   cout <<"[Msg"<<msg_count++<<"] Updated linelist at this stage. ";
   cout <<"- About to start  Propagate()"<<endl;
   putLine(linelist, true);

   // Now there are 2 cases. (Nt given in algo). XXX
   // case 1: curr_line cud be a PO line
   if (curr_line.type == PO) {
      cout <<"[Msg"<<msg_count++<<"] Concluding. Reached PO line : ";
      putLine(curr_line);cout<<endl;
      return true;
   }
   // case 2: one of the fanout of the curr_line is PO, if curr_line.dir=OUT
   if (curr_line.direction == OUT) {
      map<LineType,LineList>::iterator fanout_p;
      fanout_p = line_to_fanoutline_map.find(curr_line);
      if (fanout_p == line_to_fanoutline_map.end()){
	 cout<<"[ERROR: NO MATCHING FANOUT LINES FOR THIS LINE!]";
	 return false;
      }

      list<LineType> linelist_tmp = (fanout_p->second).mlines;
      for (list<LineType>::iterator i=linelist_tmp.begin();
	   i != linelist_tmp.end(); i++) {
	 if (PO == i->type) {
	    cout <<"[Msg"<<msg_count++<<" Concluding. Reached PO line : ";
	    putLine((*i));cout<<endl;
	    return true;
	 }
      }
   }
   
   // Neither of the above cases. Need to justify and propagate further.
   map<LineType,int>::iterator p = line_to_node_map.find(curr_line);
   if (p == line_to_node_map.end()) {
      cout<<" [NO MATCHING COMPONENT TO PROPAGATE!]"<<endl;      
      return false;
   }
   int nodeID = p->second;
   VerilogNode node_tmp = rnode[nodeID];
   int iv = node_tmp.inv_parity;
   int cv = node_tmp.cv;

   if (curr_line.direction == IN) {
      //-- First part, if its an input line to a component.
      //-- Using line_to_outputline_map
      // Check here... XXX

      cout <<"[Msg"<<msg_count++<<"]";putLine(curr_line);
      cout <<" is an input line to the component"<<endl;

      string line_out_name = node_tmp.component_name +"_"+ 
	 node_tmp.output_list.front();
      
      cout <<"[Msg"<<msg_count++<<"] Corresponding output line : ";
      cout <<line_out_name<<endl;

      LineType line_out;
      for (list<LineType>::iterator i=linelist.begin(); i != linelist.end(); i++){
	 if ((*i).name == line_out_name) line_out = (*i);
      }
      //UpdateChanges (linelist, line_out);
      
      map<LineType, LineList>::iterator p = line_to_inputline_map.find(line_out);
      if (p == line_to_inputline_map.end()) {
	 cout <<"[ERROR: NO MATCHING INPUT LINES FOR THE LINE.!]"<<endl;
	 return false;
      }
      list<LineType> linelist_i = (p->second).mlines;
      for (list<LineType>::iterator i=linelist_i.begin(); i != linelist_i.end(); i++){
	 cout <<"[Msg"<<msg_count++<<"] ";
	 cout <<"Checking "<<i->name<<" to be justified or not"<<endl;
	 if ((*i) == curr_line) continue; // skip the current line
	 // Justify all other input lines to ncv.
	 verilogValue cv_bar;
	 if (cv == 0) cv_bar = ONE;
	 else if (cv == 1) cv_bar = ZERO;
	 else { // Should never come.
	    cout <<"[ERROR: Controlling Value of the gate NOT correct]"<<endl;
	    cv_bar = U;
	 }

	 cout <<"[Msg"<<msg_count++<<"] ";
	 cout <<"Justify "<<i->name<<" inside Propagate. line.direction == IN";
	 cout <<" The line : ";putLine ((*i)); cout<<endl;

	 bool justify_status = Justify (rnode, (*i), linelist,
					line_to_node_map, line_to_net_map, 
					line_to_inputline_map, 
					cv_bar);
	 if (!justify_status) { // Need to startover.
	    cout <<"[Msg"<<msg_count++<<"] 5) Inside propagate. Cannot Justify line : ";
	    putLine((*i)); 
	    return false;
	 }
      }

      // Upto here, all the other inputs will be justified to NCV.
      // Now propagate the error.
      // XXX. 
      line_out.value = error_val; // Nt required. Set as first step in Propagate()
      EqualizeFaninFanoutValues (line_out, linelist);
      UpdateChanges (linelist, line_out);
      UpdateMaps (rnode, linelist,
		  line_to_node_map, line_to_net_map,
		  line_to_fanoutline_map,
		  line_to_faninline_map,
		  line_to_inputline_map,
		  line_to_outputline_map);

      cout <<"[Msg"<<msg_count++<<"] ";
      cout <<"All i/p lines justified for "<<line_out.name<<" to Propagate."
	   <<" The line : ";putLine(line_out);cout<<endl;

      cout <<"[Msg"<<msg_count++<<"] Current linelist status inside Propagate()"; 
      cout <<" after all input line justification";
      putLine(linelist, true);

      //Now take the fanoutlines of line_out and Propagate()
      map<LineType,LineList>::iterator fanout_p;
      fanout_p = line_to_fanoutline_map.find(line_out);
      if (fanout_p == line_to_fanoutline_map.end()){
	 cout<<"[ERROR: NO MATCHING FANOUT LINES FOR THE FAULT LINE!]";
	 return false;
      }
      else {
	 list<LineType> linelist_tmp = (fanout_p->second).mlines;
	 list<LineType>::iterator it = linelist_tmp.begin();
	 for (; it != linelist_tmp.end(); it++) {
	    LineType fanout_line_j = (*it);
	    //LineType fanout_line_j = linelist_tmp.front();
	    //linelist_tmp.pop_front();
	 
	    cout <<"[Msg"<<msg_count++<<"] error_val :"<<printValue(error_val)
		 <<" and iv : "<<iv<<endl;
	    cout <<"[Msg"<<msg_count++<<"] Input line : ";
	    putLine(fanout_line_j);
	    cout<<" is propagated with val "
		<<printValue(error_val^iv)<<" (error_val^iv)"<<endl;
	    
	    bool prop_status = Propagate (rnode, fanout_line_j, linelist,
					  line_to_node_map, 
					  line_to_outputline_map,
					  line_to_fanoutline_map,
					  line_to_inputline_map,
					  (error_val^iv));
	    if (prop_status) return true; 
	    //else propagate the next fanoutline.
	    cout <<"[Msg"<<msg_count++<<" Failed to propagate line : ";
	    putLine(fanout_line_j);cout<<". Trying next fanoutline"<<endl;
	 }
	 // Reached here, means cud nt propagate at all.
	 cout <<"[Msg"<<msg_count++<<" Could not propagate line : ";
	 putLine(line_out);cout<<" through any of the outputlines"<<endl;
	 return false;
      }
   }
   else {
      //-- Second part, if its an output line from a component.
      //-- Using line_to_fanoutline_map.
      LineType line_out = curr_line;
      //---map<LineType, LineList>::iterator p = line_to_inputline_map.find(line_out);
      //---if (p == line_to_inputline_map.end()) {
      //---	 cout <<"[ERROR: NO MATCHING INPUT LINES FOR THE LINE.!]"<<endl;
      //---	 return false;
      //---}
      //---list<LineType> linelist_i = (p->second).mlines;
      //---for (list<LineType>::iterator i=linelist_i.begin(); i != linelist_i.end(); i++){
      //---	 if ((*i) == curr_line) continue; // skip the current line. Does nt happen.
      //---	 // Justify all other input lines to ncv.
      //---	 verilogValue cv_bar;
      //---	 if (cv == 0) cv_bar = ONE;
      //---	 else if (cv == 1) cv_bar = ZERO;
      //---	 else { // Should never come.
      //---	    cout <<"[ERROR: Controlling Value of the gate NOT correct]"<<endl;
      //---	    cv_bar = U;
      //---	 }
      //---	 Justify (rnode, (*i), linelist,
      //---		  line_to_node_map, line_to_net_map, line_to_inputline_map, 
      //---		  cv_bar);
      //---}

      // Upto here, all the other inputs will be justified to NCV.
      // Now propagate the error.
      // XXX. 
      line_out.value = error_val; // Nt required. Set as first step in Propagate()
      EqualizeFaninFanoutValues (line_out, linelist); // Nt required. Nt intented for this.
      UpdateChanges (linelist, line_out); // Nt required. Set as first step in Propagate().
      UpdateMaps (rnode, linelist,
		  line_to_node_map, line_to_net_map,
		  line_to_fanoutline_map,
		  line_to_faninline_map,
		  line_to_inputline_map,
		  line_to_outputline_map);

      //Now take the fanoutlines of line_out and Propagate()
      map<LineType,LineList>::iterator fanout_p;
      fanout_p = line_to_fanoutline_map.find(line_out);
      if (fanout_p == line_to_fanoutline_map.end()){
	 cout<<"[ERROR: NO MATCHING FANOUT LINES FOR THE FAULT LINE!]";
	 return false;
      }
      list<LineType> linelist_tmp = (fanout_p->second).mlines;
      // Take first line. That is enough.!! XXX
      list<LineType>::iterator it = linelist_tmp.begin();
      for (; it != linelist_tmp.end(); it++) {
	 //LineType fanout_line_j = linelist_tmp.front();
	 //linelist_tmp.pop_front();
	 
	 LineType fanout_line_j = (*it);
	 cout <<"[Msg"<<msg_count++<<" Input line  ";
	 putLine(fanout_line_j);cout<<" is propagated"<<endl;
	 
	 bool prop_status = Propagate (rnode, fanout_line_j, linelist,
				       line_to_node_map, 
				       line_to_outputline_map,
				       line_to_fanoutline_map,
				       line_to_inputline_map,
				       (error_val^iv));
	 if (prop_status) return true; 
	 //else propagate the next fanoutline.
	 cout <<"[Msg"<<msg_count++<<" Failed to propagate line : ";
	 putLine(fanout_line_j);cout<<". Trying next fanoutline"<<endl;
      }
      // Reached here, means cud nt propagate at all.
      cout <<"[Msg"<<msg_count++<<" Could not propagate line : ";
      putLine(line_out);cout<<" through any of the outputlines"<<endl;
      return false;
   }
   return true;
}
//-------------------------------------------------------------------------
// Update changes in line to linelist.
//
// Update the linelist with the curr_line entry.
// ie, modify the line in the linelist as given in the curr_line,
//     if the names match with an entry in the linelist.
// recode. use i->name XXX
void Atpg::UpdateChanges (list<LineType> &linelist, LineType curr_line) {

   cout <<"[Msg"<<msg_count++<<"] Updating linelist with line : ";
   putLine(curr_line);cout<<endl;
   
   for(list<LineType>::iterator i=linelist.begin(); i!=linelist.end(); i++) {
      if ((*i).name == curr_line.name) {
	 //i = curr_line; Confused. taking safe approach.
	 (*i).value = curr_line.value;
	 if ((*i).direction != curr_line.direction)
	    cout <<"[ERROR: FAULT LINE DIRECTION CHANGED IN BETWEEN !]"<<endl;
	 (*i).direction = curr_line.direction; // This should never change, still.

	 if ((*i).type != curr_line.type)
	    cout <<"[ERROR: FAULT LINE TYPE CHANGED IN BETWEEN !]"<<endl;
	 (*i).type = curr_line.type; // This should never change still.
	 
	 i->isVisited = curr_line.isVisited;
      }
   }
}
//-------------------------------------------------------------------------
// Update changes in linelist to line.
// recode. use i->name XXX
void Atpg::UpdateChanges (LineType &curr_line, list<LineType> &linelist) {
   
   cout <<"[Msg"<<msg_count++<<"] Updated line : "<<curr_line.name;
   cout<<" from linelist to ";

   for(list<LineType>::iterator i=linelist.begin(); i!=linelist.end(); i++) {
      if ((*i).name == curr_line.name) {
	 curr_line.value = (*i).value;
	 if ((*i).direction != curr_line.direction)
	    cout <<"[ERROR: FAULT LINE DIRECTION CHANGED IN BETWEEN !]"<<endl;
	 curr_line.direction = (*i).direction; // This should never change, still.

	 if ((*i).type != curr_line.type)
	    cout <<"[ERROR: FAULT LINE TYPE CHANGED IN BETWEEN !]"<<endl;
	 curr_line.type = (*i).type ; // This should never change still.
	 curr_line.isVisited = i->isVisited;
      }
   }
   putLine(curr_line); cout<<endl;
}
//-------------------------------------------------------------------------
// Helper functions.
// Print the verilogValue at PI's
void PutPiPatterns (list<LineType> linelist) {
   cout <<"[Msg] Printing PI patterns "<<endl;
   for (list<LineType>::iterator i=linelist.begin(); i != linelist.end(); i++) {
      if ((i->type) == PI) 
	 cout <<"     "<<(*i).name<<" "<<printValue(*i)<<endl;
   }
}
//-------------------------------------------------------------------------
// Print the verilogValue at PI's to a file
static bool firsttime = true;
static int width_count = 0;
static int fault_count = 0;
void PutPiPatterns (list<LineType> linelist, LineType given_line, 
		    string faulttype, char* filename) {
   //---// Check filesize. If its an empty file write the header.
   //---struct stat filestatus;
   //---stat (filename, &filestatus);
   //---bool firsttime = false;
   //---if (0 == filestatus.st_size) firsttime = true;
   fstream faultfile;
   string demark;
   string::size_type indx=0; 
   char demarkchar = '-';

   //faultfile.open (filename, fstream::out | fstream::app);
   //faultfile.width (5);
   if (firsttime) { // fstream::trunc to discard existing content.
      faultfile.open (filename, fstream::trunc | fstream::out); 
      cout <<"here file"<<endl;
      faultfile <<"   # |"<<setw(20)<<"FaultLine"<<" |"<<" Type |";
      width_count = 35;
      list<LineType> linelist_tmp = linelist;
      list<LineType>::iterator it;
      for (it = linelist_tmp.begin(); it != linelist_tmp.end(); it++) {
	 if (PI == it->type) {
	    faultfile<<setw(10)<<it->name<<" |";
	    width_count = width_count+10+2;
	 }
      }
      //faultfile <<setfill('-')<<setw(width_count)<<endl;
      faultfile <<endl;
      //demark.insert (indx, (string::size_type)width_count, demarkchar);
      //faultfile <<demark<<endl;
      faultfile.close();
   }
   firsttime = false;

   fault_count++;
   faultfile.open (filename, fstream::out | fstream::app);
   demark.insert (indx, (string::size_type)width_count, demarkchar);
   faultfile <<demark<<endl;
   faultfile<<setw(4)<<fault_count<<" |"
	    <<setw(20)<<given_line.name<<" |"<<setw(5)<<faulttype<<" |";
   for (list<LineType>::iterator i=linelist.begin(); i != linelist.end(); i++) {
      if ((i->type) == PI)  {
	 //faultfile <<setw(20)<<i->name;
	 faultfile <<setw(10)<<printValue(*i)<<" |";
      }
   }
   faultfile<<endl;
   faultfile.close();
   // Opening and closing file for every fault is junk. Used for the time being.
}
//-------------------------------------------------------------------------
// Print the verilogValue at PO's
void PutPoPatterns (list<LineType> linelist) {
   cout <<"[Msg] Printing PO patterns "<<endl;
   for (list<LineType>::iterator i=linelist.begin(); i != linelist.end(); i++) {
      if ((i->type) == PO) 
	 cout <<"     "<<(*i).name<<" "<<printValue(*i)<<endl;
   }

}
//-------------------------------------------------------------------------
// JUNK!!!...
// Did not expect this mess.. :(
//
void Atpg::UpdateMaps (VerilogNode *rnode, list<LineType> linelist,
		       map<LineType,int> &line_to_node_map, 
		       map<LineType,string> &line_to_net_map,
		       map<LineType,LineList> &line_to_fanoutline_map,
		       map<LineType,LineType> &line_to_faninline_map,
		       map<LineType,LineList> &line_to_inputline_map,
		       map<LineType,LineList> &line_to_outputline_map) {

   cout <<"[Msg"<<msg_count++<<"] Updating the maps "<<endl;

   for (list<LineType>::iterator i=linelist.begin(); i != linelist.end(); i++){
      LineType curr_line_i = (*i);

      // line_to_node_map
      for (map<LineType,int>::iterator j=line_to_node_map.begin();
	   j != line_to_node_map.end(); j++) {
	 LineType curr_line_j = j->first;
	 int nodeID = j->second;
	 if (curr_line_j.name == curr_line_i.name) {
	    line_to_node_map.erase(curr_line_j);
	    line_to_node_map.insert(make_pair(curr_line_i, nodeID));
	 }
      }

      // line_to_net_map
      for (map<LineType,string>::iterator j=line_to_net_map.begin();
	   j != line_to_net_map.end(); j++) {
	 LineType curr_line_j = j->first;
	 string netname = j->second;
	 if (curr_line_j.name == curr_line_i.name) {
	    line_to_net_map.erase(curr_line_j);
	    line_to_net_map.insert(make_pair(curr_line_i, netname));
	 }
      }
      //----------------------------------------------------------------
      // line_to_fanoutline_map
      for (map<LineType,LineList>::iterator j=line_to_fanoutline_map.begin();
	   j != line_to_fanoutline_map.end(); j++) {
	 LineType curr_line_j = j->first;
	 LineList fanoutlist = j->second;

	 // Update the fanoutlist first.
	 list<LineType> linelist_k = linelist;
	 list<LineType> fanoutlist_tmp = fanoutlist.mlines;
	 list<LineType> fanoutlist_k;
	 list<LineType>::iterator k=fanoutlist_tmp.begin();
	 fanoutlist_k.clear();// Not really needed.
	 for (; k != fanoutlist_tmp.end(); k++) {
	    list<LineType>::iterator l=linelist_k.begin();
	    for (; l != linelist_k.end(); l++) {
	       if (k->name == l->name)  
		  fanoutlist_k.push_front( (*l) );
	    }
	 }
	 fanoutlist.mlines = fanoutlist_k; // Replace with the newly populuated list.
	    
	 // Now update the map.
	 if (curr_line_j.name == curr_line_i.name) {
	    line_to_fanoutline_map.erase(curr_line_j);
	    line_to_fanoutline_map.insert(make_pair(curr_line_i, fanoutlist));
	 }
      }

      //----------------------------------------------------------------
      // line_to_faninline_map
      for (map<LineType,LineType>::iterator j=line_to_faninline_map.begin();
	   j != line_to_faninline_map.end(); j++) {
	 LineType curr_line_j = j->first;
	 LineType fanin = j->second;

	 // Update the fanin first.
	 list<LineType> linelist_k = linelist;
	 list<LineType>::iterator k=linelist_k.begin();
	 for (; k != linelist_k.end(); k++) {
	    if (k->name == fanin.name)  {
	       fanin = (*k); continue;
	    }
	 }
	 
	 // Now update the map.
	 if (curr_line_j.name == curr_line_i.name) {
	    line_to_faninline_map.erase(curr_line_j);
	    line_to_faninline_map.insert(make_pair(curr_line_i, fanin));
	 }
      }
      //----------------------------------------------------------------
      // line_to_inputline_map
      for (map<LineType,LineList>::iterator j=line_to_inputline_map.begin();
	   j != line_to_inputline_map.end(); j++) {
	 LineType curr_line_j = j->first;
	 LineList inputlines = j->second;

	 // Update the inputlines first.
	 list<LineType> linelist_k = linelist;
	 list<LineType> inputlines_tmp = inputlines.mlines;
	 list<LineType> inputlines_new; // new list
	 list<LineType>::iterator k=inputlines_tmp.begin();
	 inputlines_new.clear();// Not really needed.
	 for (; k != inputlines_tmp.end(); k++) {
	    list<LineType>::iterator l=linelist_k.begin();
	    for (; l != linelist_k.end(); l++) {
	       if (k->name == l->name)  
		  inputlines_new.push_front( (*l) );
	    }
	 }
	 inputlines.mlines = inputlines_new; // Replace with the newly populuated list.
	    
	 // Now update the map.
	 if (curr_line_j.name == curr_line_i.name) {
	    line_to_inputline_map.erase(curr_line_j);
	    line_to_inputline_map.insert(make_pair(curr_line_i, inputlines));
	 }
      }
      //----------------------------------------------------------------
      // line_to_outputline_map
      for (map<LineType,LineList>::iterator j=line_to_outputline_map.begin();
	   j != line_to_outputline_map.end(); j++) {
	 LineType curr_line_j = j->first;
	 LineList outputlines = j->second;

	 // Update the outputlines first.
	 list<LineType> linelist_k = linelist;
	 list<LineType> outputlines_tmp = outputlines.mlines;
	 list<LineType> outputlines_k;
	 list<LineType>::iterator k=outputlines_tmp.begin();
	 outputlines_k.clear();// Not really needed.
	 for (; k != outputlines_tmp.end(); k++) {
	    list<LineType>::iterator l=linelist_k.begin();
	    for (; l != linelist_k.end(); l++) {
	       if (k->name == l->name)  
		  outputlines_k.push_front( (*l) );
	    }
	 }
	 outputlines.mlines = outputlines_k; // Replace with the newly populuated list.
	    
	 // Now update the map.
	 if (curr_line_j.name == curr_line_i.name) {
	    line_to_outputline_map.erase(curr_line_j);
	    line_to_outputline_map.insert(make_pair(curr_line_i, outputlines));
	 }
      }
      //----------------------------------------------------------------
   }
}
//-------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------
