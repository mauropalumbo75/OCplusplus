/* 
 * File:   qha.h
 * Author: Mauro Palumbo
 *
 * Created on 05 May 2015, 13:29
 */

#ifndef QHA_H
#define	QHA_H

#include "../ev/ev.h"
#include "../harmonic/harmonic.h"
#include "../commoninclude/math_utils.h"

/*
 Define a class for thermophysical properties
 */
class Thermophys {
public:
    Thermophys(DP t=0, DP p=0, DP v=0, DP aalat=0, DP aa=0, DP b_t=0, DP b_t_prime=0, DP b_s=0, DP helmoltz_harm=0, DP helmoltz_qha=0, 
            DP gibbs=0, DP eentropy_harm=0, DP eentropy_qha=0, DP eenergy=0, DP eenthalpy=0, DP cv=0, DP cp=0,  DP cp2=0, DP bbeta=0, DP gruneisen=0)
        :T(t), P(p), V(v), B0(b_t), B0p(b_t_prime), Fharm(helmoltz_harm), F(helmoltz_qha),
                G(gibbs), Sharm(eentropy_harm), S(eentropy_qha), E(eenergy), H(eenthalpy), Charm(cv), C(cp),
                Cfrom_eq(cp2), beta(bbeta), Gruneisen(gruneisen)
    {
    }
        
    DP r(const string var);
    void w(const string var, const DP x);


private:
    DP T;               // temperature (K)
    DP P;               // pressure (Pa)
    DP V;               // volume (m^3)
    DP B0;              // isothermal bulk modulus (Pa)
    DP B0p;             // first derivative of B0
    DP Fharm;           // Helmoltz energy (J/mol)
    DP F;               // Helmoltz energy (J/mol)
    DP G;               // Gibbs energy (J/mol)
    DP Sharm;           // entropy (J/mol/K)
    DP S;               // entropy (J/mol/K)
    DP E;               // internal energy (J/mol)
    DP H;               // enthalpy (J/mol)
    DP Charm;           // isocoric heat capacity (J/mol/K)
    DP C;               // isobaric heat capacity (J/mol/K)
    DP Cfrom_eq;       // isobaric heat capacity (J/cell/K) from eq. Cp-Cv=B_T*V*beta^2*T
    DP beta;            // thermal expansion coeff. (1/K)
    DP Gruneisen;       // thermodynamic Gruneisen parameter gamma=V*beta*B_S/Cp ()
    DP chi;             // chi^2 from EOS fit
    DP temp;            // temp variable for data manipulation 
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class QHA : public List_EV  { 
public:
     
    bool is(const EV& sp) const;
    bool is(const string s) const; 
         
    string listT() { string temp; for (auto x:T) temp+=x+" "; return temp; } 
    
    void calculate(const DP P);
    bool calculated() { return calc; }
    bool calc_harmonic(string ident="", DP Tin=0.0, DP Tfin=1.0, DP Tstep=1.0);
    
//    DP E_TP(const string T, const DP P) { if (qha) return get(id+"FvsV."+T+".in").E_P(P);  throw; }
//    DP F(const DP T, const DP P) { if (fitted) return Pfit(V/sqr3(Bohr_radius)/NA) *Rydberg_to_kbar*1.E8; }
//    DP H(const DP T, const DP P) { if (fitted) return Hfit(V/sqr3(Bohr_radius)/NA)*Rydberg_to_J*NA;  throw; }
//    DP V(const DP T, const DP P) { if (fitted) return p[1] * pow( 1.0+P/Rydberg_to_kbar/1.E8*p[3]/p[2], -1.0/p[3] ) *sqr3(Bohr_radius)*NA; }

    bool qha_from_dos(vector<string> vT, string ident="", string lattice="V", string v_unit="au", string e_unit="cm-1", int nat=1);  
    bool read_alat_files(string ident="", string fname="alat_values");    
    bool read_pdos_files(string ident="", string fname="phdos", string e_unit="cm-1", int nat=1);  
    bool read_EV(string ident="", string f="EvsV.in", string lattice="V", string v_unit="au",  string e_unit="Rydberg", string norm="at");
    bool read_qha_files(vector<string> vT, string ident="", string lattice="V", string v_unit="au",  string e_unit="Rydberg", string norm="at");
    bool write_qha_results(vector<string> outfiles); 
    
protected:
    bool deriv(string f, string x, string dfdx, int NP=0, int NL=0, int NR=0, int LD=0, int M=0);
    
private:
    string id;              // an identifier for the qha data set (for example "Al")
    bool calc=false;
    vector<string> T;      // vector of temperatures for the qha, stored as strings for convenience
    vector<string> alat;   // vector of lattice(volume) for the qha, stored as strings for convenience

    List_PDOS pdos;         // if the phonon dos are provided, store them
    Thermophys E;
    vector<Thermophys> F;
};

#endif	/* QHA_H */

