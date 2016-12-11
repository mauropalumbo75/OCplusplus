/* 
 * File:   harmonic.h
 * Author: Mauro Palumbo
 *
 * Created on 03 May 2015, 18:13
 * 
 * This file defines the classes and methods to compute thermodynamic properties 
 * from a phonon DOS using textbooks statistical equations. 
 * 
 * Input is an object phonon DOS, containing the DOS as calculated for example
 *  by matdyn in Quantum Espresso. Input is also the temperature T.
 * 
 * Output is the object thermo of the class PDOS, containing the 
 * calculated thermodynamic functions. The temperature in thermo is of course
 * not changed.
 *
 * In practice, the integral is calculated in the interval where the DOS
 * is calculated from Quantum Espresso (or any other ab initio code).
 *
*/


#ifndef HARMONIC_H
#define	HARMONIC_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <set>
#include "../commoninclude/physconsts.h"
#include "../commoninclude/math_utils.h"
#include "../commoninclude/utils.h"

class PDOS {
public:
    PDOS(string name, string e_unit="cm-1", int nat=1, string fname="");    
    PDOS(vector<DP>& tempe, vector<DP>& tempdos, string name, int nat=1);
  
    
    string name() const { return n; }
    int n_atoms() const { return natoms; }
    size_t size() const { return e.size(); }
    DP E(size_t i) const { return e[i]; }
    DP DOS(size_t i) const { return dos[i]; }
    
    // This is a simple routine to integrate  dos with the option moment
    // If moment=0, then the pure dos integral is computed, else the corresponding moment of the dos is calculated
    // The integral is calculated by the trapezoid formula used for each step defined by the dos values.
    DP integral(const DP moment=0.0);
 
    // Functions to get harmonic thermodynamic properties from the DOS. Units are SI
    DP ZPE() { return 0.5*integral(1.0)*eV_to_J*NA; }    // in J/mol
    DP E(DP T);     // in J/mol
    DP S(DP T);     // in J/mol/K
    DP C(DP T);     // in J/mol/K
    DP F(DP T) { return E(T)-T*S(T); }      // // in J/mol
    
    DP F_Ryd(DP T) { return F(T)/Rydberg_to_J/NA; }      // in Rydberg/at, for EOS fitting
    
    bool read_dos_file(string fname, string e_unit="cm-1", int nat=1);
    void calc(ostream& os=cout, DP Tin=0.0, DP Tfin=1.0, DP Tstep=1.0);
    
    string to_string() const;
    void clear();
    
//    void print_TDB(ostream& os=cout, bool in_tdb=true);
    void print_TDB(ostream& os=cout, bool in_tdb=true) const;

protected:
    // read dos data from a string stream, used in the constructors and in read_dos_file
    bool read_dos(stringstream& ss, string e_unit="cm-1");
    
private:    
    string n;               // PDOS identifier, a string to identify it
    int natoms {1};           // number of atoms in the unit cell of the input data

    // dos and e contain the whole data set
    vector<DP> dos;     // density of states (number of states per eV/cell)
    vector<DP> e;       // energy in eV/cell
};

ostream& operator<<(ostream& os, const PDOS& pdos);


/* Calculate the Bose-Einstein distribution function
 * E  -> energy (eV/at)
 * T  -> temperature (K)
*/
inline DP f_bose(const DP E, const DP T) { return 1.0/(exp(E/(kB_to_eV*T))-1.0); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


struct PDOS_order_alphabetic {
    bool operator()(const PDOS& a, const PDOS& b) const
    {
        return a.name()<b.name();
    }
}; 

class List_PDOS : public set<PDOS,PDOS_order_alphabetic> {
public:
    List_PDOS()
    {        
    }

    PDOS get(const string n) const;    
    
    bool is(const PDOS& pdos) const;
    bool is(const string name) const;
    
//    using set<PDOS, PDOS_order_alphabetic>::insert;
    void insert(const PDOS& pdos) 
      { emplace(pdos); }
    void insert(string name, string e_unit="cm-1", int nat=1, string fname="") 
      { emplace(name,e_unit,nat,fname); }
    void insert(vector<DP> tempe, vector<DP> tempdos, string name="", int nat=1) 
      { emplace(tempe,tempdos,name,nat); } 

    string list() const;     // get a list of PDOS names separated by comma
    
    void calc(ostream& os=cout, string name="", DP Tin=0.0, DP Tfin=1.0, DP Tstep=1.0);
    
    void print_TDB(ostream& os=cout, bool in_tdb=true);     // print PDOS with TDB format
    
//    bool read_dos_file(string name, string fname, string e_unit="cm-1", int nat=1);
    
};

#endif	/* HARMONIC_H */

