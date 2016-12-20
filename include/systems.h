/* 
 * File:   systems.h
 * Author: Mauro Palumbo
 *
 * This file implements the class Systems, which is basically a big container for
 * all kind of thermodynamic data in a system. It can contain elements, species,
 * phases, etc.
 * 
 * Created on January 24, 2015, 8:52 PM
 */

#ifndef SYSTEMS_H
#define	SYSTEMS_H

#include "elements.h"
#include "species.h"
#include "tpfunctions.h"
#include "structure.h"
#include "phases.h"
#include "references.h"
#include "types.h"
#include "harmonic.h"
#include "electronic.h"
#include "readabinitio.h"
#include "readtdb.h"

using namespace std;

class Systems {
public:
    Systems();
    
    void insert(const Element el) { le.insert(el); }
    void insert(const Species sp) { ls.insert(sp); }
    void insert(const TPfunction fun) { lf.insert(fun); }
    void insert(const Structure str) { lst.insert(str); }
    void insert(const Phase ph) { lp.insert(ph); }
    void insert(const Type typ) { lt.insert(typ); }
    void insert(const EDOS edos) { ledos.insert(edos); }
    void insert(const PDOS pdos) { lpdos.insert(pdos); }
    
    Phase get(const string ph) { return lp.get(ph); }
    
    DP calculate_TPfunction(string ss) { return lf.calc(ss); }   
    string calculate_phase(string ss) { return lp.calc(lf,ss); }
    void calculate_edos(ostream& os, string ss) { ledos.calc(os,ss); } 
    void calculate_pdos(ostream& os, string ss) { lpdos.calc(os,ss); }   
    
//    void add_vib(const string ph, DP T, DP& maxF) { lp.add_vib(ph,T,maxF); }

    void print_TDB(ostream& os=cout, bool el=true,bool sp=true,bool fun=true,
        bool typ=true,bool ph=true,bool ref=true,bool edos=true ,bool pdos=true);
    
    //    void print_TDB(ostream& os=cout) const;
    void print_structures(ostream& os=cout);
    
    bool read_tdb(string fin);   
    bool read_raw_abinitio(string fin);
    void print_raw_abinitio(ostream& os=cout);
    void print_abinitio_files(ostream& os=cout);
  
    void reset();
    
private:
    bool in_tdb=true;       // write extra data (pdos, edos, etc.) in the tdb file?
    
    List_elements le;
    List_species ls;
    List_TPfunctions lf;
    List_structures lst;
    List_phases lp;
    List_references lr;
    List_types lt;
    List_EDOS ledos;
    List_PDOS lpdos;
    
    /////////////////////////////////////////////////////////////////////////
    vector<string> abinitio_files;            // List of raw ab initio file names read
    vector<string> abinitio_files_lines;      // List of raw text lines in the read ab initio files (separated by an empty line)
    vector<ABLine> raw_abinitio;              // Raw data from ab initio
};


#endif	/* SYSTEMS_H */

