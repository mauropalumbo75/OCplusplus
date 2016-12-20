/* 
 * File:   main.cpp
 * Author: mauro
 *
 * Created on January 24, 2015, 8:52 PM
 */

#include <cstdlib>
#include "../include/systems.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    Systems system;
    
 //   system.insert(Element("Cr"));
 //   system.insert(Element("FE"));
//    system.insert(Species("H1O2"));
//    system.insert(TPfunction("FUNCTION  GHSERAL	0	3*T**2;	3.000000e+03   Y   1;  4000 Y  50*T^2; 6000 N "));
//    system.insert(Phase("PHASE SIGMA  %  5 2 4 8 8 8 !","   CONSTITUENT  SIGMA :CR,FE:CR,FE:CR,FE:CR,FE:CR,FE: !"));   
    
    system.print_TDB(cout,false,false,false);
    
//    cout<<"CALCULATE GHSERAL 1 1e5 = "<<system.calculate_TPfunction("CALCULATE GHSERAL 1 1e5")<<endl;
    
//    cout<<system.calculate_phase("CALCULATE PHASE  SIGMA 298.15 1E5 1 1 1 1 1 1")<<endl;
    
 //   cout<<"N. sub.= "<<system.get("SIGMA").nsub()<<endl;
 //   system.print_structures();
   
 //   system.reset();
 //   system.read_tdb("test.TDB");
    
 //   system.print_TDB();
    
   
//    system.read_raw_abinitio("Al.data");
//    system.print_raw_abinitio();
    
    system.insert(PDOS("PDOS Aldos cm-1 1 PHDOS.OUT"));
//    system.print_TDB(cout,false,false,false,false,false,false,true);
    system.print_TDB();
    system.calculate_pdos(cout,"CALCULATE PDOS ALDOS 1 2000 1");

    return 0;
}

