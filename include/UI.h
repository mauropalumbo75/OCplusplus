/* 
 * File:   UI.h
 * Author: Mauro Palumbo
 *
 * This is only a very preliminary command line User Interface. Unfortunately, it 
 * takes time to develop a proper one. 
 *
 *  Almost totally missing is error checking...
 * 
 * It must still be tested before being used extensively. Use it with precaution
 * and at your own risk!
 * 
 * Created on January 20, 2015, 1:50 AM
 */



#ifndef UI_H
#define	UI_H

#include <cstdlib>
#include <set>
#include <vector>
#include <stack>
#include <string>
#include <iostream>
#include <sstream>
#include "utils.h"
#include "systems.h"

using namespace std;

class Commands {
public:
    Commands(Systems& ssys, istream& iis=cin, ostream& oos=cout, string p="OC++>") 
      : sys(ssys), is(iis), os(oos), prompt(p)
    {}
    
    string get_prompt() { return prompt; }
    void command_loop();
    void select(const string command);
    
protected:
    // get next command from the stringstream "ss", check it is in "list" and return it
    string get_next_command_in_list(const set<string>& list, string msg="", string def="");
    
    // get next list of options from the stringstream "ss" and return them
    vector<string> get_list_of_options(string msg="", string def="");
    void get_list_of_n_options(vector<string>& list, vector<string> msg, vector<string> def);
    void print_options(set<string>& options);
    void print_commands();
    
    // List of functions, each taking appropriate actions for each command
    bool command_add();
      bool subcommand_add_vibrations();
    bool command_calculate();
      bool subcommand_calculate_function();
      bool subcommand_calculate_phase();
      bool subcommand_calculate_edos();
      bool subcommand_calculate_pdos();
    bool command_define();
      bool subcommand_def_elements();
      bool subcommand_def_species();
      bool subcommand_def_functions();  
      bool subcommand_def_edos();
      bool subcommand_def_pdos();
    void command_exit();
    bool command_list();
    bool command_read();
    bool command_reset();
    bool command_set();
    bool command_write();
    
private:
    Systems& sys;           // the current system (elements, species, phases,...) data
    istream& is;            // input stream for commands (default=cin)
    ostream& os;            // output stream for commands (default=cout)
    const string prompt;    // the prompt used by the UI

    stringstream ss {};     // string stream for input processing    
    stack<string> history;  // yet to be implemented
    set<string> commands    // list of available commands
         {"ADD","CALCULATE","DEFINE", "EXIT", "LIST", "READ", "RESET", "SET", "WRITE"};        
};

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions
////////////////////////////////////////////////////////////////////////////////

// Is s1 a subcommand of s2?
bool is_subcommand(string s1, string s2);

// get the full command s in commands_list (considering possible abbreviations)
string get_command(set<string> commands_list, string s); 


#endif	/* UI_H */

