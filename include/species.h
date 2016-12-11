/* 
 * File:   species.h
 * Author: Mauro Palumbo
 *
 * This file contains classes and methods for species. A species is a combination
 * of elements (or ions) with a fixed ratio. They can have a net charge different
 * from zero. 
 *
 * Created on January 15, 2015, 11:35 AM
 */

#ifndef SPECIES_H
#define	SPECIES_H

#include <string>
#include <vector>
#include "elements.h"
#include "utils.h"
#include "math_utils.h"

struct wrong_species {};
struct no_such_species {};
    

class Species {
public:     
    Species(string def, string name="");
              
    string symbol() const { return n; }
    DP mass() const { return m; }
    DP charge() const { return c; }
    const Element& element(int i) const { if (i<el.size()) return el[i]; }
    DP stoichiometry(int i) const { if (i<stoich.size()) return stoich[i]; }

    size_t size() const { return el.size(); }
   
    void print_TDB(ostream& os=cout);
    void print_TDB(ostream& os=cout) const;
    
private:
    string n;   // species name (symbol)
    DP m;       // mass
    DP c;       // charge

    vector<Element> el;  // we do not use here List_elements because the order matters
    vector<DP> stoich;  // stoichiometry of each element as in vector el 
};

ostream& operator<<(ostream& os, const Species& e);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct Species_order_alphabetic {
    bool operator()(const Species& a, const Species& b) const
    {
        return a.symbol()<b.symbol();
    }
}; 


class List_species : public set<Species, Species_order_alphabetic> { 
public:
    
    Species get(const string sp) const;     
    bool is(const Species& sp) const;
    bool is(const string s) const;

    using set<Species, Species_order_alphabetic>::insert;  // make available standard set::insert functions    
    void insert(const string s) { insert(*new Species(s)); }
    void insert(const Element& el) { insert(*new Species(el.symbol()+"1")); }  // an element can become a species (default stoichiometry=1)
    void insert(const List_elements& listel);
    
    string list() const;             // list species symbols separated by comma
    void print_TDB(ostream& os=cout);     // print species using TDB format

};

#endif	/* SPECIES_H */

