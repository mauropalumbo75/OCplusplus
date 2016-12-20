/* 
 * File:   phases.h
 * Author: Mauro Palumbo
 * 
 * This file contains the classes and methods for phases. Notably, they are the 
 * most complicated ones and require including almost all other classes.
 * 
 * To do:
 * second derivatives of the excess term
 *
 * Created on March 15, 2015, 2:55 PM
 */

#ifndef PHASES_H
#define	PHASES_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include "utils.h"
#include "math_utils.h"
#include "physconsts.h"
#include "elements.h"
#include "structure.h"
#include "tpfunctions.h"
#include "types.h"
#include "y.h"
#include "configuration.h"
#include "magnetic.h"

struct too_many_or_too_few_components {};
struct no_such_element_in_phase {};
struct unknown_structure {};
struct no_such_phase {};
struct wrong_parameter_order {};
struct wrong_parameter_command {};


// A small class derived by vector for the paramater hierarchy
// It makes sure that the list of member is continuous from 0 to n order
class Vec_par : public vector<TPfunction> {
public:
    void push_back(TPfunction f,int ord);
};


// Auxiliary function to generate all possible endmembers configurations 
// given a set of sublattices and their constituents in vector<List_elements> x
// Return a vector of endmembers configurations as strings (CR:CR:CR,CR:CR:NI,...)
vector<string> generate_endmembers(vector<List_elements> x);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Phase : public Structure {
public:
    struct Configuration_order_alphabetic {
        bool operator()(const Configuration& a, const Configuration& b) const
        {
            return a.to_string()<b.to_string();
        }
    }; 
    
    // Define an alias for complex types G_ref and G_ex used to store the parameters
    using G_ref_type = map<Configuration,TPfunction,Configuration_order_alphabetic>;
    using G_ex_type = map<Configuration,Vec_par,Configuration_order_alphabetic>;
   
    // Constructors for Phase class
    // name structure, list of all structure loaded, n sublattices, vector with multiplicities
    Phase(string sn, int nns, vector<DP> mms, List_structures& ls, List_elements& el_list, List_types typ={})
        : Structure(ls.get(sn)), ns(nns), ms(mms), t(typ)         
    {      
        for (int i=0; i<ns; i++)
            elsub.push_back(el_list);    // by default, put all elements in each sublattice
        
        nm=0; for (int i=0; i<ms.size(); i++) nm+=ms[i];  
        
        initialize();
    }
    
    // name structure, list of all structure loaded, n sublattices, vector with multiplicities
    Phase(string sn, List_structures& ls, List_elements& el_list, List_types typ={})
        : Structure(ls.get(sn)), ns(Structure::nWyckoff()), t(typ)    
    {
        for (int i=0; i<ns; i++)
            ms.push_back(Structure::mWyckoff(i));  // if not specified, take the number of sublattices
                                                   // and their multiplicities from Structure
        
        for (int i=0; i<ns; i++) 
            elsub.push_back(el_list);    // by default, put all elements in each sublattice
        
        nm=0; for (int i=0; i<ms.size(); i++) nm+=ms[i];  
        
        initialize(); 
    }
    
    // Constructor to initialize an "empty" phase or to handle the TDB format
    Phase(string phase_def, string constituent_def="", List_types typ={});
    
    
    string name() const { return get(Structure::name,-1); }
    int nsub() const { return ns; }
    DP msub(const int i) const { return ms[i]; }
    DP nmol() const { return nm; }
    List_elements get_elements_in_sub(const int i) const { return elsub[i]; }
    vector<List_elements> get_elsub() const { return elsub; }
    List_types get_types() const { return t; }

    // handles input string in tdb format such as G(BCC,Al,Ni:Ni;0)
    void set_parameter(const string& par);
    
    
//    void set_G_ref(Configuration& c, TPfunction g_ref)  { if (check_conf(c)) G_ref[c]=g_ref; }    // Wrong!!! must multiply per nm the J/mol value from ab initio!!!
//    void set_G_ref(const string& c, TPfunction g_ref)  { if (check_conf(c)) G_ref[*new Configuration(c)]=g_ref; }  // Wrong!!! must multiply per nm the J/mol value from ab initio!!!
//    TPfunction get_G_ref(Configuration& c) { if (check_conf(c)) return G_ref[c]; }   
//    TPfunction get_G_ref(const Configuration& c) { if (check_conf(c)) return G_ref[c]; }   
//    TPfunction get_G_ref(const string& c) { if (check_conf(c)) return G_ref[*new Configuration(c)]; }   

    void set_G_ref(Configuration& c, TPfunction g_ref)  { if (check_conf(c)) G_ref[c]=TPfunction("","1;"); }    // Wrong!!! must multiply per nm the J/mol value from ab initio!!!
    void set_G_ref(const string& c, TPfunction g_ref)  { if (check_conf(c)) G_ref[*new Configuration(c)]=TPfunction("","1;"); }  // Wrong!!! must multiply per nm the J/mol value from ab initio!!!
    TPfunction get_G_ref(Configuration& c) { if (check_conf(c)) return TPfunction(""); }   
    TPfunction get_G_ref(const Configuration& c) { if (check_conf(c)) return TPfunction(""); }   
    TPfunction get_G_ref(const string& c) { if (check_conf(c)) return TPfunction(""); }   
    
    void set_Y(const string el, const int sub=0, const DP yy=0) { y[sub].insert(el,yy); }
    void set_Y(const Y& yy, const int sub=0) { y[sub].insert(yy); }
    List_Y& get_List_Y(const int sub) { return y[sub]; }
    Y get_Y_full(const string el, const int sub=0) const { return y[sub].get(el); }
    DP get_Y(const string el, const int sub=0) const { return y[sub].get(el).y(); }
    
//    DP estimate_ternary_E_tot(const Configuration& c);
//    DP estimate_ternary_G_ref(const Configuration& c);
//    void add_vib(DP T, DP& maxF);     // temporary solution for adding Fvib at a given temperature T 
    
    string calc(List_TPfunctions& list, DP T, DP P, DP nmol, vector<DP> y);

//    void list_Y() const;
    
    
    const G_ref_type& get_G_ref() const { return G_ref; }   
    const G_ex_type& get_G_ex() const { return G_ex; } 
    const G_ref_type& get_TC_ref() const { return TC_ref; } 
    const G_ex_type& get_TC_ex() const { return TC_ex; } 
    const G_ref_type& get_BMAGN_ref() const { return BMAGN_ref; } 
    const G_ex_type& get_BMAGN_ex() const { return BMAGN_ex; } 

    bool check_conf(const Configuration& c);
    void initialize();
//    void get_binary_configurations(const Configuration& e, vector<Configuration>& vconf, vector<DP>& weights);
    
    DP calc_deriv_ref_type(const G_ref_type& ref, List_TPfunctions& list, const DP T=298.15, const DP P=1E5, int sub=0, string deriv="", int sub2=0, string deriv2="") const;
    DP calc_ref_type( const G_ref_type& ref, List_TPfunctions& list, const DP T=298.15, const DP P=1E5, int sub=0, string deriv="") const;
    DP calc_deriv_ex_type(const G_ex_type& ex, List_TPfunctions& list, const DP T=298.15, const DP P=1E5, int sub=0, string deriv="", int sub2=0, string deriv2="") const;
    DP calc_ex_type(const G_ex_type& ex, List_TPfunctions& list, const DP T=298.15, const DP P=1E5, int sub=0, string deriv="") const;

    DP calc_G_ref(List_TPfunctions& list, const DP T=298.15, const DP P=1E5, int sub=0, string deriv="", int sub2=0, string deriv2="") const;
    DP calc_S_conf(List_TPfunctions& list, int sub=0, string deriv="", int sub2=0, string deriv2="") const;
    DP calc_G_ex(List_TPfunctions& list, const DP T=298.15, const DP P=1E5, int sub=0, string deriv="", int sub2=0, string deriv2="") const;
    DP calc_TC(List_TPfunctions& list, const DP T=298.15, const DP P=1E5, int sub=0, string deriv="", int sub2=0, string deriv2="") const;
    DP calc_BMAGN(List_TPfunctions& list, const DP T=298.15, const DP P=1E5, int sub=0, string deriv="", int sub2=0, string deriv2="") const;
    DP calc_Gmagn(List_TPfunctions& list, const DP T=298.15, const DP P=1E5, int sub=0, string deriv="", int sub2=0, string deriv2="") const;
    DP calc_G(List_TPfunctions& list, const DP T=298.15, const DP P=1E5, int sub=0, string deriv="", int sub2=0, string deriv2="") const;
  
private:
    int ns;                        // number of sublattices (could be different from the nWyck in Structure)
    vector<DP> ms;                 // multiplicity of each sublattice
    DP nm;                         // number of mol of the phase (sum of the multiplicities over all sublattices)
    vector<List_elements> elsub;   // each vector element is a list of the elements in the sublattice [i]
                                   // (to be improved, should consider species)
    string option="";              // eventually option :F or :B in fcc, bcc, hcp phases. Default no option
    List_types t;                  // List of TYPE_DEFINITIONs for this phase
                
    G_ref_type G_ref;               // map of all possible G_ref energies  
    G_ex_type  G_ex;                // map of all possible excess energies     
    
    // magnetic parameters
    DP p=0.0,D=0.0,AFM=-1.0;        // these are the magnetic parameters in the Inden model and the antiferromagnetic factor
                                    // p=0.40, DP D=1.55828482 for bcc phases, p=0.28, DP D=2.342456517 for other phases

    G_ref_type TC_ref;              // these are the magnetic parameters TC and BMAGN for the phase    
    G_ex_type TC_ex;                // there may be parameters only for the endmembers or interaction parameters
    G_ref_type BMAGN_ref;           // although not very often, in the general case they can be TPfunctions and not only real numbers 
    G_ex_type BMAGN_ex;             // and there can also be excess terms...
                                                
    vector<List_Y> y;

};

ostream& operator<<(ostream& os, const Phase& p);
void print_estimates(ostream& os, const Phase& p);    

// Help function for handling the PARAMETER command string.  
// Input is the full PARAMETER definition in par. It returns the phase name.
// It also returns the PARAMETER definition without the "PARAMETER" command in definition,
// the configuration in conf, the order of the parameter in order and what kind
// of parameter (G,TC,BMAGN)
string handle_par(string par, string& definition, string& conf, int& order, string& kind);

// get option ':F' or ':B' or ':L' from phase name, if present. Else return '0'
string get_option(string name);

// Remove option ':F' or ':B' or ':L' from phase name, if present.
string remove_option(string name);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct Phase_order_alphabetic {
    bool operator()(const Phase& a, const Phase& b) const
    {
        return a.get(Structure::name,-1)<b.get(Structure::name,-1);
    }
}; 
    
class List_phases : public set<Phase, Phase_order_alphabetic> {
public:    
    List_phases(const Phase& s)
    {        
        emplace(s);
    }
    
    List_phases(const string& s)
    {        
        emplace(s);
    }
         
    List_phases()
    {        
    }

    Phase get(const string ph);
    bool is(const Phase& ph) const;
    bool is(const string ph) const;
    
    void set_G_ref(Phase& p, Configuration& c, DP g_ref); 
    void set_G_ref(const string& p,const string& c, DP g_ref);  
    
    // handles input PARAMETER G(BCC,Al,Ni:Ni;0)...
    void set_parameter(const string& par);
    
    void set_Y(const string& p, const string el, const int sub=0, const DP yy=0); 
//    void set_Y(const Y& yy, const int sub=0) { y[sub].push_back(yy); }
    DP get_Y(const string& p, const string el, const int sub=0) const; 
    Y get_Y_full(const string& p, const string el, const int sub=0) const; 
//    void list_Y() const;
    
//    void add_vib(const string ph, DP T, DP& maxF);     // temporary solution for adding Fvib at a given temperature T 
    
    string calc(List_TPfunctions& list, string ph, DP T=298.15, DP P=1e5, DP nmol=1.0, vector<DP> y={});
        
    using set<Phase, Phase_order_alphabetic>::insert;
    void insert(const string ph) { emplace(ph); }
    string list() const;                                        // list elements symbols separated by comma
    void print_TDB(ostream& os=cout);                           // print phases with TDB format
           
};
#endif	/* PHASES_H */

