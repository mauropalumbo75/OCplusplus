/* 
 * File:   main.cpp
 * Author: mauro
 *
 * Created on 27 April 2015, 13:33
 */

#include <cstdlib>
#include <vector>
#include <iostream>
#include "ev.h"
#include "../commoninclude/physconsts.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

//    string f="Ni.EvsV";
//    string f="Ni.FvsV.1";
//    EV pippo(f+".in",f,"fcc");    
/*
    pippo.print_data();
    cout<<pippo.size()<<"\t"<<pippo.npar()<<endl;
    pippo.fit();
    cout<<"Chi= "<<pippo.chisq()<<endl;
//    pippo.print_results();
 //   pippo.set_fit_limits(4,4);
 //   pippo.print_data();
 //   pippo.fit();
       
    pippo.write_to_file(f+".out","fcc");
    pippo.write_to_file(f+".out.SI","V", "m","J","mol");
    cout<<pippo.V_P(0)<<"\t"<<pippo.V_P(1e5)<<endl;
    cout<<pippo.P_V(1.0322718124e-05)<<"\t"<<pippo.P_V(pippo.V_P(1e5))<<endl;
    cout<<pippo.E_V(pippo.V_P(0))<<"\t"<<pippo.E_V(pippo.V_P(1e5))<<endl;
    cout<<pippo.H_V(pippo.V_P(0))<<"\t"<<pippo.H_V(pippo.V_P(1e5))<<endl;
 */   
//    f="Al.FvsV.2";
//    EV b(f+".in",f,"fcc"); 
//    f="Ni.FvsV.298";
//    EV c(f+".in",f,"fcc"); 
    
//    b.fit();
//    b.print_results(cout,"fcc");
    
//    vector<string> v {"Al.EvsV.in", "Al.FvsV.1.in","Al.FvsV.2.in","Al.FvsV.3.in"};
//    vector<string> v2 {"Al.EvsV.out", "Al.FvsV.1.out","Al.FvsV.2.out"};
    List_EV list;
//    list[b.name()] = b;
    list.read_from_file("Ni.EvsV.in","fcc");
//    list.read_from_file(v,"fcc");
  
  
    cout<<"N list: "<<list.size()<<"\tList EV: "<<list.list()<<endl;
    list.set_fit_limits(4,4);
//    list.print_data();
    list.fit();
    
    EV test=list.get("Ni.EvsV.in");

    test.fit();
    cout<<test.chisq()<<endl;
//    cout<<list.E_TP("1",0.0);
    list.print_results(cout);
    list.write_to_file("");
    list.write_to_file("SI","V","m","J","mol");
 
    return 0;
}

