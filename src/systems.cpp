/* 
 * File:   systems.h
 * Author: Mauro Palumbo
 *
 * Created on January 24, 2015, 8:52 PM
 */

#include "../include/systems.h"


Systems::Systems()
{
    // by default the "electron" and "vacuum" are always present
    le.insert(*new Element("/-"));
    le.insert(*new Element("VA"));    
    // read the structures from default "structures_database" file
    if (!lst.read("structures_database")) 
        cout<<"Could not open default \"structures_database\" file. No structure loaded.";
    else
        lst.print();
}

void Systems::print_TDB(ostream& os, bool el,bool sp,bool fun,bool typ,bool ph,bool ref,bool edos, bool pdos)
{
    if (el) { 
        le.print_TDB(os); 
        os<<endl;
    }
    if (sp) {
        ls.print_TDB(os); 
        os<<endl;
    }
    if (fun) {
        lf.print_TDB(os); 
        os<<endl;
    }
    if (typ) {
        lt.print_TDB(os); 
        os<<endl;
    }
    if (ph) {
        lp.print_TDB(os);  
        os<<endl;
    }
    if (ref) {
        lr.print_TDB(os);  
        os<<endl;
    }
    if (edos) {
        ledos.print_TDB(os,in_tdb);  
        os<<endl;
    }
    if (pdos) {
        lpdos.print_TDB(os,in_tdb);  
        os<<endl;
    }
}


void Systems::print_structures(ostream& os)
{
    lst.print();
    os<<endl;
}

bool Systems::read_tdb(string fin)
{
    string refs;
    if (!read_tdb_file(le, ls, lf, lp, refs, lt, fin))
        return false;
    if (refs.size()>0) lr.insert(refs);
    return true;
}

bool Systems::read_raw_abinitio (string fin)
{
    List_elements abinitio_elements;
    if (!read_abinitio_file(abinitio_elements,raw_abinitio, lst, fin, abinitio_files_lines))
        return false;
    
    for (auto x : abinitio_elements) 
        if (!le.is(x.symbol())) { 
            cerr<<"Element "<<x.symbol()<<" from ab initio file not defined. Before read ab initio data, you have to "
                    "define the necessary elements with the DEFINE ELEMENT command. Aborting..."<<endl;
            return false;
        }

    List_elements onlyrealelements;   // create a list of elements without VA and /-
    for (auto x : le) 
        if (x.symbol()!="VA"&&x.symbol()!="/-") onlyrealelements.insert(x);
    
    lf.insert("UN_ASS");
    for (int i=0; i<onlyrealelements.size(); i++) 
       lf.insert("GHSER"+onlyrealelements[i].symbol());
            
    abinitio_files.push_back(fin);   // put the file name into the list of files read
    for (int i=0; i<raw_abinitio.size(); i++)
    {
       if (!lp.is(to_up(raw_abinitio[i].ph))) lp.insert(Phase(to_up(raw_abinitio[i].ph), lst, onlyrealelements));
       lp.set_parameter("PARAMETER G("+to_up(raw_abinitio[i].ph)+","+to_up(raw_abinitio[i].conf)+";0)  0  "+
       to_string(raw_abinitio[i].entJ*lp.get(to_up(raw_abinitio[i].ph)).nmol())+";  6.00000E+03   N REF0 !");
    }
    return true;
}
    

void Systems::print_raw_abinitio (ostream& os)
{         
    os<<"Raw ab initio file(s) read:\n";
    for (int i=0; i<abinitio_files.size(); i++)
        os<<abinitio_files[i]<<endl;
    
    os<<"Original files read:\n";
    for (int i=0; i<abinitio_files_lines.size(); i++)
        os<<abinitio_files_lines[i];      
    
    os<<"Energies read:\n";
    for (int i=0; i<raw_abinitio.size(); i++)
        os<<raw_abinitio[i];      
}

void Systems::reset()
{
    le.clear();
    ls.clear();
    lf.clear();
    lp.clear();
    lr.clear();
    lt.clear();
    
    ledos.clear();
    lpdos.clear();
    
    abinitio_files.clear();            
    abinitio_files_lines.clear();      
    raw_abinitio.clear();              
    
    le.insert(*new Element("/-"));
    le.insert(*new Element("VA"));    
}