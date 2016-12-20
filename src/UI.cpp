/* 
 * File:   UI.cpp
 * Author: Mauro Palumbo
 *
 * Created on January 20, 2015, 1:50 AM
 * 
 * 
 */

#include "../include/UI.h"
#include <algorithm>


// Is s1 a subcommand of s2?
bool is_subcommand(string s1, string s2)
{
    for(int i=0; i<s1.size(); i++)
        if (s1[i]!=s2[i]) return false;
    return true;
}

string get_command(set<string> commands_list, string s) 
{
    string command;
            
    s=to_up(s);     // To upper case
        
    // Turn input command into a proper one (considering abbreviations and making all in capital letters)
    set<string>::iterator it;
    std::pair<std::set<string>::iterator,bool> ins;
        
    ins=commands_list.insert(s);    // insert input command in set of commands
    if (ins.second) {  // input command was not in set, maybe it is an abbreviation
            if (++ins.first!=commands_list.end()) {
                if (!is_subcommand(s,*ins.first))
                    command=""; 
                else {
                    command=*ins.first;
                    
                    if ((++ins.first!=commands_list.end()) && (is_subcommand(s,*ins.first))) 
                        command="";
                }
                commands_list.erase(s);
            } else command="";       
        } else command=*ins.first;
    
    return command;
}

string Commands::get_next_command_in_list(const set<string>& list, string msg, string def)
{
    string line;
    if (!(ss>>def)) {                 // still something in the stream? 
        os<<msg;   // if not, get more input
        getline(is,line); 
        ss.clear(); ss<<line; ss>>def; 
    } 
    def=get_command(list,def);
    return def;
}

// Get a list of additional options of the same kind for a command
vector<string> Commands::get_list_of_options(string msg, string def)
{
    string buf,line;
    vector<string> list;
    if (!(ss>>buf)) {
        os<<msg<<def;
        getline(is,line); 
        ss.clear(); ss<<line; 
    }
    else list.push_back(to_up(buf));  
    // read eventually more options (if any)
    while (ss>>buf) 
        list.push_back(to_up(buf));    
    return list;    
}

// Get a list of a fixed number of additional options of possibly different kind for a command
void Commands::get_list_of_n_options(vector<string>& list, vector<string> msg, vector<string> def)
{
    string buf,line;

    for (int i=0; i<msg.size(); i++) {
        if (!(ss>>buf)) {
            os<<">"<<msg[i]<<" /"<<def[i]<<"/ : ";
            getline(is,line); 
            ss.clear(); ss<<line; 
            if (!(ss>>buf)||buf=="") list.push_back(to_up(def[i]));   // use default
            else list.push_back(to_up(buf));              // use input
        }
        else list.push_back(to_up(buf));  
    } 
}

// Print the list of possible options in input vector "options"
void Commands::print_options(set<string>& options)
{
    os<<"Possible options: ";
    for (auto it=options.begin(); it!=options.end(); it++) 
        os<<*it<<", ";
    os<<endl;
}

// Print the list of possible commands
void Commands::print_commands()
{
    os<<"Possible commands: ";
    for (auto it=commands.begin(); it!=commands.end(); it++) 
        os<<*it<<", ";
    os<<endl;
}

void Commands::select(const string command)
{
    // Select appropriate actions
    if (command=="ADD")
        command_add();
    else if (command=="CALCULATE")
        command_calculate();
    else if (command=="DEFINE")
        command_define();
    else if (command=="EXIT")
        command_exit(); 
    else if (command=="LIST")
        command_list(); 
    else if (command=="READ")
        command_read();
    else if (command=="RESET")
        command_reset(); 
    else if (command=="SET")
        command_set(); 
    else if (command=="WRITE")
        command_write();
    else {
        cout<<"Wrong or ambiguous command entered!"<<endl;
        print_commands();
    }
}    

void Commands::command_loop()
{
    string command,line,s;

    ss.str(""); ss.clear();   // reset the string stream ss
    
    os<<this->get_prompt();                     // print the prompt
    while (getline(is,line)) {      // read input line (command + optional additional input)
        
        if (line!="") {              // if it is not an empty line...
            ss<<line;      // put the line into a stringstream             
            ss>>s;         // extract first string from ss
            command=get_command(commands,s);    // get command corresponding to s (if any). If not, command=""
            this->select(command);        // select the appropriate actions, pass the rest of ss stream for
                                                // further 
        }
        
        os<<this->get_prompt();    // print the prompt again for next command 
        ss.str(""); ss.clear();
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  
// Here are the functions for each command stating what to do
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool Commands::subcommand_add_vibrations()
{
    // read options for command ADD VIBRATIONS
    
    // Define msg and default answers for list_of_n_options
    vector<string> msg {"Phase /SIGMA/: ","Temperature /298.15/: "};
    vector<string> def {"SIGMA","298.15"};
    
    vector<string> list;
    
    get_list_of_n_options(list,msg,def);
    if ((list.size()==0)||(list[0]=="")) {
        os<<"Not enough input parameters!"<<endl;
        return false;
    }
    
        
    // Full command
    os<<">Full command: ADD VIBRATIONS ";
    for (auto x : list)  
        os<<x<<" ";
    os<<endl;       
    
    // Execute the command
    DP T {298.15}, maxF;
    from_string(T,list[1]);
//    sys.add_vib(list[0],T,maxF);  
//    sys.insert(TPfunction("FUNCTION UNSTABLE 1.0 "+to_string(maxF)+"; 6E3 N !"));  
    
    return true;
}


bool Commands::command_add()
{
    os<<"ADD command entered!"<<endl;
  
    // read "WHAT" to define
    string what;
    set<string> options {"VIBRATIONS"};
    what=get_next_command_in_list(options,
            ">Define what? /VIBRATIONS/: ","VIBRATIONS");
    if (what=="") {
        os<<"No such option!"<<endl;
        print_options(options);
        return false;
    }
    
    // Call appropriate function depending what you want to define
    if (what=="VIBRATIONS") 
        if (!subcommand_add_vibrations()) return false; 
    
    return true;
}

bool Commands::subcommand_calculate_function()
{
    // read options for command DEFINE FUNCTIONS
    
    // Define msg and default answers for list_of_n_options
    vector<string> msg {"Function name", "T", 
                "P","Derivative","Order"};
    vector<string> def {"", "298.15", "1E5","","1"};
    
    vector<string> list;
    
    get_list_of_n_options(list,msg,def);
    if ((list.size()==0)||(list[0]=="")) {
        os<<"Not enough input parameters!"<<endl;
        return false;
    }
    
        
    // Full command
    string all_options="CALCULATE ";
    os<<">Full command: CALCULATE FUNCTION  ";
    for (auto x : list) { 
        os<<x<<" ";
        all_options+=(x+" ");
    }
    os<<endl;       
    
    // Execute the command
    cout<<"Result= "<<sys.calculate_TPfunction(all_options)<<endl;  
    
    return true;
}


bool Commands::subcommand_calculate_phase()
{
    // read options for command DEFINE FUNCTIONS
    
    // Define msg and default answers for list_of_n_options
    vector<string> msg {"Phase name", "T", 
                "P","Amount of phase (mol)"};
    vector<string> def {"", "298.15", "1E5","1"};
    vector<string> list;
    
    get_list_of_n_options(list,msg,def);
    if ((list.size()==0)||(list[0]=="")) {
        os<<"Not enough input parameters!"<<endl;
        return false;
    }
    
    // Now get the site fractions in each sublattice for each constituent (less 1)
    msg.clear();    // clear mgs and def vectors to reuse them
    def.clear();
    Phase temp_ph {sys.get(list[0])};
    os<<"N. sub.= "<<temp_ph.nsub()<<endl;
    for (int i=0; i<temp_ph.nsub(); i++) {
        List_elements temp_el {temp_ph.get_elements_in_sub(i)};
        os<<"Elements in sub. "<<i<<": "<<temp_el.list()<<endl;
        for (int j=0; j<(temp_el.size()-1); j++) {                         // loop over all elements but one (no need to ask all, the last will be 1-y1-y2-y3...)
            msg.push_back("Site fraction sublattice "+to_string(i)+" constituent "+temp_el[j].symbol());
            def.push_back("1.0E-12");    // default site fraction is 1.0E-12
        }
    }
    get_list_of_n_options(list,msg,def);
    
        
    // Full command
    string all_options="CALCULATE PHASE ";
    os<<">Full command: CALCULATE PHASE ";
    for (auto x : list) { 
        os<<x<<" ";
        all_options+=(x+" ");
    }
    os<<endl;       
    
    // Execute the command
    cout<<"Result= "<<sys.calculate_phase(all_options)<<endl;  
    
    return true;
}

bool Commands::subcommand_calculate_edos()
{
    // read options for command CALCULATE PDOS
    
    // Define msg and default answers for list_of_n_options
    vector<string> msg {"EDOS name", "T low", "T high", "T step",
                 "File name (or SCREEN)"};
    vector<string> def {"", "1", "2000","1", "SCREEN"};
    
    vector<string> list;
    
    get_list_of_n_options(list,msg,def);
    if ((list.size()==0)||(list[0]=="")) {
        os<<"Not enough input parameters!"<<endl;
        return false;
    }
    
        
    // Full command
    string all_options="CALCULATE EDOS ";
    os<<">Full command: CALCULATE EDOS ";
    for (auto it=list.begin(); it<list.end()-1; it++) { 
        os<<*it<<" ";
        all_options+=(*it+" ");
    }
    os<<endl;       
    
    // Execute the command
    if (to_up(list[list.size()-1])!="SCREEN") {        // Open output file
        ofstream ost(list[list.size()-1]);             //  ost write the file fname
        if (!ost) {
            cerr<<"Input file cannot be opened"<<endl;
            return false;
        }
        ost.clear(); 
        
        sys.calculate_edos(ost,all_options);
    }  else sys.calculate_edos(os,all_options);
     
    return true;    
}

bool Commands::subcommand_calculate_pdos()
{
    // read options for command CALCULATE PDOS
    
    // Define msg and default answers for list_of_n_options
    vector<string> msg {"PDOS name", "T low", "T high", "T step",
                 "File name (or SCREEN)"};
    vector<string> def {"", "1", "2000","1", "SCREEN"};
    
    vector<string> list;
    
    get_list_of_n_options(list,msg,def);
    if ((list.size()==0)||(list[0]=="")) {
        os<<"Not enough input parameters!"<<endl;
        return false;
    }
    
        
    // Full command
    string all_options="CALCULATE PDOS ";
    os<<">Full command: CALCULATE PDOS ";
    for (auto it=list.begin(); it<list.end()-1; it++) { 
        os<<*it<<" ";
        all_options+=(*it+" ");
    }
    os<<endl;       
    
    // Execute the command
    if (to_up(list[list.size()-1])!="SCREEN") {        // Open output file
        ofstream ost(list[list.size()-1]);             //  ost write the file fname
        if (!ost) {
            cerr<<"Input file cannot be opened"<<endl;
            return false;
        }
        ost.clear(); 
        
        sys.calculate_pdos(ost,all_options);
    }  else sys.calculate_pdos(os,all_options);
     
    return true;    
}

bool Commands::command_calculate()
{
    os<<"CALCULATE command entered!"<<endl;
  
    // read "WHAT" to define
    string what;
    set<string> options {"EDOS", "FUNCTION","PHASE","PDOS"};
    what=get_next_command_in_list(options,
            ">Calculate what? /FUNCTION/: ","FUNCTION");
    if (what=="") {
        os<<"No such option!"<<endl;
        print_options(options);
        return false;
    }
    
    // Call appropriate function depending what you want to calculate
    if (what=="FUNCTION") 
        if (!subcommand_calculate_function()) return false; 
    if (what=="PHASE") 
        if (!subcommand_calculate_phase()) return false; 
    if (what=="EDOS") 
        if (!subcommand_calculate_edos()) return false; 
    if (what=="PDOS") 
        if (!subcommand_calculate_pdos()) return false; 
        
    return true;
}

bool Commands::subcommand_def_elements()
{
    // read options for command DEFINE, this should be a list of elements
    vector<string> list;
    list=get_list_of_options(">List of elements: ");
    if (list.size()==0) {
        os<<"No input!"<<endl;
        return false;
    }
    
    // Full command
    os<<">Full command: DEFINE ELEMENTS ";
    for (auto x : list) os<<x<<" ";
    os<<endl;       
    
    // Execute the command
    for (auto x : list) sys.insert(Element(x));
    
    return true;
}
      
bool Commands::subcommand_def_species()
{
    // read options for command DEFINE, this should be a list of species
    vector<string> list;
    list=get_list_of_options(">List of species: ");
    if (list.size()==0) {
        os<<"No input!"<<endl;
        return false;
    }
    
    // Full command
    os<<">Full command: DEFINE SPECIES ";
    for (auto x : list) os<<x<<" ";
    os<<endl;       
    
    // Execute the command
    for (auto x : list) sys.insert(Species(x));  
    
    return true;
}

bool Commands::subcommand_def_functions()
{
    // read options for command DEFINE FUNCTIONS
    
    // Define msg and default answers for list_of_n_options
    vector<string> msg {"Function name", "Low temperature limit", 
                "Expression", "High temperature limit", "Any more ranges?"};
    vector<string> def {"", "0", "0;", "6000", "N" };
    
    vector<string> list;
    
    get_list_of_n_options(list,msg,def);
    if ((list.size()==0)||(list[0]=="")) {
        os<<"Not enough input parameters!"<<endl;
        return false;
    }
    
    while (list[list.size()-1]=="Y") {          // more ranges to input
        size_t oldn=list.size();
               
        // reduce the options to read
        vector<string> msg2 {"Expression", "High temperature limit", "Any more ranges?"};
        vector<string> def2 {"0;", "6000", "N" };
        
        get_list_of_n_options(list,msg2,def2);
        if (list.size()<(oldn+msg2.size())) {            // if less than additional new options to read
            os<<"Not enough input parameters!"<<endl;
            return false;
        }
    }
        
    // Full command
    string all_options="FUNCTION ";
    os<<">Full command: DEFINE FUNCTIONS  ";
    for (auto x : list) { 
        os<<x<<" ";
        all_options+=(x+" ");
    }
    os<<endl;       
    
    // Execute the command
    sys.insert(TPfunction(all_options));  
    
    return true;
}

bool Commands::subcommand_def_edos()
{
    // read options for command DEFINE PDOS
    
    // Define msg and default answers for list_of_n_options
    vector<string> msg {"EDOS name", "Input energy unit",  "Number of atoms",
                "Number of electrons", "Magnetic",  "Fermi energy", "Input file"};
    vector<string> def {"", "eV", "1", "1", "1", "0","EDOS.OUT"};
    
    vector<string> list;
    
    get_list_of_n_options(list,msg,def);
    if ((list.size()==0)||(list[0]=="")) {
        os<<"Not enough input parameters!"<<endl;
        return false;
    }
        
    // Full command
    string all_options="EDOS ";
    os<<">Full command: DEFINE EDOS  ";
    for (auto x : list) { 
        os<<x<<" ";
        all_options+=(x+" ");
    }
    os<<endl;       
    
    // Execute the command
    sys.insert(EDOS(all_options));  
    
    return true;
}

bool Commands::subcommand_def_pdos()
{
    // read options for command DEFINE PDOS
    
    // Define msg and default answers for list_of_n_options
    vector<string> msg {"PDOS name", "Input energy unit", 
                "Number of atoms", "Input file"};
    vector<string> def {"", "cm-1", "1", "PHDOS.OUT"};
    
    vector<string> list;
    
    get_list_of_n_options(list,msg,def);
    if ((list.size()==0)||(list[0]=="")) {
        os<<"Not enough input parameters!"<<endl;
        return false;
    }
        
    // Full command
    string all_options="PDOS ";
    os<<">Full command: DEFINE PDOS  ";
    for (auto x : list) { 
        os<<x<<" ";
        all_options+=(x+" ");
    }
    os<<endl;       
    
    // Execute the command
    sys.insert(PDOS(all_options));  
    
    return true;
}

bool Commands::command_define()
{
    os<<"DEFINE command entered!"<<endl;
  
    // read "WHAT" to define
    string what;
    set<string> options {"ELEMENTS", "SPECIES", "FUNCTION", "EDOS", "PDOS"};
    what=get_next_command_in_list(options,
            ">Define what? /ELEMENTS/: ","ELEMENTS");
    if (what=="") {
        os<<"No such option!"<<endl;
        print_options(options);
        return false;
    }
    
    // Call appropriate function depending what you want to define
    if (what=="ELEMENTS") 
        if (!subcommand_def_elements()) return false; 
    if (what=="SPECIES") 
        if (!subcommand_def_species()) return false; 
    if (what=="FUNCTION") 
        if (!subcommand_def_functions()) return false; 
    if (what=="EDOS") 
        if (!subcommand_def_edos()) return false; 
    if (what=="PDOS") 
        if (!subcommand_def_pdos()) return false; 
    
    return true;
}

void Commands::command_exit()
{
    os<<"I hope you enjoyed OC++! See you soon again."<<endl;
    exit(0);
}

bool Commands::command_list()
{    
    os<<"LIST command entered!"<<endl;
    
    // read "WHAT" to list 
    string what;
    set<string> options {"ABINITIO", "ALL", "EDOS", "ELEMENTS", "SPECIES", "FUNCTIONS", "PHASES", "PDOS"};
    what=get_next_command_in_list(options,
            ">List what? /ALL/: ","ALL");
    if (what=="") {
        os<<"No such option!"<<endl;
        print_options(options);
        return false;
    }
    
    // read "WHERE" to list (screen or a file)
    string fname,line;
    if (!(ss>>fname)) {
        os<<">File name? (or SCREEN) : ";
        getline(cin,line); 
        ss.clear(); ss<<line; ss>>fname; 
        if (fname=="") fname="SCREEN";
    }  
    
    // Full command
    os<<"Full command: LIST "<<what<<" "<<fname<<endl;  
        
    // DO SOMETHING! call appropriate function
    if (to_up(fname)!="SCREEN") {  // Open output file
        ofstream ost(fname);            //  ost write the file fname
        if (!ost) {
            cerr<<"Input file cannot be opened"<<endl;
            return false;
        }
        ost.clear(); 

        if (what=="ALL") 
            sys.print_TDB(ost);
        if (what=="ELEMENTS") 
            sys.print_TDB(ost,true,false,false,false,false,false);
        if (what=="SPECIES") 
            sys.print_TDB(ost,false,true,false,false,false,false);
        if (what=="FUNCTIONS") 
            sys.print_TDB(ost,false,false,true,false,false,false);
        if (what=="PHASES") 
            sys.print_TDB(ost,false,false,false,true,false,false);
        if (what=="EDOS") 
            sys.print_TDB(ost,false,false,false,false,true,false);
        if (what=="PDOS") 
            sys.print_TDB(ost,false,false,false,false,false,true);
        if (what=="ABINITIO")
            sys.print_raw_abinitio(ost);
    }   else {
        if (what=="ALL") 
            sys.print_TDB(os);
        if (what=="ELEMENTS") 
            sys.print_TDB(os,true,false,false,false,false,false);
        if (what=="SPECIES") 
            sys.print_TDB(os,false,true,false,false,false,false);
        if (what=="FUNCTIONS") 
            sys.print_TDB(os,false,false,true,false,false,false);
        if (what=="PHASES") 
            sys.print_TDB(os,false,false,false,true,false,false); 
        if (what=="EDOS") 
            sys.print_TDB(os,false,false,false,false,true,false);
        if (what=="PDOS") 
            sys.print_TDB(os,false,false,false,false,false,true);
        if (what=="ABINITIO")
            sys.print_raw_abinitio(os);
    }     
  
    return true;
}


bool Commands::command_read()
{
    os<<"READ command entered!"<<endl;
  
    // read "WHAT" to define
    string what;
    set<string> options {"TDB", "ABINITIO"};
    what=get_next_command_in_list(options,
            ">Read what? /TDB/: ","TDB");
    if (what=="") {
        os<<"No such option!"<<endl;
        print_options(options);
        return false;
    }
    
    string fname,line;
    if (!(ss>>fname)) {
        os<<">File name?: ";
        getline(cin,line); 
        ss.clear(); ss<<line; ss>>fname;     
    }  
    
    //  Execute the appropriate command
    if (what=="TDB") 
        sys.read_tdb(fname);
    if (what=="ABINITIO") 
        sys.read_raw_abinitio(fname);
    
    // Full command
    cout<<"Full command: READ "<<what<<" "<<fname<<endl;
    
    return true;
}

bool Commands::command_reset()
{    
    cout<<"RESET command entered!"<<endl;
    sys.reset();   
    return true;
}

bool Commands::command_set()
{    
    cout<<"SET command entered!"<<endl;   
    return true;
}

bool Commands::command_write()
{
    cout<<"WRITE command entered!"<<endl;    
    return true;
}




