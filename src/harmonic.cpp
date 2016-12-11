/* 
 * File:   harmonic.h
 * Author: mauro
 *
 * Created on 03 May 2015, 18:13
 */

#include "harmonic.h"


PDOS::PDOS(string name, string e_unit, int nat, string fname)
{       
    // set all characters to upright
    name=to_up(name);
    
    // start processing the input string (can be just the PDOS name or the full definition)
    stringstream ss(name);
    string command,more;  
    ss>>command;
    command=get_subcommand_extended(command,"PDOS");
    if (command=="PDOS") {      // assume only the string ss with the full definition is provided
        ss>>name>>e_unit>>nat>>more;   // read full format
        n=name;
        natoms=nat;        
        if (more==">") {     // read next lines containing DOS data till !
            if (!read_dos(ss,e_unit)) throw;
        }       
        else if (more=="") throw;       // if next string is not ">" it must be a file name
        else if ( !read_dos_file(more,e_unit,nat) ) throw;   
    }  
    else {
        n=name;
        natoms=nat;
        if (fname!="")
            if ( !read_dos_file(fname,e_unit,nat) ) throw;   
    }
}   


PDOS::PDOS(vector<DP>& tempe, vector<DP>& tempdos, string name, int nat)
        : n(name) 
{
    if (tempe.size()!=tempdos.size()) throw;
    if (tempe.size()==0) throw;  
    if (nat<1) throw;  
 
    for (int i=0; i<tempe.size(); i++) {    
        if (tempdos[i]<0.0) throw;      // dos cannot be negative
        dos.push_back(tempdos[i]);      // store input values
        e.push_back(tempe[i]);
    }
    natoms=nat;
} 


bool PDOS::read_dos(stringstream& ss, string e_unit)
{
    DP tempe, tempdos;
    try {
        while (ss>>tempe>>tempdos) {     // read E and dos 
            // convert them to SI units
            if (tempe < 1E-5) tempe=1E-5;    // E=0 or negative causes problems, set it to a low value
            tempe=convertE(tempe,e_unit);
            tempdos=convertdos(tempdos,e_unit);
            e.push_back(tempe);     // store them
            dos.push_back(tempdos);
        }
    }
    catch (...) { return false; }
    return true;                
}

/* This function read the input file... more comments to be added */
bool PDOS::read_dos_file(string fname, string e_unit, int nat)
{
    // Open input file
    ifstream ist(fname.c_str());        //  ist read the file fin
    if (!ist) {
        cerr<<"Input file cannot be opened"<<endl;
        return false;
    } 

    ist.clear();  
    
    string line;  // line is a buffer string, initially empty, to read input file line by line
    // Start reading input stream
    while(getline(ist,line))
    {
        stringstream ss(line);
        if (!read_dos(ss,e_unit)) return false;   
    }    
    
    natoms=nat;
    return true;        
}

string PDOS::to_string() const
{
    stringstream buf;
    buf.scientific; buf.precision(10);
    for (int i=0; i<e.size(); i++)
        buf<<e[i]<<"\t\t"<<dos[i]<<"\n";
    return buf.str();
}    

void PDOS::clear()
{
    e.clear();
    dos.clear();
}   


void PDOS::print_TDB(ostream& os, bool in_tdb) const
{
    os<<"PDOS "<<n<<" eV 1 ";
    if (in_tdb) {           // write the DOS inside the tdb file
        os<<"> "<<endl;
        os.scientific; os.precision(10);
        for (int i=0; i<e.size(); i++)
            os<<e[i]<<"\t\t"<<dos[i]<<"\n";
        os<<"!"<<endl;
    } else {                // write the DOS in a different file whose name is DOSNAME.PDOS
        string fname=n+".PDOS";
        os<<fname<<" !"<<endl;
        
        // Open input file
        ofstream ost(fname.c_str());        //  ist read the file fin
        if (!ost) {
            cerr<<"Output file "<<fname<<" cannot be opened!"<<endl;
            throw;
        } 

        ost.clear();
        ost.scientific; ost.precision(10);
        for (int i=0; i<e.size(); i++)
            ost<<e[i]<<"\t\t"<<dos[i]<<"\n";        
    }
}

void PDOS::calc(ostream& os, DP Tin, DP Tfin, DP Tstep)
{    
    if (Tin<0 || Tfin<0) throw;
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    os<<"X  Harmonic results from phonon DOS"<<endl;
    os<<"X  N. of atoms: "<<natoms<<endl;
    os<<"X  Raw DOS integral= "<<integral(0.0)/natoms<<
            "\t(it must be approximately equal to 3, phonon modes per atom)"<<endl;
    os<<"X  ZPE= "<<ZPE()<<" J/mol"<<endl;
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    os<<"X\tT\t\t\tEvib\t\t\tFvib\t\t\tSvib\t\t\tCvib"<<endl;
    os<<"X\tK\t\t\tJ/mol\t\t\tJmol\t\t\tJ/mol/K\t\t\tJ/mol/K"<<endl;
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    os.precision(20);
    os<<scientific;    
    for (DP T=Tin; T<=Tfin; T+=Tstep)
        os<<T<<"\t"<<E(T)<<"\t"<<F(T)<<"\t"<<S(T)<<"\t"<<C(T)<<endl;
//        os<<T<<"\t"<<E(T)/Rydberg_to_J/NA<<"\t"<<F(T)/Rydberg_to_J/NA<<
//                "\t"<<S(T)/R<<"\t"<<C(T)/R<<endl;
}

// All the routines below are based on the Simpson 3/8 rule of integration
DP PDOS::integral(const DP moment)
{
    if (dos.size()<3) return 0;   // not enough points
    DP sum=0.0; 
    DP h=0.5*(e[2]-e[0]);
    for (int j=0; j<(dos.size()-3); j+=3) 
        sum += 3.0*pow(e[j],moment)*dos[j]
                +3.0*pow(e[j+1],moment)*dos[j+1]
                +2.0*pow(e[j+2],moment)*dos[j+2];
    return h*sum*3.0/8.0;
}

    
DP PDOS::E(DP T)
{
    if (dos.size()<3) return 0;   // not enough points
    if (T<0.1) return 0.5*integral(1.0)*eV_to_J*NA;
    DP sum=0.0; 
    DP h=0.5*(e[2]-e[0]);         // evaluate the integration step
    for (int j=0; j<(dos.size()-3); j+=3) 
        sum += 3.0*e[j]*coth( e[j]/(2.0*kB_to_eV*T) )*dos[j]
                +3.0*e[j+1]*coth( e[j+1]/(2.0*kB_to_eV*T) )*dos[j+1]
                +2.0*e[j+2]*coth( e[j+2]/(2.0*kB_to_eV*T) )*dos[j+2];
    
    return h*0.5*sum*3.0/8.0*eV_to_J*NA;   // h is the integration step, 0.5 comes from the equation for E,
                                // the factor 3.0/8.0 comes from the Simpson 3/8 rule
}

DP PDOS::S(DP T)
{
    if (dos.size()<3) return 0;   // not enough points
    if (T<0.5) return 0.0;
    DP sum=0.0; 
    DP h=0.5*(e[2]-e[0]);         // evaluate the integration step
    for (int j=0; j<(dos.size()-3); j+=3) 
        sum += 3.0*(e[j]/(2.0*kB_to_eV*T)*coth(e[j]/(2.0*kB_to_eV*T))-log(2.0*sinh(e[j]/(2.0*kB_to_eV*T))))*dos[j]
                +3.0*(e[j+1]/(2.0*kB_to_eV*T)*coth(e[j+1]/(2.0*kB_to_eV*T))-log(2.0*sinh(e[j+1]/(2.0*kB_to_eV*T))))*dos[j+1]
                +2.0*(e[j+2]/(2.0*kB_to_eV*T)*coth(e[j+2]/(2.0*kB_to_eV*T))-log(2.0*sinh(e[j+2]/(2.0*kB_to_eV*T))))*dos[j+2];
    
    return h*kB_to_eV*sum*3.0/8.0*eV_to_J*NA;  // h is the integration step, 0.5 comes from the equation for E,
                                    // the factor 3.0/8.0 comes from the Simpson 3/8 rule
}

DP PDOS::C(DP T)
{
    if (dos.size()<3) throw;   // not enough points
    if (T<0.5) return 0.0;
    DP sum=0.0; 
    DP h=0.5*(e[2]-e[0]);         // evaluate the integration step         
    for (int j=0; j<(dos.size()-3); j+=3) 
        sum +=  3.0*sqr(e[j]/(kB_to_eV*T))/( 4.0*sqr(sinh(e[j]/(2.0*kB_to_eV*T))) )*dos[j]
                +3.0*sqr(e[j+1]/(kB_to_eV*T))/( 4.0*sqr(sinh(e[j+1]/(2.0*kB_to_eV*T))) )*dos[j+1]
                +2.0*sqr(e[j+2]/(kB_to_eV*T))/( 4.0*sqr(sinh(e[j+2]/(2.0*kB_to_eV*T))) )*dos[j+2];
    
    return h*kB_to_eV*sum*3.0/8.0*eV_to_J*NA;  // h is the integration step, 0.5 comes from the equation for E,
                                    // the factor 3.0/8.0 comes from the Simpson 3/8 rule
}

ostream& operator<<(ostream& os, const PDOS& pdos)
{
    os<<"$---------------------------------------------------------------------"
            <<"--------------------------------------------"<<endl;
    os<<"$ Phonon DOS name: "<<pdos.name()<<"\tN. of atoms: "<<pdos.n_atoms()<<endl;
    if (pdos.size()!=0) { 
        os<<"$ DOS data:"<<endl;
        os.scientific; os.precision(10);
        for (int i=0; i<pdos.size(); i++)
            os<<pdos.E(i)<<"\t\t"<<pdos.DOS(i)<<"\n";
    } else
        os<<"$ No DOS data!"<<endl;
    os<<"$---------------------------------------------------------------------"
            <<"--------------------------------------------"<<endl;
}

/* All the routines below are based on the simplest trapezoidal rule of integration
 * Since the Simpson 3/8 rule is better, I am not using them but keep for 
 * debugging and future reference...
 * Besides there are two versions of the harmonic equations, one using the Bose-Einstein
 * distribution and the other using hyperbolic functions. They are equivalent and 
 * tested.
 * 
/* This is a simple routine to integrate a the dos with the option moment
 * If moment=1, then the pure dos integral is computed
 * If moment!=1, then the corresponding moment of the dos is calculated
 * 
DP PDOS::int_trap_points(const DP moment)
{
    DP sum=0.0;      
    for (int j=1; j<dos.size(); j++) 
        sum += 0.5*(e[j]-e[j-1])*( pow(e[j],moment)*dos[j]+pow(e[j-1],moment)*dos[j-1] );
    return sum;
}

DP PDOS::E(const DP T)
{
    DP sum=0.0;      
    for (int j=1; j<dos.size(); j++) 
//        sum += 0.5*(e[j]-e[j-1])*( e[j]*(f_bose(e[j],T)+0.5)*dos[j]+e[j-1]*(f_bose(e[j-1],T)+0.5)*dos[j-1] );
        sum += 0.5*(e[j]-e[j-1])*0.5*(  e[j]*coth( e[j]/(2.0*kB_to_eV*T) )*dos[j]
                +e[j-1]*coth( e[j-1]/(2.0*kB_to_eV*T) )*dos[j-1]  );
    return sum;
}

DP PDOS::S(const DP T)
{
    DP sum=0.0, small=1e-8;      
    for (int j=1; j<dos.size(); j++) 
//        if ((fabs(f_bose(e[j],T))<small)||(fabs(1.0+f_bose(e[j],T))<small))    // if the Bose-Einstein distribution or 1+Bose-Einstein is very small...
//            sum += 0.0;
//        else 
//           sum += 0.5*kB_to_eV*(e[j]-e[j-1])*
//                    (  ( ( 1+f_bose(e[j],T) )*log( 1+f_bose(e[j],T) )-f_bose(e[j],T)*log( f_bose(e[j],T) ) )*dos[j]+
//                    ( ( 1+f_bose(e[j-1],T) )*log( 1+f_bose(e[j-1],T) )-f_bose(e[j-1],T)*log( f_bose(e[j-1],T) ) )*dos[j-1]  );
    
            sum += 0.5*kB_to_eV*(e[j]-e[j-1])*
                    ((e[j]/(2.0*kB_to_eV*T)*coth(e[j]/(2.0*kB_to_eV*T))-log(2.0*sinh(e[j]/(2.0*kB_to_eV*T))))*dos[j]+
                    (e[j-1]/(2.0*kB_to_eV*T)*coth(e[j-1]/(2.0*kB_to_eV*T))-log(2.0*sinh(e[j-1]/(2.0*kB_to_eV*T))))*dos[j-1]);
    return sum;
}

DP PDOS::C(const DP T)
{
    DP sum=0.0;      
    for (int j=1; j<dos.size(); j++) 
//        sum += 0.5*kB_to_eV*(e[j]-e[j-1])*
//               ( sqr(e[j]/(kB_to_eV*T))*exp(e[j]/(kB_to_eV*T))*sqr(f_bose(e[j],T))*dos[j]+
//               sqr(e[j-1]/(kB_to_eV*T))*exp(e[j-1]/(kB_to_eV*T))*sqr(f_bose(e[j-1],T))*dos[j-1] );
        sum += 0.5*kB_to_eV*(e[j]-e[j-1])*
                (  sqr(e[j]/(kB_to_eV*T))/( 4.0*sqr(sinh(e[j]/(2.0*kB_to_eV*T))) )*dos[j]+
                sqr(e[j-1]/(kB_to_eV*T))/( 4.0*sqr(sinh(e[j-1]/(2.0*kB_to_eV*T))) )*dos[j-1]  );
    return sum;
}
            
*/

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool List_PDOS::is(const PDOS& pdos) const
{
    if (find(pdos)==end()) return false;
    return true;
}    

bool List_PDOS::is(const string name) const
{
    if (find(name)==end()) return false;
    return true;
}    

PDOS List_PDOS::get(const string name) const
{ 
    auto it=find(name);
    if (it==end()) throw;
    return *it;
}

string List_PDOS::list() const
{
    if (size()==0) return "";              // maybe better to throw here?
    string temp;
    for (auto it=begin(); it!=end(); it++) 
       temp+=it->name()+",";
    temp.erase(temp.size()-1,temp.size());    // remove last comma
    return temp;
}

void List_PDOS::calc(ostream& os, string name, DP Tin, DP Tfin, DP Tstep)
{
    // set all characters to upright
    name=to_up(name);
    
    // start processing the input string (can be just the function name or the full command CALCULATE NAME T P nmol list_of_site_fractions
    stringstream sss(name);
    string command;
    sss>>command;
    if (command=="CALCULATE")      // full command
        sss>>command>>name>>Tin>>Tfin>>Tstep;        // read full format, command here is =PDOS
    
    // search for the correct pdos in the list
    auto it=find(name);
    if (it==end()) throw ;
    
    // create a temp and compute
    PDOS temp(*it);
    temp.calc(os,Tin,Tfin,Tstep);
}

void List_PDOS::print_TDB(ostream& os, bool in_tdb)
{
    for (auto it=begin(); it!=end(); it++) 
       it->print_TDB(os,in_tdb);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

