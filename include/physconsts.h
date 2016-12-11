/* 
 * File:   physconsts.h
 * Author: Mauro Palumbo
 *
 * C++ Library with useful physical constants
 * Created on January 22, 2015, 10:18 AM
 * 
 * To do:
 */



#ifndef PHYSCONSTS_H
#define	PHYSCONSTS_H

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//

// Definitions of useful physical constants (values from ThermoCalc)
const double R        { 8.31451        	 };       // molar gas constant, J mol-1 K-1

// Definitions of useful physical constants (values from Quantum Espresso)
//const double Bohr_radius { 0.52917720859E-10 }; // Bohr radius in m
//const double Hartree    { 4.35974394E-18 };	  // Hartree energy in J 

// Definitions of useful physical constants (values from NIST, http://physics.nist.gov/cuu/Constants/index.html)

const double NA       { 6.02214129e23    };       // Avogadro constant, mol-1
//const double R        { 8.3144621        };     // molar gas constant, J mol-1 K-1
const double h        { 6.62606957e-34   };       // Planck constant, J s 
const double hbar     { 1.054571726e-34  };       // Planck constant over 2*PI, J s 
const double kB       { 1.3806488e-23    };       // Boltzmann constant, J K-1
const double eV       { 1.602176565e-19  };       // electronvolt, J
const double c0       { 2.99792458e8     };       // speed of light in vacuum, m s-1
const double Vm       { 22.710953e-3     };       // molar volume of ideal gas, m^3 mol^(-1) (at 273.15 K, 100 kPa)
const double m_u      { 1.660538921e-27  };       // atomic mass constant, kg 

const double Rydberg_to_J       { 2.1798720294e-18 };       // 1 Rydbgerg in J
const double eV_to_Rydberg      { eV / Rydberg_to_J };      // 1 Rydbgerg in J
const double Bohr_radius        { 0.52917721092e-10 };      // Bohr radius in m
const double Hartree            { 4.35974434e-18 };         // Hartree energy in J 
const double inversem_to_J      { c0*h };                   // m-1 to J
const double inversem_to_eV     { inversem_to_J/eV };       // m-1 to eV
const double kB_to_eV           { kB/eV        };           // Boltzmann constant in eV/K
const double R_to_eVatK         { R*eV/NA      };           // molar gas constant in eV at-1 K-1



const double au_to_GPa  { Hartree / Bohr_radius / Bohr_radius / Bohr_radius / 1.0E+9 }; 
const double Rydberg_to_kbar { 10.0*au_to_GPa/2.0 };
const double eV_to_J       { eV  };       
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif   // PHYSCONSTS_H
