/* 
 * File:   main.cpp
 * Author: mpalumbo
 *
 * Created on January 20, 2015, 17:11 AM
 */

#include <cstdlib>
#include "../include/tpfunctions.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    try {
/* 
    string s1,s2;
    
    s1="3*LN(3+T)+ALNOT+LOG(10)+TOLOGY+4*LN(1+LN(T*P))+LOG(T)+LN(P)";
 //   s1="3*LN(3+T)+LN(4)+ALNOT+LN(10)+";
    cout<<s1<<endl;
    cout<<s1_to_s2(s1,"LN","LOG")<<endl;
    cout<<s1_to_s2(s1,"LOG","LN")<<endl;
 

      Range p("0;", 600);
 
    
      cout<<"In range? "<<p.in_range(500)<<endl;
      cout<<"In range? "<<p.in_range(50)<<endl;
      p.print_TDB(cout);
      cout<<"\n\n";
     
      TPfunction prova0("FUNCTION  ALPHA	1	1;	200   Y   3*T;  4000 Y  50*T^2; 6000 N ");
      TPfunction prova("FUNCTION  BETA	1	T+R+ALPHA;	3.000000e+02   Y   1+ALPHA;  4000 Y  50*T^2; 6000 N ");
      TPfunction prova2("FUNCTION  GHSERAL	1	BETA;	1.000000e+03   Y   BETA;  2000 Y  50*T^2; 6000 N ");
      prova.print_TDB(cout);
      cout<<prova.get_expression(300,1)<<endl;
      cout<<prova.get_expression(3500,1)<<endl;
      cout<<prova.get_expression(5000,1)<<endl;
      List_TPfunctions temp {prova};
      temp.insert(prova2);         temp.insert(prova0);
      temp.print_TDB(cout);
      cout<<"\n"<<prova2.name()<<" (T="<<2<<",P="<<4<<")= "<<prova2.calc(temp,1001,4)<<endl;
      
      TPfunction prova10("FUNCTION TEST 0 T^3+P*4; 6000 N REF1 !");
      prova10.print_TDB(cout);
      cout<<prova10.get_expression(1,1)<<endl;
*/      
      List_TPfunctions list {};
      
      TPfunction prova0("FUNCTION  ALPHA 50 1; 200   Y   3*T;  3000 Y  0.0001*T; 6000 N ");
      list.insert(prova0);
      list.insert("GHSERRE","2*T+R+ALPHA",500,6000);
      list.insert("GHSERTi","2*T^3+R",0,6000);
      list.insert("wrong","GHSERRE",0,6000);
      
      list.print_TDB(cout);
      
      cout<<list.is("GHSERTi")<<"   "<<list.is("pippo")<<endl;
      
//      cout<<list.get_expression("GHSERTI",1,1)<<endl;
      
      cout<<"\n"<<list.get("ALPHA").name()<<" (T="<<2<<",P="<<4<<")= "<<list.calc("ALPHA",100000,4)<<endl;
      cout<<"\nDerivative: "<<list.get("GHSERTi").name()<<" (T="<<2<<",P="<<4<<")= "<<list.calc("CALCULATE GHSERTi 2 4 T 1")<<endl;
      cout<<"\nFunction: "<<list.get("wrong").name()<<" (T="<<2<<",P="<<4<<")= "<<list.calc("CALCULATE wrong 2 4 T 1")<<endl;
      cout<<"\n"<<list.get("wrong").name()<<" (T="<<2<<",P="<<4<<")= "<<list.calc("wrong",1,4)<<endl;
      
    }
    catch (std::runtime_error& e) {
        cerr<<e.what()<<endl;
    }
      
    return 0;
}

