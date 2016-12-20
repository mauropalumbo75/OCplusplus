/* 
 * File:   main.cpp
 * Author: mauro
 *
 * Created on 05 May 2015, 13:29
 */

#include <cstdlib>
#include "../include/qha.h"
#include "../include/fv.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
 
/*   
    FV pippo("FvsV.in","fef");
    
    pippo.print_data(cout);
    pippo.fit();
    pippo.print_results();
 */ 
   
    vector<string> outfiles {"T","V","F","Fharm","G","H","S","Sharm","C","Charm","B0","B0p","beta","chi"};
        
    QHA list;
    
    vector<string> temperatures;
    for (int i=1; i<2000; i++)
        temperatures.push_back(to_string(i));  
    
    list.qha_from_dos(temperatures,"preqha/","V","au","cm-1",2);
 //   list.set_fit_limits(15,15);

    list.fit();
    list.calculate(0);   // at which pressure? 0 or 1e5?
    list.write_to_file("");
    list.write_qha_results(outfiles);

    
/*
//    list.read_from_file("Al.EvsV.in","fcc");
//    list.read_from_file(v,"fcc");
  
    vector<string> temperatures;
    for (int i=1; i<2000; i++)
        temperatures.push_back(to_string(i));        
//    for (auto x:temperatures) cout<<x<<endl;
    
    list.read_qha_files(temperatures,"Al/","fcc");
    list.set_fit_limits(4,4);
    
    cout<<"N list: "<<list.size()<<"\tList EV: "<<list.list()<<endl;
    cout<<"List T: "<<list.listT()<<endl;
//    list.print_data();
    list.fit();
    list.calculate(1e5);
    
//    cout<<list.E_TP("1",0.0);
//    list.print_results(cout);
//    list.write_qha_set(cout,1e5);
    list.write_to_file("","V","Ang","Rydberg","at");
    list.write_qha_results(outfiles);
*/
    
    return 0;
}

