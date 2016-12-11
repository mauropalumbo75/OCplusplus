/* 
 * File:   eos.h
 * Author: Mauro Palumbo
 *
 * Created on 27 April 2015, 13:33
 */

#ifndef EOS_H
#define	EOS_H

#include <random>	// necessary for random numbers
#include <functional>   // necessary for bind
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include "../commoninclude/physconsts.h"
#include "../commoninclude/math_utils.h"
#include "../commoninclude/utils.h"

struct no_such_EV {};


class EV {
public:   
    EV(const EV& ev);
    EV(string fname, string name, string lattice="V", string v_unit="au", string e_unit="Rydberg", string norm="at");   
    EV(vector<DP> tempv, vector<DP> tempe, string name="",string lattice="V", string v_unit="au", string e_unit="Rydberg", string norm="at");      
    EV(string name="")        
        : n(name), chi(big), fitt(0), nminus(-1), nplus(-1)
    {
    } 
    
    string name() const { return n; }
    bool fitted() const { return fitt; }

    size_t size() const { return v.size(); }
    size_t npar() const { return p.size(); }
 
    DP V(const size_t i) const { if (i<v.size() && i>=0) return v[i]; else return 0.0; }
    DP E(const size_t i) const { if (i<e.size() && i>=0) return e[i]; else return 0.0; }
   
    // extract important quantities in SI units
    DP E_V(const DP V) { if (fitt) return Efit(V/sqr3(Bohr_radius)/NA)*Rydberg_to_J*NA; throw; }
    DP P_V(const DP V) { if (fitt) return Pfit(V/sqr3(Bohr_radius)/NA) *Rydberg_to_kbar*1.E8; }
    DP H_V(const DP V) { if (fitt) return Hfit(V/sqr3(Bohr_radius)/NA)*Rydberg_to_J*NA;  throw; }
    
    DP E_P(const DP P) { if (fitt) return Efit(Vfit(P/Rydberg_to_kbar/1.E8))*Rydberg_to_J*NA;  throw; }
    DP H_P(const DP P) { if (fitt) return Hfit(Vfit(P/Rydberg_to_kbar/1.E8))*Rydberg_to_J*NA;  throw; }
    DP V_P(const DP P) { if (fitt) return p[1] * pow( 1.0+P/Rydberg_to_kbar/1.E8*p[3]/p[2], -1.0/p[3] ) *sqr3(Bohr_radius)*NA; throw; }

    DP E0() const { if (fitt) return p[0]*Rydberg_to_J*NA; throw; }       // E0 is the minimum energy from the fit, in J/mol
    DP V0() const { if (fitt) return p[1]*sqr3(Bohr_radius)*NA; throw; }  // V0 is the minimum volume from the fit, in m^3/mol
    DP B0() const { if (fitt) return p[2]*Rydberg_to_kbar*1.E8; throw; }  // B0 is the bulk modulus from the fit, in Pa
    DP B0p() const { if (fitt) return p[3]; throw; }                      // B0p (B0') is the first derivative of the bulk modulus, adimensional

    DP chisq() const { if (fitt) return chi*Rydberg_to_J*NA; throw; } 
        
    // Execute the fit!
    void fit();
    
    void set_fit_limits(int minus=-1, int plus=-1);
 
    bool write_to_file(string fname="", string lattice="V", string v_unit="au",  string e_unit="Rydberg", string norm="at");     
    bool read_from_file(string fname="input_file", string lattice="V", string v_unit="au",  string e_unit="Rydberg", string norm="at");
    void print_data(ostream& os=cout);
    void print_results(ostream& os=cout, string lattice="V", string v_unit="au", string e_unit="Rydberg", string norm="at");

private:
    // set of private methods used in the fitting (units are as in storage)
    DP Efit(const DP V) { return p[0] -p[2]*p[1]/(p[3]-1.0) + V*p[2]/p[3]*( pow(p[1]/V,p[3])/(p[3]-1.0)+1.0 ); }
    DP Pfit(const DP V) { return p[2]/p[3]*(pow(p[1]/V,p[3])-1.0); }
    DP Vfit(const DP P) { return p[1] * pow( 1.0+P*p[3]/p[2], -1.0/p[3] ); }
    DP Hfit(const DP V) { return this->Efit(V) + this->Pfit(V) * V; }
    DP calc_E0();
    DP calc_chi();

    DP Efit(const DP V, const vector<DP> par) 
        { return par[0] - par[2]*par[1]/(par[3]-1.0) + V*par[2]/par[3]*( pow(par[1]/V,par[3])/(par[3]-1.0)+1.0 ); }
    DP calc_E0(const vector<DP> p);
    DP calc_chi(const vector<DP> p, const DP E0);
    
    void initialize_parameters();

    // private data
    string n;           // EV identifier, a string to identify it
    
    DP chi;             // chi square
    size_t pos_min;     // position of the minimum energy (data) and corresponding volume
    int nminus,nplus;   // number of data point to the left and right of posEmin to be used in the fit
    
    bool fitt;    // Has the EOS been calculated from the data?

    // v and e contains the whole data set
    vector<DP> v;   // volume in (a.u.)^3/at 
    vector<DP> e;   // energy in Rydberg/at
    
    // vfit and efit are subsets of v and e on which the fit is performed. You can set them with the function set_fit_limits
    vector<DP> vfit;   // volume in (a.u.)^3/at 
    vector<DP> efit;   // energy in Rydberg/at
    
    vector<DP> p;   // vector of parameters of the EOS (par[0]=E0, par[1]=V0, par[2]=B0, par[3]=B0')
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class List_EV : public map<string,EV>  { 
public:     
    EV get(const string sp) const;     
    EV get(const EV& ev) const;     
    
    bool is(const EV& sp) const;
    bool is(const string s) const;
    
    void fit();   
    void set_fit_limits(int minus=-1, int plus=-1);
    
    string list() const;             // list EV names separated by comma
      
    bool read_from_file(string fname="input_file", string lattice="V", string v_unit="au",  string e_unit="Rydberg", string norm="at");
    bool read_from_file(vector<string> fname, string lattice="V", string v_unit="au",  string e_unit="Rydberg", string norm="at");
    bool write_to_file(string fname="", string lattice="V", string v_unit="au",  string e_unit="Rydberg", string norm="at");   
    bool write_to_file(vector<string> fname, string lattice="V", string v_unit="au",  string e_unit="Rydberg", string norm="at");     
    void print_data(ostream& os=cout);
    void print_results(ostream& os=cout, string lattice="V", string v_unit="au", string e_unit="Rydberg", string norm="at");

};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Auxiliary functions
// Convert back and forth volume (lattice parameters)
DP convertV(DP V, string lattice="", string vunit="au", string nnorm="at");
DP convertVback(DP V, string lattice="", string vunit="au", string nnorm="at");

DP convertE(DP V, string eunit="Rydberg", string nnorm="at");
DP convertEback(DP V, string eunit="Rydberg", string nnorm="at");

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#endif	/* EOS_H */

