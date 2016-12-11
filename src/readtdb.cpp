/* 
 * File:   readtdb.cpp
 * Author: Mauro Palumbo
 *
 * Created on September 21, 2014, 8:02 PM
 * 
 * Still to do: handling PARAMETER, magnetism, special features
 */

#include "readtdb.h"

vector<string> tdb_commands {"CONSTITUENT", "ELEMENT", "FUNCTION", "LIST_OF_REFERENCES",
        "PARAMETER", "PHASE", "SPECIES", "TYPE_DEFINITION"};
        
bool get_tdb_line(ifstream& ist, string& line)
{
    string temp;
    while(getline(ist,temp)) {
        char ch;
        stringstream ss(temp);
        
        if (ss>>ch && ch!='$') {                  //   ignore comment lines starting with $ and empty lines
            ss.unget();      // put back the char in the stream

            
            // Check and read list of references (usually at the end of the TDB file, but not necessary)
            // Command is  LIST_OF_REFERENCES and must end with !            
            // The list of references needs to be treated differently from other lines
            string command;
            ss>>command; 
            command=to_up(command);
            if (command=="LIST_OF_REFERENCES") {
                line="LIST_OF_REFERENCES ";
                while (ist.get(ch)&&ch!='!') line+=ch;
                line=to_up(line);
                return true;
            }
           

            temp=to_up(temp);   // set all characters to upright
            temp=trim(temp);    // trim whitespaces,\t,\n,\r from the beginning and end of the line
            
            ch=temp[temp.size()-1];     // check last character in the line
            if (ch=='Y' || ch=='N' || ch==';') temp+=" ";   // add a space at the end if last char is Y, N, ; 
            line+=temp;
            
            // check for the !
            size_t pos_exc=string::npos;
            pos_exc=line.find("!");
            if (pos_exc != string::npos) {
               line.erase(pos_exc,string::npos);   // erase all characters after ! and return
               return true;
            }
        }
    }    
    if (!ist.good()) return false;
    return true;
}


bool read_tdb_file(List_elements& elements, List_species& species, 
        List_TPfunctions& TPfunctions, List_phases& phases, string& refs, List_types& types, string fin)
{
    // temporary storage variables
    string line, command;
    map<string,string> phase_definitions;       // for all PHASE lines
    map<string,string> constituent_definitions; // for all CONSTITUENT lines
    map<string,string> parameter_definitions;   // for all PARAMETERS lines 
    
    // Open input file
    ifstream ist(fin.c_str());        //  ist read the file fin
    if (!ist) {
        cerr<<"Input file cannot be opened"<<endl;
        return false;
    } else cout<<"Reading from file "<<fin<<endl;

    ist.clear();

    while(get_tdb_line(ist,line))            // Read line by line. A tdb line ends with !
    {
        stringstream ss(line);

        // Process the line according to first string (tdb command)
        string ph;   // store the second string, could be the phase name needed in PHASE, CONSTITUENT and PARAMETERS
        ss>>command>>ph;
        command=get_subcommand_extended(command,tdb_commands);
                
        cout<<"Line: "<<line<<endl;
        if (command=="ELEMENT") elements.insert(Element(line));
        if (command=="SPECIES") species.insert(Species(line));
        if (command=="FUNCTION") TPfunctions.insert(TPfunction(line)); 
        if (command=="TYPE_DEFINITION") types.insert(Type(line)); 
               
        // For phases, constituents and parameters, here we store the defining strings
        // Later we will check that they match and use the corresponding constructors
        if (command=="PHASE") phase_definitions[ph]=line;
        if (command=="CONSTITUENT") constituent_definitions[ph]=line;
        if (command=="PARAMETER") parameter_definitions[ph]=line;

        if (command=="LIST_OF_REFERENCES") refs=line;                      
 
        line.clear();            
    }
    
    types.print_TDB(cout);
    // After scanning the whole file, we check PHASE and CONSTITUENT definitions for matching
    // and use the constructor to create a phase
    cout<<phase_definitions.size()<<"\t"<<constituent_definitions.size()<<endl;
    for (auto p : phase_definitions) {
        bool match=false;
        cout<<p.first<<"\t"<<p.second<<endl;
        auto q=constituent_definitions.begin();
        for (; q != constituent_definitions.end(); q++) 
            if ((p.first)==(q->first)) { match=true; break; }
        cout<<q->first<<"\t"<<q->second<<endl;
        if (match) phases.insert(Phase(p.second, q->second,types));
        else cerr<<"No proper CONSTITUENT declaration for phase "<<p.first<<endl;
    }
    
    // Now we loop over the PARAMETERS stored in parameter_definitions. If the corresponding phase
    // has been defined, we save the parameter in the phase class, if not error!
    for (auto p : parameter_definitions) {
        cout<<p.first<<"\t"<<p.second<<endl;
        cout<<"Defining: "<<p.second<<endl;
        phases.set_parameter(p.second);     // check if the phase in PARAMETER exists in phases. if so, call set_parameter  
    }
        
    return true;
}