/* 
 * File:   electronic.h
 * Author: Mauro Palumbo
 *
 * This file defines the classes and methods to compute thermodynamic properties 
 * from an electronic DOS using textbooks statistical equations. 
 * 
 * Input is an object electronic DOS, containing the DOS as calculated for example
 *  by Quantum Espresso. Input is also the temperature T from the object EDOS.
 * 
 * Output is the object thermo of the class EDOS, containing the 
 * calculated thermodynamic functions. The temperature in EDOS is of course
 * not changed.
 *
 * In practice, the integral is calculated in the interval where the DOS
 * is calculated from Quantum Espresso (or any other ab initio code).
 * 
 * Created on 10 May 2015, 19:10
 */

#ifndef ELECTRONIC_H
#define	ELECTRONIC_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <set>
#include "physconsts.h"
#include "math_utils.h"
#include "utils.h"

class EDOS {
public:
    EDOS(string name, string e_unit="eV", int nat=1, int nel=1, bool mag=false, DP ef=0.0, string fname="");    
    EDOS(vector<DP>& tempe, vector<DP>& tempdos, vector<DP>& tempdosdown, string name, int nat=1);
      
    string name() const { return n; }
    int n_atoms() const { return natoms; }
    size_t size() const { return e.size(); }
    DP E_F() const { return EF; }
    DP E(size_t i) const { return e[i]; }
    DP DOS(size_t i) const { return dos[i]; }
    DP DOSDOWN(size_t i) const { return dosdown[i]; }
    bool magnetic() const { return magn; }
    
    // This is a simple routine to integrate  dos with the option moment
    // If moment=0, then the pure dos integral is computed, else the corresponding moment of the dos is calculated
    // The integral is calculated by the trapezoid formula used for each step defined by the dos values.
    DP integral(const DP moment=0.0) const;
    
    DP electrons() const;
 
    // Functions to get electronic thermodynamic properties from the DOS. Units are SI
      DP E(DP T);     // in J/mol
      DP S(DP T);     // in J/mol/K
      DP C(DP T);     // in J/mol/K
      DP F(DP T) { return E(T)-E(0.0)-T*S(T); }      // in J/mol
    
      DP E_Sommerfeld(DP T);     // in J/mol
      DP S_Sommerfeld(DP T);     // in J/mol/K
      DP C_Sommerfeld(DP T);     // in J/mol/K
      DP F_Sommerfeld(DP T) { return E_Sommerfeld(T)-T*S_Sommerfeld(T); }    // in J/mol    
   
    bool read_dos_file(string fname, string e_unit="eV");
    void calc(ostream& os=cout, DP Tin=0.0, DP Tfin=1.0, DP Tstep=1.0, int model=1);
    
    string to_string() const;
    void clear();
    
//    void print_TDB(ostream& os=cout, bool in_tdb=true);
    void print_TDB(ostream& os=cout, bool in_tdb=true) const;

protected:
    // read dos data from a string stream, used in the constructors and in read_dos_file
    bool read_dos(stringstream& ss, string e_unit="cm-1");

    // get the value(s) of the dos at the Fermi energy and store it in dos_ef.
    // If magnetic, do the same for dosdown and store it in dosdown_ef. 
    void get_dos_ef();
    
private:    
    string n;                 // EDOS identifier, a string to identify it
    DP EF {0.0};              // Fermi energy in input (eV)
    DP E0 {0.0};              // Ground state electronic energy (eV)
    DP dos_ef {0.0};          // density of states at the Fermi energy
    DP dosdown_ef {0.0};      // density of states for spin down at the Fermi energy
    int natoms {1};           // number of atoms in the unit cell of the input data
    int nelectrons {1};       // number of conduction electrons as in your dos calculations
    bool magn {false};        // magnetic? If true, there are two dos, for spin up and spin down

    // dos and e contains the whole data set
    vector<DP> dos;         // density of states (number of states per eV/cell). If magnetic, it is for spin up
    vector<DP> dosdown;     // If magnetic, density of states for spin down. Otherwise empty vector.
    vector<DP> e;           // energy in eV/cell
};

ostream& operator<<(ostream& os, const EDOS& edos);


/* A simple function to calculate the Fermi-Dirac distribution function
 * E  -> energy (eV)
 * T  -> temperature (K)
 * mu -> "chemical potential" (eV)
*/
inline DP f_fermi(const DP E, const DP T, const DP mu)
{
    return 1.0/(exp((E-mu)/(kB_to_eV*T))+1.0);
}


/* A simple function to calculate the derivative of the Fermi-Dirac distribution function with respect to energy
 * E  -> energy (eV)
 * T  -> temperature (K)
 * mu -> "chemical potential" (eV)
*/
inline DP df_fermi_dE(const DP E, const DP T, const DP mu)
{
    return -1.0/(kB_to_eV*T)*f_fermi(E,T,mu)*(1-f_fermi(E,T,mu));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


struct EDOS_order_alphabetic {
    bool operator()(const EDOS& a, const EDOS& b) const
    {
        return a.name()<b.name();
    }
}; 

class List_EDOS : public set<EDOS,EDOS_order_alphabetic> {
public:
    List_EDOS()
    {        
    }

    EDOS get(const string n) const;    
    
    bool is(const EDOS& edos) const;
    bool is(const string name) const;
    
//    using set<EDOS, EDOS_order_alphabetic>::insert;
    void insert(const EDOS& edos) 
      { emplace(edos); }
    void insert(string name, string e_unit="eV", int nat=1, int nel=1, bool mag=false, DP ef=0.0, string fname="") 
      { emplace(name,e_unit,nat,nel,mag,ef,fname); }
    void insert(vector<DP>& tempe, vector<DP>& tempdos, vector<DP>& tempdosdown, string name, int nat=1) 
      { emplace(tempe,tempdos,tempdosdown,name,nat); } 

    string list() const;     // get a list of EDOS names separated by comma
    
    void calc(ostream& os=cout, string name="", DP Tin=0.0, DP Tfin=1.0, DP Tstep=1.0);
    
    void print_TDB(ostream& os=cout, bool in_tdb=true);     // print PDOS with TDB format
    
//    bool read_dos_file(string name, string fname, string e_unit="cm-1", int nat=1);
    
};


#endif	/* ELECTRONIC_H */

