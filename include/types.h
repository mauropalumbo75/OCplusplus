/* 
 * File:   type.h
 * Author: Mauro Palumbo
 *
 * This file defines the classes Type, List_types and related methods for handling
 *  the "TYPE_DEFINITION" command in the tdb format.
 * 
 * At the moment, this is just a container, for the type identifier and related command.
 * The proper handling of the types is done elsewhere or not jet implemented in 
 * actually many cases.
 * 
 * Created on February 21, 2015, 8:11 AM
 */

#ifndef TYPE_DEFINITION_H
#define	TYPE_DEFINITION_H

#include <string>
#include <set>
#include <iostream>
#include <sstream>
#include <cctype>
#include "../include/utils.h"
#include "../include/math_utils.h"


struct no_such_type { 
    string msg;
    no_such_type(string s) : msg(s) {}
    no_such_type() {}
};
  

class Type {
public:
    Type(char ii, vector<string> cc={})
        : ident(ii), v(cc)
        {
        }
       
    Type(string ss);
       
    char id() const { return ident; }
    string full() const; 
    const size_t size() const { return 1+v.size(); }
    string operator[](const size_t);
    const string operator[](const size_t) const; 
    
    void print_TDB(ostream& os=cout);
    void print_TDB(ostream& os=cout) const;
    
private:
    char ident;     // first keyword (ex. %), which identifies the type and is used afterwards for this
    vector<string> v;   // secondary keyword with parameters (ex. GES A_P_D BCC_A2 MAGNETIC  -1.0    4.00000E-01)
};

ostream& operator<<(ostream& os, const Type& t);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct Type_order_alphabetic {
    bool operator()(const Type& a, const Type& b) const
    {
        return a.id()<b.id();
    }
}; 
    
// A List_elements is a set of Elements with additional functionalities...
class List_types : public set<Type,Type_order_alphabetic> {
public:
    List_types() {};
    List_types(const Type& el) { emplace(el); }
    List_types(const vector<string>& lst) { for (auto x:lst) emplace(x); }
    List_types(initializer_list<Type> list) 
       { for (auto x : list) emplace(x); }
    
    Type get(const char el) const;     
    
    // Is the element in the list?
    bool is(const Type& el) const;
    bool is(const char el) const;

    using set<Type,Type_order_alphabetic>::insert;  // make available standard set::insert functions
    void insert(const string typ) { emplace(typ); }
    
    // Specific output functions
    string list() const;                    // list elements symbols separated by comma
    void print_TDB(ostream& os=cout);       // print elements data (TDB format)

};

#endif	/* TYPE_DEFINITION_H */

