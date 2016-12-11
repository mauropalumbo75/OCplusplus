/* 
 * File:   main.cpp
 * Author: mauro
 *
 * Created on January 29, 2015, 9:14 AM
 */

#include <cstdlib>
#include "../include/configuration.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    vector<string> ee {"Cr","Re"};
    Configuration p {ee};
    
    Element cr {"Cr"};
    Element re {"Re"};  
    vector<List_elements> lee {{cr,re},Element{"Al"}};
    Configuration pp {lee};
    
    string s {"Cr::Cr,Fe:Fe:Al,Cr,Fe"};
    Configuration ppp {s};
    
    cout<<p.to_string()<<endl;
    cout<<pp.to_string()<<endl;
    cout<<ppp.to_string()<<endl;
    
    cout<<p.to_string_nocolons()<<endl;
    cout<<pp.to_string_nocolons()<<endl;
    cout<<ppp.to_string_nocolons()<<endl;
    
    string ss;
    while (cin>>ss&&ss!="*") {
    
       try {
           Configuration temp {ss};
           cout<<temp.to_string()<<endl;           
           cout<<temp.to_string_nocolons()<<endl;
           cout<<"Is an endmember? "<<temp.is_endmember()<<endl;
       }
       catch (no_such_element) { cout<<"No such element!"<<endl; }
       catch (empty_configuration) { cout<<"Empty configuration!"<<endl; }
    }
    
    return 0;
}

