/* 
 * File:   references.h
 * Author: Mauro Palumbo
 *
 * This file implements the simple class Reference for handling references in a
 * tdb file. It is quite simple and self-explaining.
 * 
 * Created on February 12, 2015, 2:26 PM
 */

#ifndef REFERENCES_H
#define	REFERENCES_H

#include <string>
#include <set>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "../commoninclude/utils.h"

using namespace std;

struct wrong_ref : runtime_error {
    wrong_ref(string s1="", string s2="") : runtime_error {s1+s2} {}
};


class Reference {
public:
    Reference(string name, string reference)
        : n(name), r(reference)
    {}
    
    Reference(string refall);
    
    string tag() const { return n; } 
    void print_TDB(ostream& os=cout);                           // print a Reference with TDB format
    void print_TDB(ostream& os=cout) const;                           // print a Reference with TDB format  
    
private:
    string n;
    string r;
};


struct Reference_order_alphabetic {
    bool operator()(const Reference& a, const Reference& b) const
    {
        if (a.tag().size()<b.tag().size()) 
            return true;
        else if (a.tag().size()>b.tag().size()) 
            return false;
        else
            return a.tag()<b.tag();
    }
}; 

class List_references : public set<Reference,Reference_order_alphabetic> {
public:
    List_references() {};
    List_references(const Reference& r) { emplace(r); }
    List_references(string s);   // accept as a single string a list of references
    
    using set<Reference,Reference_order_alphabetic>::insert;
    void insert(string s); // accept as a single string a list of references
    
    void print_TDB(ostream& os=cout);      // print a references in the list with TDB format
};

    
vector<Reference> handle_refs_list(string s);
    
#endif	/* REFERENCES_H */

