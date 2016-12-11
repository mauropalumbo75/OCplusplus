/* 
 * File:   tpfuntions.h
 * Author: Mauro Palumbo
 * 
 * Classes for handling functions of T and P as for thermodynamic calculations.
 * Each function can have several T ranges (not P ranges for now).
 * 
 * The routines for calculating the actual value of a TPfunction use the GiNaC
 * library for parsing the expression and for symbolic differentiation.
 * You must include the GiNaC headers and library in any code using TPfunctions!
 * 
 * The calculation routines must be revised in a later version for optimization
 * and to better fit the calculation of equilibrium routine. 
 *  
 * Created on January 20, 2015, 17:11 AM
 */

#ifndef TPFUNTIONS_H
#define	TPFUNTIONS_H

#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include "../commoninclude/utils.h"
#include "../commoninclude/math_utils.h"

#include "/usr/local/include/ginac/ginac.h"   // use the GiNaC library for functions evaluation
                                              // and derivatives

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 
// Error handling
//

struct bad_token : runtime_error {
    bad_token(string s1="", string s2="") : runtime_error {s1+s2} {}
};

struct wrong_range : runtime_error {
    wrong_range(string s1="", string s2="") : runtime_error {s1+s2} {}
};

struct tpfunction_out_of_range : runtime_error {
    tpfunction_out_of_range(string s1="", string s2="") : runtime_error {s1+s2} {}
};

struct wrong_function : runtime_error {
    wrong_function(string s1="", string s2="") : runtime_error {s1+s2} {}
};

struct no_such_function : runtime_error {
    no_such_function(string s1="", string s2="") : runtime_error {s1+s2} {}
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

string s1_to_s2(string s, string s1, string s2);

// Two simple functions to switch the symbols for natural logarithms from LN to
// LOG and vice versa
// TDB files use LN style (compatible with ThermoCalc) while this code uses LOG style
string ln_to_log(string s);
string log_to_ln(string s);

// Two simple functions to switch the symbols for powers from fortran style **
// to C style ^ and vice versa
// TDB files use fortran style powers (compatible with ThermoCalc) while this code uses C/C++ style
string pow_to_C_style(string s);
string pow_to_fortran_style(string s);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Range {
public:
    Range(string expr, DP T1=298.15, DP T2=6000)
            : e(pow_to_C_style(trim(expr,";"))), Tl(T1), Th(T2)
    {
        if (Tl<0||Tl>6000||Th<0||Th>6000||Tl>Th) throw wrong_range();
    }
    
    DP Tlow() const { return Tl; }
    DP Thigh() const { return Th; }
    string expr() const { return e; }
    
    bool in_range(DP T) const 
    {   
        if ((T>=Tl)&&(T<=Th)) return true;
        else return false;
    }
    
    void print_TDB(ostream& os=cout);
    void print_TDB(ostream& os=cout) const;
    
private:
    DP Tl;                // low limit of T range
    DP Th;                // TP expression
    string e;             // high limit of T range
};

ostream& operator<<(ostream& os, const Range& r);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class List_TPfunctions;   // forward declaration needed

class TPfunction {
public:
    TPfunction(string name, string expr="0;", DP Tlow=298.15, DP Thigh=6000, string rref="");
    
    TPfunction() : TPfunction("") {}   // The empty constructor is needed for handling the phase class...
    
    string name() const { return n; }
    string ref() const { return rr; }
    size_t size() const { return r.size(); }
    Range range(size_t i) const { return r[i]; } 
    string get_expression(DP T, DP P) const;
    DP calc(List_TPfunctions& list, DP T=298.15, DP P=1e5, string deriv="", int order=1) const;
    
    
    void print_TDB(ostream& os=cout);
    void print_TDB(ostream& os=cout) const;
    
private:
    string n;                   // name
    string rr;                 // eventually a reference tag (ex. REF1)
    vector<Range> r;            // vector containing expression ranges        
};

// Print only the function ranges, with no "FUNCTION NAME" before
ostream& operator<<(ostream& os, const TPfunction& tp);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct TPfunction_order_alphabetic {
    bool operator()(const TPfunction& a, const TPfunction& b) const
    {
        return a.name()<b.name();
    }
}; 

class List_TPfunctions : public set<TPfunction,TPfunction_order_alphabetic> {
public:
    List_TPfunctions()
    {        
        insert(TPfunction("R", "8.314", 0.0, 6.0e3));   // by default, R is always present
    }

    List_TPfunctions(const TPfunction& s)
        : List_TPfunctions()
    {        
        insert(s);
    }
   
    List_TPfunctions(const string& s, string e="0;", DP T1=0, DP T2=6000)
        : List_TPfunctions()
    {        
        insert(TPfunction(s, e, T1, T2));        
    }
         


    TPfunction get(const string n) const;    
    string get_expression(const string n, DP T=298.15, DP P=1e5) const;    
    string substitute_functions(string expression, DP T=298.15, DP P=1e5);
    DP calc(string ph, DP T=298.15, DP P=1e5, string deriv="", int order=1);
    
    bool is(const TPfunction& ph) const;
    bool is(const string name) const;
    
    using set<TPfunction, TPfunction_order_alphabetic>::insert;
    void insert(const string& name, string expr="0;", DP Tlow=298.15, DP Thigh=6000) 
      { insert(TPfunction(name,expr,Tlow,Thigh)); }

    string list() const;                                        // list elements symbols separated by comma
    void print_TDB(ostream& os=cout);                           // print elements with TDB format
    void print_TDB(ostream& os=cout) const;                     // print elements with TDB format
            
};


#endif	/* TPFUNTIONS_H */

