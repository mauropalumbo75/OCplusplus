/* 
 * File:   main.cpp
 * Author: mpalumbo
 *
 * Created on March 15, 2014, 4:55 PM
 * 
 * Just for testing the structure and derived classes...
 * 
 */

#include <cstdlib>
#include "../include/structure.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    string filename="structures_database";
    List_structures str;
    
    str.read(filename);
    str.print();
    
    cout<<"\n"<<str.get("A15");

    string s="  PHASE A15 fff";
    Structure p(s);
    cout<<p<<endl;
    
    return 0;
}

