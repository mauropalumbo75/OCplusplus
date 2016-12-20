/* 
 * File:   main.cpp
 * Author: mauro
 *
 * Created on 10 May 2015, 19:10
 */

#include <cstdlib>
#include "../include/electronic.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
//    EDOS pippo {"EDOS Aldos eV 1 3 0 7.8547 Al.dos.36"}; 
//    EDOS pippo {"EDOS Nidos eV 1 10 1 14.2678 Ni.dos.44"}; 
 //   EDOS pippo {"EDOS Crdos eV 2 6 1 18.3275 Cr.dos.40"}; 
      EDOS pippo {"EDOS Redos eV 2 7 0 24.7219 RE.dos.36"}; 
    
//    pippo.print_TDB(cout,true);
//    cout<<kB_to_eV<<endl;
//    cout<<kB<<endl;
//    cout<<f_fermi(7.88,1000,7.8547)<<endl;    
    
//    cout<<pippo.integral(0.0)<<endl;
    
//    cout<<pippo.electrons()<<endl;
       
//    DP T=1000.0;
//    cout<<T<<"\t"<<pippo.E_Sommerfeld(T)<<"\t"<<pippo.S_Sommerfeld(T)<<"\t"
//            <<pippo.C_Sommerfeld(T)<<"\t"<<pippo.F_Sommerfeld(T)<<endl;   
    
//    cout<<T<<"\t"<<pippo.E(T)-pippo.E(0.0)<<"\t"<<pippo.S(T)<<"\t"
//            <<pippo.C(T)<<"\t"<<pippo.F(T)<<endl;   

//    pippo.calc(cout,0,10,1,1);
//    pippo.print_TDB(cout);
    
//    cout<<pippo<<endl;
    
    List_EDOS test;
    test.insert(pippo);
    test.insert("EDOS Aldos eV 1 3 0 7.8547 Al.dos.36");
    cout<<test.get("REDOS");
    
    // Open output file
    ofstream ost("ReSommerfeld");        //  ist read the file fin
    if (!ost) {
        cerr<<"Input file cannot be opened"<<endl;
        return false;
    } 

    ost.clear();     
    test.get("REDOS").calc(ost,0,2500,1,0);
    
//    test.insert("EDOS Crdos eV 2 6 1 18.3275 Cr.dos.40");
//    cout<<test.list()<<endl;
//    test.calc(cout,"CALCULATE EDOS CRDOS 1 10 1");
//    test.print_TDB();
    
    return 0;
}

