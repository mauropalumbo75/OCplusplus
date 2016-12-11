/* 
 * File:   main.cpp
 * Author: mpalumbo
 *
 * Created on January 15, 2015, 11:35 AM
 */

#include <cstdlib>
#include "../include/species.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    //Species test("SPECIES h2/-5o1/2 pippo");
    
    //cout<<test<<endl;
    
    
    List_species pippo;
    string s;
    
    Element test2("H");

    pippo.insert(test2);
    pippo.print_TDB(cout);
      
    pippo.clear();


    while (cin>>s&&s!="*") {
    
       try {
         pippo.insert(s);
         Species p=pippo.get(s);
         cout<<p<<endl;
       }
       catch (no_such_species) { cout<<"No such species in the list!"<<endl; }
       catch (wrong_species) { cout<<"Wrong species!"<<endl; }
    }

    cout<<"N. species: "<<pippo.size()<<" list species: "<<pippo.list()<<endl;
    
    pippo.print_TDB(cout);

    cout<<"\nWhich species you want to get? \n";
    while (cin>>s&&s!="*") {
    
       try {
          if (pippo.is(s))
            cout<<pippo.get(s)<<endl;
          else cout<<"Not in the list!"<<endl;
       }
       catch (no_such_species) { cout<<"No such species in the list!"<<endl; } // should't be possible to get here...
       catch (wrong_species) { cout<<"Wrong species!"<<endl; }
    } 
        
    pippo.print_TDB(cout);

             
    string ss="SPECIES   H1O100                      pippo";
    
    Species p(ss);
    cout<<p<<endl;
    p.print_TDB(cout);

    return 0;
}

