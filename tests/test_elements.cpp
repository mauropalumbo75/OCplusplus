/* 
 * File:   main.cpp
 * Author: mpalumbo
 *
 * Created on January 13, 2015, 11:35 AM
 */

#include <cstdlib>
#include "../include/elements.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
  
    const Element el("Ni");
    el.print_TDB();
    
    Element el2 {"Al"};
    el2.print_TDB();
    
        
    List_elements pippo {el,el2};
    List_elements paperino {Element{"Ni"},Element{"Al"}};
    
    vector<string> temp {"Al","Co","W"};
    List_elements pluto {temp};
    
    cout<<"This is pluto: "<<endl;
    pluto.print_TDB();

    pippo.insert(Element("H"));
    pippo.insert(Element("O"));
    pippo.insert("Re");
    cout<<pippo.size()<<endl;
    
    if (pippo.is("re")) cout<<"Found!"<<endl;
    else cout<<"Not found!"<<endl;
    
    string s;
    
    string ss=" ELEMENT H    1/2_MOLE_H2(G)       1.00794E+00  4.2340E+03  6.5285E+01";
    
    Element p(ss);
    cout<<p<<endl;
    
    while (cin>>s&&s!="*") {
    
       try {
         pippo.insert(s);
         Element p=pippo.get(s);
         cout<<p;
       }
       catch (no_such_element) { cout<<"No such element!"<<endl; }
    }

    cout<<"N. Elements: "<<pippo.size()<<" list elements: "<<pippo.list()<<endl;
    
    for (int i=0; i<pippo.size(); i++) 
        cout<<"Element("<<i<<") in list: "<<pippo[i].symbol()<<endl;

    cout<<"\nWhich elements you want to get? \n";
    while (cin>>s&&s!="*") {
    
       try {
          Element temp(s);
          if (pippo.is(s))
            cout<<temp;
          else throw no_such_element {"iii"}; 
       }
       catch (out_of_range) { cout<<"Out of range!"<<endl; }
       catch (no_such_element& p) { cout<<"No such element! "<<p.msg<<endl; }
    } 
    
    cout.clear();
    cout<<"List elements: "<<pippo.list()<<endl;
    pippo.print_TDB(cout);
    cout<<endl;  
                
    return 0;
}

