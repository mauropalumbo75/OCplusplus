/* 
 * File:   main.cpp
 * Author: mpalumbo
 *
 * Created on March 15, 2015, 2:55 PM
 */

#include <cstdlib>
#include <vector>
#include "../include/phases.h"
#include "../include/readabinitio.h"

using namespace std;

/*
 * 
 */

int main(int argc, char** argv) {

    string filename="structures_database";
    List_structures structures;
    
    structures.read(filename);
    structures.print();
    
    cout<<"\n";
    
    vector<ABLine> vec;
    string finput="Al.data";
//    read_abinitio_file(vec,structures,finput);
    finput="Ni.data";
//    read_abinitio_file(vec,structures,finput);
    finput="Al-Ni.data";
//    read_abinitio_file(vec,structures,finput);
    
    cout<<"\nConfigurations loaded: "<<endl;    
    for (int i=0; i<vec.size(); i++)
        cout<<vec[i];
    
    List_elements elements;
    elements.insert("/-");     elements.insert("VA");
    elements.insert("AL");
    elements.insert("NI");
    
    elements.print_TDB(cout);
/*    
    List_phases phases;
    for (int i=0; i<vec.size(); i++)
    {
       if (!phases.is(vec[i].ph)) phases.insert(Phase(vec[i].ph, structures, elements));
//       phases.set_G_ref(vec[i].ph,vec[i].conf,vec[i].entJ);
//       phases.set_E_tot(vec[i].ph,vec[i].conf,vec[i].Etot);
    }
 
    phases.set_G_ref("A15","Al,Ni:Al",10.0);
    cout<<phases.get("A15");
    cout<<endl;
    
    cout<<phases.get("A15").get_E_tot("Al:Al")/8<<endl;
    cout<<phases.get("A15").get_E_tot("Al:Ni")/8<<endl;
    cout<<phases.get("A15").get_E_tot("Ni:Al")/8<<endl;
    cout<<phases.get("A15").get_E_tot("Ni:Ni")/8<<endl;
*/    
 //   phases.print_TDB(cout);
/*     
    phases.set_Y("A15","AL",0,1e-12);
    phases.set_Y("A15","AL",1,1);
    phases.set_Y("A15","NI",0,1);
    phases.set_Y("A15","NI",1,1e-12);
    cout<<phases.get_Y_full("A15","AL",0);
    cout<<phases.get_Y_full("A15","AL",1);
    cout<<phases.get_Y_full("A15","NI",0);
    cout<<phases.get_Y_full("A15","NI",1);
    
    cout<<"\nG_ref(A15)="<<phases.get("A15").calculate_G_ref();
    cout<<"\nS_conf(A15)="<<phases.get("A15").calculate_S_conf();
    DP T=3000;
    cout<<"\nG(A15,T="<<T<<")="<<phases.get("A15").calculate_G(T);
    
    cout<<endl;
    cout<<"\ndG_ref(A15,AL,0)="<<phases.get("A15").calculate_dG_ref("AL",0);
    cout<<"\ndG_ref(A15,AL,1)="<<phases.get("A15").calculate_dG_ref("AL",1);
    cout<<"\ndG_ref(A15,NI,0)="<<phases.get("A15").calculate_dG_ref("NI",0);
    cout<<"\ndG_ref(A15,NI,1)="<<phases.get("A15").calculate_dG_ref("NI",1);
    
    cout<<endl;
    cout<<"\ndS_conf(A15,AL,0)="<<phases.get("A15").calculate_dS_conf("AL",0);
    cout<<"\ndS_conf(A15,AL,1)="<<phases.get("A15").calculate_dS_conf("AL",1);
    cout<<"\ndS_conf(A15,NI,0)="<<phases.get("A15").calculate_dS_conf("NI",0);
    cout<<"\ndS_conf(A15,NI,1)="<<phases.get("A15").calculate_dS_conf("NI",1);
    
//    cout<<"\nS_conf(A15)="<<phases.get("A15").calculate_S_conf();
//    DP T=3000;
//    cout<<"\nG(A15,T="<<T<<")="<<phases.get("A15").calculate_G(T);

/*
    cout<<phases.get("sigma");
    
    cout<<endl;
     
    phases.set_Y("sigma","AL",0,2.81433E-02);
    phases.set_Y("sigma","AL",1,3.63970E-01);
    phases.set_Y("sigma","AL",2,1.14013E-02);
    phases.set_Y("sigma","AL",3,5.39229E-01);
    phases.set_Y("sigma","AL",4,1.03484E-02);
    phases.set_Y("sigma","NI",0,9.71857E-01);
    phases.set_Y("sigma","NI",1,6.36030E-01);
    phases.set_Y("sigma","NI",2,9.88599E-01);
    phases.set_Y("sigma","NI",3,4.60771E-01);
    phases.set_Y("sigma","NI",4,9.89652E-01);

    cout<<phases.get_Y_full("sigma","AL",0);
    cout<<phases.get_Y_full("sigma","AL",1);
    cout<<phases.get_Y_full("sigma","AL",2);
    cout<<phases.get_Y_full("sigma","AL",3);
    cout<<phases.get_Y_full("sigma","AL",4);
    cout<<phases.get_Y_full("sigma","NI",0);
    cout<<phases.get_Y_full("sigma","NI",1);
    cout<<phases.get_Y_full("sigma","NI",2);
    cout<<phases.get_Y_full("sigma","NI",3);
    cout<<phases.get_Y_full("sigma","NI",4);
    
    cout<<"\nG_ref(sigma)="<<phases.get("sigma").calculate_G_ref();
    cout<<"\nS_conf(sigma)="<<phases.get("sigma").calculate_S_conf();
           
    DP T=3000;
    cout<<"\nG(sigma,T="<<T<<")="<<phases.get("sigma").calculate_G(T);

    string s1="PHASE C14  pippo  3 4.000000 2.000000 6.000000";
    string s2="   CONSTITUENT  C14 :AL,CR,NI,LA,P,S:AL,NI:VA: ";
    
    Phase phases2 {s1,s2};
//    cout<<phases2.is("C14")<<endl;
    phases2.set_parameter("PARAMETER G(C14,Al,Ni:Al,Ni,S:VA;0) 0 0; 3000 N !");
    phases2.set_parameter("PARAMETER G(C14,Al,Ni:Al:VA;0) 100 2*T+50; 4000 N !");
    cout<<phases2;  
*/        
    List_types list {};
    
    list.insert("TYPE_DEFINITION & GES A_P_D SIGMA MAGNETIC  -1.0    4.00000E-01 ");
    list.insert("TYPE_DEFINITION A GES A_P_D SIGMA MAGNETIC  -2.0    grfgrgrwe ");
    list.insert("TYPE_DEFINITION % SEQ *");
    list.print_TDB(cout);
    
    string s1="PHASE SIGMA  %&  5 2 4 8 8 8 !";
    string s2="   CONSTITUENT  SIGMA :CR,RE:CR,RE:CR,RE:CR,RE:CR,RE: !";    
    Phase phase3 {Phase{s1,s2,list}};  
    phase3.set_parameter("PARAMETER G(SIGMA,CR:CR:CR:CR:CR;0)  2.98150E+02  1;  6.00000E+03   N REF283 !");
    phase3.set_parameter("PARAMETER G(SIGMA,CR:CR:CR:CR:RE;0)  2.98150E+02  1;  6.00000E+03   N REF283 !");
//    phase3.set_parameter("PARAMETER G(SIGMA,CR:CR:CR:RE:CR;0)  2.98150E+02  1;  6.00000E+03   N REF283 !");
//    phase3.set_parameter("PARAMETER G(SIGMA,CR:CR:CR:RE:RE;0)  2.98150E+02  1;  6.00000E+03   N REF283 !");
//    phase3.set_parameter("PARAMETER G(SIGMA,RE:RE:RE:CR:RE;0)  2.98150E+02  1;  6.00000E+03   N REF283 !");
//    phase3.set_parameter("PARAMETER G(SIGMA,CR:CR:RE:RE:RE;0)  2.98150E+02  1;  6.00000E+03   N REF283 !");
//    phase3.set_parameter("PARAMETER G(SIGMA,RE:RE:RE:RE:RE;0)  2.98150E+02  1;  6.00000E+03   N REF283 !");
    
    cout<<phase3<<endl; 
    
    List_phases phases {phase3};
    
//    vector<string> temp=generate_endmembers(phase3.get_elsub()); 
//    for (auto x : temp) cout<<x<<endl;
    
    cout<<"N. sub.= "<<phases.get("SIGMA").nsub()<<endl;
    
    List_TPfunctions temp {};
    string boo=phases.calc(temp,"CALCULATE PHASE SIGMA 298.15 1E5 1 1e-20 1e-20 1e-20 1e-20 1e-20");
    
//    DP maxF {0};
//    phase3.add_vib(2001,maxF);
    
//    cout<<" max Fvib = "<<maxF<<endl;
//    cout<<phase3<<endl;  
    
    return 0;
}

