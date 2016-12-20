/* 
 * File:   readabinitio.h
 * Author: Mauro Palumbo
 *
 * This file contains classes and methods for reading the DFT files with enthalpies
 * of formation. The format is an internal one used in ICAMS (www.icams.de). An example of such 
 * file is given at the end of this file as a comment. There is not (yet) much
 * documentation related to this format.
 * 
 * Created on June 15, 2014, 8:52 PM
 */

#ifndef READABINITIO_H
#define	READABINITIO_H

#include<iostream>
#include<string>
#include<vector>
#include "utils.h"
#include "math_utils.h"
#include "structure.h"
#include "elements.h"


struct Composition {
    string t;
    DP c;
};

istream& operator>>(istream& is, Composition& r);
ostream& operator<<(ostream& os, Composition& r);


// Define a struct just for reading a line of the file with ab initio enthalpies
struct ABLine {
    DP Etot,enteV, entJ;
    string conf;
    string ph;
    ABLine()
       :Etot(0),enteV(0), entJ(0) {}
}; 

istream& operator>>(istream& is, ABLine& l);
ostream& operator<<(ostream& os, ABLine& l);


// Define another struct for reading the line(s) defining what A, B,... are 
struct ABLine_reference {
    string command;
    char letter;
    string element;
    
    ABLine_reference() {}
}; 

istream& operator>>(istream& is, ABLine_reference& l);
ostream& operator<<(ostream& os, ABLine_reference& l);

struct multiplicities {
    string phase;
    vector<DP> m;
    vector<int> flip;
};

ostream& operator<<(ostream& os, multiplicities& l);

// General functions
bool is_in_flip(int k,vector<int> flip);
string convert_special(string conf);  
bool read_abinitio_file(List_elements& le,vector<ABLine>& v, List_structures& structures, string& fin, vector<string>& lines);


#endif	/* READABINITIO_H */

/*
 ++++++++++++++++++++++++++++++++++++
 reference A: Al fcc.FM -3.745590 
++++++++++++++++++++++++++++++++++++
# Calculation for Al with  spin-relaxation
# XC-functional   =  PBE-PAW 
# cut-off energy  =  400 
# k-point density =  0.020 
# date : Wed May 28 16:40:08 CEST 2014 
A15.FM Al
# Calculation for Al with  spin-relaxation
# Multiplicities = 6 2 
# Missing A15.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.665370  eV/at dH=  	    80.220000 meV/at dH= 	  7740.050766 J/mol ch: F 		A15-AA.FM 	 A15.FM 
# there are 4 calculations for the A15.FM-phase 
Al12W.FM Al
# Calculation for Al with  spin-relaxation
# Multiplicities = 2 24
# Missing Al12W.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.684632  eV/at dH=  	    60.958000 meV/at dH= 	  5881.550917 J/mol ch: F 		Al12W-AA.FM 	 Al12W.FM 
# there are 4 calculations for the Al12W.FM-phase 
Al4W.FM Al W_pv
# Calculation for Al-W_pv with  spin-relaxation
# Multiplicities = 6 24
# Missing Al4W.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.666857  eV/at dH=  	    78.733000 meV/at dH= 	  7596.577125 J/mol ch: F 		Al4W-AA.FM 	 Al4W.FM 
# there are 4 calculations for the Al4W.FM-phase 
Al5W.FM Al
# Calculation for Al with  spin-relaxation
# Multiplicities = 2 2 2 6
# Missing Al5W.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.732498  eV/at dH=  	    13.092000 meV/at dH= 	  1263.185548 J/mol ch: F 		Al5W-AAAA.FM 	 Al5W.FM 
# there are 16 calculations for the Al5W.FM-phase 
bcc.FM Al
# Calculation for Al with  spin-relaxation
# Missing bcc.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.649668  eV/at dH=  	    95.922000 meV/at dH= 	  9255.062947 J/mol ch: F 		bcc-A2.FM 	 bcc.FM 
# there are 18 calculations for the bcc.FM-phase 
C14.FM Al 
# Calculation for Al with  spin-relaxation
# Multiplicities = 4 2 6 
# Missing C14.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.597917  eV/at dH=  	   147.673000 meV/at dH= 	 14248.273707 J/mol ch: F 		C14-AAA.FM 	 C14.FM 
# there are 8 calculations for the C14.FM-phase 
C15.FM Al 
# Calculation for Al with  spin-relaxation
# Multiplicities = 4 2 
# Missing C15.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.583671  eV/at dH=  	   161.919000 meV/at dH= 	 15622.803291 J/mol ch: F 		C15-AA.FM 	 C15.FM 
# there are 4 calculations for the C15.FM-phase 
C36.FM Al
# Calculation for Al with  spin-relaxation
# Multiplicities = 4 4 4 6 6 
# Missing C36.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.590548  eV/at dH=  	   155.042000 meV/at dH= 	 14959.273883 J/mol ch: F 		C36-AAAAA.FM 	 C36.FM 
# there are 32 calculations for the C36.FM-phase 
chi.FM Al 
# Calculation for Al with  spin-relaxation
# Multiplicities = 1 4 12 12
# Missing chi.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.692344  eV/at dH=  	    53.246000 meV/at dH= 	  5137.456284 J/mol ch: F 		chi-AAAA.FM 	 chi.FM 
# there are 16 calculations for the chi.FM-phase 
fcc-cubic.FM Al
# Calculation for Al with  spin-relaxation
fcc.FM Al 
# Calculation for Al with  spin-relaxation
# Missing fcc.FM structures: CH40-AB.FM L60-AB3.FM 
x(A)=    1.000000  0 0 Etot= 	    -3.745590  eV/at dH=  	     0.000000 meV/at dH= 	     0.000000 J/mol ch: F 		fcc-A1.FM 	 fcc.FM 
# there are 19 calculations for the fcc.FM-phase 
hcp.FM Al
# Calculation for Al with  spin-relaxation
# Missing hcp.FM structures: B_h-AB.FM B_h-BA.FM B35-BA.FM 
x(A)=    1.000000  0 0 Etot= 	    -3.712554  eV/at dH=  	    33.036000 meV/at dH= 	  3187.488371 J/mol ch: F 		hcp-A3.FM 	 hcp.FM 
# there are 14 calculations for the hcp.FM-phase 
mu.FM Al 
# Calculation for Al with  spin-relaxation
#  Multiplicities  = 1 6 2 2 2 
# Missing mu.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.642269  eV/at dH=  	   103.321000 meV/at dH= 	  9968.957681 J/mol ch: F 		mu-AAAAA.FM 	 mu.FM 
# there are 32 calculations for the mu.FM-phase 
sigma.FM Al 
# Calculation for Al with  spin-relaxation
# Multiplicities = 2 4 8 8 8
# Missing sigma.FM structures: 
x(A)=    1.000000  0 0 Etot= 	    -3.679949  eV/at dH=  	    65.641000 meV/at dH= 	  6333.391577 J/mol ch: F 		sigma-AAAAA.FM 	 sigma.FM 
# there are 32 calculations for the sigma.FM-phase 
 */
