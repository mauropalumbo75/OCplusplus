/* 
 * File:   fv.h
 * 
 * Author: Mauro Palumbo
 *
 * Created on 20 August 2015, 13:33
 */

#ifndef FV_H
#define	FV_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include "../commoninclude/physconsts.h"
#include "../commoninclude/math_utils.h"
#include "../commoninclude/utils.h"

struct no_such_FV {};

class FV {
public:   
    FV(const FV& fv);
    FV(string fname, string name);   
    FV(vector<DP> tempv, vector<DP> tempe, string name="");      
    FV(string name="")        
        : n(name), chi(big), fitt(0)
    {
    } 
    
    string name() const { return n; }
    bool fitted() const { return fitt; }

    size_t size() const { return v.size(); }
    size_t npar() const { return p.size(); }
    
    DP F(const size_t i) const { if (i<f.size() && i>=0) return f[i]; else return 0.0; }
   
    // extract important quantities in SI units
    DP F_V(const DP V) { if (fitt) return Ffit(V/sqr3(Bohr_radius)/NA)*Rydberg_to_J*NA; throw; }

    DP chisq() const { if (fitt) return chi*Rydberg_to_J*NA; throw; } 
        
    // Execute the fit!
    void fit();
    

    bool write_to_file(string fname="");     
    bool read_from_file(string fname="input_file");
    void print_data(ostream& os=cout);
    void print_results(ostream& os=cout);

private:
    // set of private methods used in the fitting (units are as in storage)
    DP Ffit(const DP V) { return p[0]+p[1]*V+p[2]*V*V; }
    DP Fpoly(DP xi, int i) { return pow(xi,i); }

    DP calc_chi();

    void initialize_parameters();

    // private data
    string n;           // FV identifier, a string to identify it
    
    DP chi;             // chi square
    
    bool fitt;    // Has the EOS been calculated from the data?

    // v and f contains the whole data set
    vector<DP> v;   // volume in (a.u.)^3/at 
    vector<DP> f;   // energy in Rydberg/at
    
    vector<DP> p;   // vector of parameters of the polynomial aV^2+bV+c (p[0]=a, p[1]=b, p[2]=c)
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class List_FV : public map<string,FV>  { 
public:     
    FV get(const string sp) const;     
    FV get(const FV& ev) const;     
    
    bool is(const FV& sp) const;
    bool is(const string s) const;
    
    void fit();   
    
    string list() const;             // list FV names separated by comma
      
    bool read_from_file(string fname="input_file");
    bool read_from_file(vector<string> fname);
    bool write_to_file(string fname="");   
    bool write_to_file(vector<string> fname);     
    void print_data(ostream& os=cout);
    void print_results(ostream& os=cout);

};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif	/* FV_H */

