/* 
 * File:   main.cpp
 * Author: mauro
 *
 * Created on 03 May 2015, 18:13
 */

#include <cstdlib>
#include "../include/harmonic.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    string fname="PHDOS.OUT";

    
//    PDOS pippo {"PDOS Aldos cm-1 1 >       1e-10      0.000000   0.7500    0.6800000E-06       1.5000    0.2740000E-05"}; 
//    PDOS pippo {"PDOS Aldos eV 1 PHDOS.6.5037"};
    PDOS pippo {"PDOS Aldos cm-1 1 PHDOS.6.5037"}; 
//     PDOS pippo {"PDOS Aldos eV 1 PHDOS.OUT"};
//    PDOS pippo {"Aldos"}; 
//    pippo.read_dos_file(fname);
    
    
    // Open output file
    ofstream ost("out");        //  ist read the file fin
    if (!ost) {
        cerr<<"Input file cannot be opened"<<endl;
        return false;
    } 

    ost.clear();  
    
    pippo.print_TDB(ost,true);
    
    
 //   cout<<inversem_to_J*NA<<endl;
 //   cout<<inversem_to_eV<<endl;
 //   cout<<kB_to_eV<<endl;
    
// Calculate the raw DOS integral
    cout<<pippo.integral(0.0)<<endl;
    pippo.calc(cout,0,2000,1);
    
//    List_PDOS test;
//   test.insert(pippo);
//    test.insert("PDOS Aldos cm-1 1 PHDOS.OUT");
//    test.get("ALDOS").print_TDB();
//    test.get("ALDOS").calc(cout,1,10,1);
//    test.insert("PDOS test cm-1 1 PHDOS.OUT");
//    cout<<test.list()<<endl;
//    test.calc(cout,"CALCULATE PDOS ALDOS 1 2000 1");
//    test.print_TDB();

    return 0;
}

