/* 
 * File:   main.cpp
 * Author: mpalumbo
 *
 * Created on June 15, 2014, 8:51 PM
 */

#include <cstdlib>
#include <vector>
#include "../include/readabinitio.h"
#include "../include/structure.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    // Load structure database (necessary here to check the Wyckoff positions order)
    string filename="structures_database";
    List_structures structures;
    
    structures.read(filename);
    structures.print();
    
    cout<<"\n";
    
       
    vector<ABLine> vec;
    vector<string> file_lines;
    List_elements le;
    
    string finput="Co.data";
 //   read_abinitio_file(vec,structures,finput);
    finput="Al.data";
 //   read_abinitio_file(vec,structures,finput);
//    finput="Co-W-Ti.data";
//    read_abinitio_file(le,vec,structures,finput);
//    finput="test.data";
    read_abinitio_file(le,vec,structures,finput,file_lines);
    for (auto x:file_lines) cout<<x<<endl;
        
    return 0;
}

