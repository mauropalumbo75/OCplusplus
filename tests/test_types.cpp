/* 
 * File:   main.cpp
 * Author: mauro
 *
 * Created on February 21, 2015, 8:11 AM
 *
 */

#include "types.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Type prova {"TYPE_DEFINITION & GES A_P_D BCC_A2 MAGNETIC  -1.0    4.00000E-01 "};
    cout<<prova.id()<<" "<<prova.full()<<"\t"<<prova.size()<<endl;
    for (int i=0; i<prova.size(); i++)
       cout<<"\n"<<prova[i];
    cout<<"\n\n";  
    prova.print_TDB(cout);
    
    List_types list {prova};
    
    list.insert("TYPE_DEFINITION A GES A_P_D BCC_A2 MAGNETIC  -2.0    grfgrgrwe ");
    list.insert("TYPE_DEFINITION ( GES A_P_D BCC_A2 MAGNETIC  -12.0    pippo");
    list.print_TDB(cout);
    
    cout<<"List: "<<list.list()<<endl;
    
    cout<<"A: "<<list.get('A')<<endl;
    
    return 0;
}

