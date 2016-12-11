/*
 * File:   elements.h
 * Author: Mauro Palumbo
 *
 * This file contains the classes and methods for the "elements" in the thermodynamic
 * library. Nearly all atoms in the periodic table are included.
 *
 * WARNING! The values here for atomic weight, standard enthalpy and entropy are from
 * the paper of A. Dinsdale CALPHAD 1991. There are revisions which are not included.
 * If the values read from a tdb file are different from the ones stored here, a warning
 * is printed and Dinsdale values neglected.
 *
 * Created on January 13, 2015, 11:35 AM
 */

#ifndef ELEMENT_H
#define	ELEMENT_H

#include <string>
#include <set>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "utils.h"
#include "math_utils.h"


struct no_such_element {
    string msg;
    no_such_element(string s) : msg(s) {}
    no_such_element() {}
};
struct out_of_range {};


class Element {
public:
    Element(string nn, int aa, DP ww, string rr, DP hh, DP ss);

    Element(string ss);

    string symbol() const { return n; }
    int atomic_number() const { return a; }
    DP mass() const { return w; }
    string ref() const { return r; }
    DP h298() const { return h; }
    DP s298() const { return s; }

    void print_TDB(ostream& os=cout);
    void print_TDB(ostream& os=cout) const;

private:
    string n;   // symbol
    int a;      // atomic number
    DP w;       // atomic weight
    string r;   // reference phase
    DP h;    // standard enthalpy
    DP s;    // standard entropy
};

ostream& operator<<(ostream& os, const Element& e);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct Element_order_alphabetic {
    bool operator()(const Element& a, const Element& b) const
    {
        return a.symbol()<b.symbol();
    }
};

// A List_elements is a set of Elements with additional functionalities...
class List_elements : public set<Element,Element_order_alphabetic> {
public:
    List_elements() {};
    List_elements(const Element& el) { emplace(el); }
    List_elements(const vector<string>& lst) { for (auto x:lst) emplace(x); }
    List_elements(initializer_list<Element> list)
    { for (auto x : list) emplace(x); }

    Element get(const string el) const;
    Element& operator[](const size_t);
    const Element& operator[](const size_t) const;

    // Is the element in the list?
    bool is(const Element& el) const;
    bool is(const string el) const;

    using set<Element,Element_order_alphabetic>::insert;  // make available standard set::insert functions
    void insert(const string el) { emplace(el); }

    // Specific output functions
    string list() const;                    // list elements symbols separated by comma
    void print_TDB(ostream& os=cout);       // print elements data (TDB format)

};


#endif	/* ELEMENT_H */

