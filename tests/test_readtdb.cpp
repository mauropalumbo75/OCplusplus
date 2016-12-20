/* 
 * File:   main.cpp
 * Author: mpalumbo
 *
 * Created on September 21, 2014, 8:00 PM
 */

#include <cstdlib>
#include <vector>
#include "../include/readtdb.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    List_elements elements;
    List_species species;    
    List_TPfunctions TPfunctions;
    List_phases phases;
    string refs;
    List_types types;
       
//    string finput="crfemo.tdb";
    string finput="test.tdb";

    read_tdb_file(elements, species, TPfunctions, phases, refs, types, finput);
       
    
    // Open output file
    string fout="out.TDB";
    
    ofstream ost(fout.c_str());        //  ist read the file fin
    if (!ost) {
        cerr<<"Output file cannot be opened"<<endl;
        return false;
    } else cout<<"Writing to file "<<fout<<endl;

    ost.clear();
    
    elements.print_TDB(ost);
    ost<<endl;
    species.print_TDB(ost);
    TPfunctions.print_TDB(ost);
    ost<<"\n TYPE_DEFINITION % SEQ *! \n DEFINE_SYSTEM_DEFAULT ELEMENT 2 !\n DEFAULT_COMMAND DEF_SYS_ELEMENT VA /- !\n\n";
    phases.print_TDB(ost);
    
    cout<<refs<<endl;
    if (refs!="") {
        List_references list {refs};
        list.print_TDB(ost);
    }
    
    return 0;
}

