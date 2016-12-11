/* 
 * File:   electronic.h
 * Author: Mauro Palumbo
 *
 * Created on 10 May 2015, 19:10
 */

#include "electronic.h"

EDOS::EDOS(string name, string e_unit, int nat, int nel, bool mag, DP ef, string fname)
{    
    // set all characters to upright
    name=to_up(name);
    
    // start processing the input string (can be just the PDOS name or the full definition)
    stringstream ss(name);
    string command,more;  
    ss>>command;
    command=get_subcommand_extended(command,"EDOS");
    if (command=="EDOS") {      // assume only the string ss with the full definition is provided
        // read full format
        ss>>n>>e_unit>>natoms>>nelectrons>>magn>>EF>>more;
        if (more==">") {                     // read next lines containing DOS data till !
            if (!read_dos(ss,e_unit)) throw;
        }       
        else if (more=="") throw;       // if next string is not ">" it must be a file name
        else if ( !read_dos_file(more,e_unit) ) throw;   
    }  
    else {
        n=name;
        natoms=nat;
        nelectrons=nel;
        magn=mag;
        if (fname!="")
            if ( !read_dos_file(fname,e_unit) ) throw;   
    }
    if (e.size()>0) {
        this->get_dos_ef();
        E0=E(0.0);      // calculate the ground state energy from the DOS at 0 K        
    }
}   

EDOS::EDOS(vector<DP>& tempe, vector<DP>& tempdos, vector<DP>& tempdosdown, string name, int nat)
        : n(name) 
{
    if (tempe.size()!=tempdos.size()) throw;
    if (tempe.size()==0) throw;  
    if (nat<1) throw;  
 
    for (int i=0; i<tempe.size(); i++) {    
        if (tempdos[i]<0.0) throw;      // dos cannot be negative
        dos.push_back(tempdos[i]);      // store input values
        dosdown.push_back(tempdosdown[i]);      // store input values
        e.push_back(tempe[i]);
    }
    natoms=nat;
    this->get_dos_ef();
    E0=E(0.0);      // calculate the ground state energy from the DOS at 0 K
} 

bool EDOS::read_dos(stringstream& ss, string e_unit)
{
    DP tempe, tempdos, tempdosdown;
    try {
        while (ss>>tempe>>tempdos) {     // read V and E
            // convert them to SI units
            tempe=convertE(tempe,e_unit);
            tempdos=convertdos(tempdos,e_unit);
            e.push_back(tempe);     // store them
            dos.push_back(tempdos);
            if (magn) {     // if magnetic read one more dos data per energy value
                ss>>tempdosdown;
                tempdosdown=convertdos(tempdosdown,e_unit);
                dosdown.push_back(tempdosdown);
            }
        }
        
        if (magn && dos.size()!=dosdown.size()) throw;
    }
    catch (...) { return false; }
    return true;                
}


/* This function read the input file... more comments to be added */
bool EDOS::read_dos_file(string fname, string e_unit)
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

    return true;        
}

string EDOS::to_string() const
{
    stringstream buf;
    buf.scientific; buf.precision(10);
    for (int i=0; i<e.size(); i++)
        buf<<e[i]<<"\t\t"<<dos[i]<<"\n";
    return buf.str();
} 

void EDOS::clear()
{
    e.clear();
    dos.clear();
}  

void EDOS::print_TDB(ostream& os, bool in_tdb) const
{
    os<<"EDOS "<<n<<" eV 1 "<<nelectrons<<" "<<magn<<" "<<EF<<" ";
    if (in_tdb) {           // write the DOS inside the tdb file
        os<<"> "<<endl;
        os.scientific; os.precision(10);
        for (int i=0; i<e.size(); i++) {
            if (magn) os<<e[i]<<"\t\t"<<dos[i]<<"\t\t"<<dosdown[i]<<"\n"; 
            else os<<e[i]<<"\t\t"<<dos[i]<<"\n";    
        }
        os<<"!"<<endl;
    } else {                // write the DOS in a different file whose name is DOSNAME.PDOS
        string fname=n+".EDOS";
        os<<fname<<" !"<<endl;
        
        // Open input file
        ofstream ost(fname.c_str());        //  ist read the file fin
        if (!ost) {
            cerr<<"Output file "<<fname<<" cannot be opened!"<<endl;
            throw;
        } 

        ost.clear();
        ost.scientific; ost.precision(10);
        for (int i=0; i<e.size(); i++) {
            if (magn) ost<<e[i]<<"\t\t"<<dos[i]<<"\t\t"<<dosdown[i]<<"\n"; 
            else ost<<e[i]<<"\t\t"<<dos[i]<<"\n";    
        }
    }
}

ostream& operator<<(ostream& os, const EDOS& edos)
{
    os<<"$---------------------------------------------------------------------"
            <<"--------------------------------------------"<<endl;
    os<<"$ Electronic DOS name: "<<edos.name()<<"\tN. of atoms: "<<edos.n_atoms()<<endl;
    os<<"$  Fermi energy E_F (eV) = "<<edos.E_F()<<endl;
    os<<"$  Raw DOS integral = "<<edos.integral(0.0)<<endl;
    os<<"$  Number of conduction electrons (per atom) from input DOS = "<<edos.electrons()<<endl;
    if (edos.size()!=0) { 
        os<<"$ DOS data:"<<endl;
        os.scientific; os.precision(10);
        for (int i=0; i<edos.size(); i++)
            if (edos.magnetic()) os<<edos.E(i)<<"\t\t"<<edos.DOS(i)<<"\t\t"<<edos.DOSDOWN(i)<<"\n";
            else os<<edos.E(i)<<"\t\t"<<edos.DOS(i)<<"\n";
    } else
        os<<"$ No DOS data!"<<endl;
    os<<"$---------------------------------------------------------------------"
            <<"--------------------------------------------"<<endl;
}

void EDOS::get_dos_ef()
{
    DP m=0.0;   // angular coeff. for linear interpolation
    int i=0;    // assumes the Fermi energy is not <= than the first element of DOS!
    while (i<e.size() && e[i]<EF) i++;    // get i, the closest, but less than EF
    if (i==e.size()) throw;
    
    m=(dos[i]-dos[i-1])/(e[i]-e[i-1]);
    dos_ef=dos[i-1]+m*(EF-e[i-1]);            // linear interpolation between i and i-1 elements
    if (magn) {
        m=(dosdown[i]-dosdown[i-1])/(e[i]-e[i-1]);
        dosdown_ef=dosdown[i-1]+m*(EF-e[i-1]);            // linear interpolation between i and i-1 elements
    }
}

DP EDOS::E_Sommerfeld(DP T)
{
    DP factor=1.0/natoms;
    if (magn) factor*=0.5;
    return (factor*sqr(pi)/3.0*dos_ef*sqr(kB_to_eV)*sqr(T)
            +factor*sqr(pi)/3.0*dosdown_ef*sqr(kB_to_eV)*sqr(T))
            *eV_to_J*NA;
}

DP EDOS::S_Sommerfeld(DP T)
{
    DP factor=2.0/natoms;
    if (magn) factor*=0.5;
    return factor*(sqr(pi)/3.0*dos_ef*sqr(kB_to_eV)*T
            +sqr(pi)/3.0*dosdown_ef*sqr(kB_to_eV)*T)
            *eV_to_J*NA;
}

DP EDOS::C_Sommerfeld(DP T)
{
    return S_Sommerfeld(T);     // in the Sommerfeld approximation, Cv and S are the same
}

// All the routines below are based on the Simpson 3/8 rule of integration
DP EDOS::integral(const DP moment) const
{
    if (dos.size()<3) return 0;   // not enough points
    DP sum {0.0}, sumdown {0.0}; 
    DP h=0.5*(e[2]-e[0]);
    for (int j=0; j<(dos.size()-3); j+=3) 
        sum += 3.0*pow(e[j],moment)*dos[j]
                +3.0*pow(e[j+1],moment)*dos[j+1]
                +2.0*pow(e[j+2],moment)*dos[j+2];
    if (magn)
        for (int j=0; j<(dosdown.size()-3); j+=3) 
            sumdown += 3.0*pow(e[j],moment)*dosdown[j]
                +3.0*pow(e[j+1],moment)*dosdown[j+1]
                +2.0*pow(e[j+2],moment)*dosdown[j+2];
    
    return h*(sum+sumdown)*3.0/8.0;
}

DP EDOS::electrons() const
{
    if (dos.size()<3) return 0;   // not enough points
    DP sum {0.0}, sumdown {0.0}; 
    DP h=0.5*(e[2]-e[0]);
    for (int j=0; j<(dos.size()-3); j+=3) 
        sum += 3.0*dos[j]*f_fermi(e[j],0.0,EF)
                +3.0*dos[j+1]*f_fermi(e[j+1],0.0,EF)
                +2.0*dos[j+2]*f_fermi(e[j+2],0.0,EF);
    if (magn)
        for (int j=0; j<(dosdown.size()-3); j+=3) 
            sumdown += 3.0*dosdown[j]*f_fermi(e[j],0.0,EF)
                +3.0*dosdown[j+1]*f_fermi(e[j+1],0.0,EF)
                +2.0*dosdown[j+2]*f_fermi(e[j+2],0.0,EF);
    
    return h*(sum+sumdown)*3.0/8.0/natoms;
}


DP EDOS::E(DP T)
{
    if (dos.size()<3) return 0;   // not enough points
    DP sum {0.0}, sumdown {0.0}; 
    DP h=(e[2]-e[0]);    
    if (magn) h*=0.5;
    for (int j=1; j<(dos.size()-3); j+=3) 
        sum += 3.0*(e[j]-EF)*dos[j]*f_fermi(e[j],T,EF)
                +3.0*(e[j+1]-EF)*dos[j+1]*f_fermi(e[j+1],T,EF)
                +2.0*(e[j+2]-EF)*dos[j+2]*f_fermi(e[j+2],T,EF);
    if (magn)
        for (int j=1; j<(dosdown.size()-3); j+=3) 
        sumdown += 3.0*(e[j]-EF)*dosdown[j]*f_fermi(e[j],T,EF)
                +3.0*(e[j+1]-EF)*dosdown[j+1]*f_fermi(e[j+1],T,EF)
                +2.0*(e[j+2]-EF)*dosdown[j+2]*f_fermi(e[j+2],T,EF);
    
    return h*(sum+sumdown)*3.0/8.0/natoms*eV_to_J*NA-E0;
}

DP EDOS::S(DP T)
{
    if (dos.size()<3) return 0;   // not enough points
    DP sum {0.0}, sumdown {0.0}; 
    DP h=-kB_to_eV*(e[2]-e[0]);
    if (magn) h*=0.5;
    for (int j=1; j<(dos.size()-3); j+=3) 
        if ( !( f_fermi(e[j],T,EF)<eps || (1.0-f_fermi(e[j],T,EF)<eps) ) )
            sum += 3.0*(f_fermi(e[j],T,EF)*log(f_fermi(e[j],T,EF))+(1.0-f_fermi(e[j],T,EF))*log(1.0-f_fermi(e[j],T,EF)))*dos[j]
                +3.0*(f_fermi(e[j+1],T,EF)*log(f_fermi(e[j+1],T,EF))+(1.0-f_fermi(e[j+1],T,EF))*log(1.0-f_fermi(e[j+1],T,EF)))*dos[j+1]
                +2.0*(f_fermi(e[j+2],T,EF)*log(f_fermi(e[j+2],T,EF))+(1.0-f_fermi(e[j+2],T,EF))*log(1.0-f_fermi(e[j+2],T,EF)))*dos[j+2];
        
    if (magn)
        for (int j=0; j<(dosdown.size()-3); j+=3) 
        if ( !( f_fermi(e[j],T,EF)<eps || (1.0-f_fermi(e[j],T,EF)<eps) ) )
            sumdown += 3.0*(f_fermi(e[j],T,EF)*log(f_fermi(e[j],T,EF))+(1.0-f_fermi(e[j],T,EF))*log(1.0-f_fermi(e[j],T,EF)))*dosdown[j]
                +3.0*(f_fermi(e[j+1],T,EF)*log(f_fermi(e[j+1],T,EF))+(1.0-f_fermi(e[j+1],T,EF))*log(1.0-f_fermi(e[j+1],T,EF)))*dosdown[j+1]
                +2.0*(f_fermi(e[j+2],T,EF)*log(f_fermi(e[j+2],T,EF))+(1.0-f_fermi(e[j+2],T,EF))*log(1.0-f_fermi(e[j+2],T,EF)))*dosdown[j+2];
    
    return h*(sum+sumdown)*3.0/8.0/natoms*eV_to_J*NA;
}

DP EDOS::C(DP T)
{
    if (dos.size()<3) return 0;   // not enough points
    DP sum {0.0}, sumdown {0.0}; 
    DP h=-(e[2]-e[0]);    
    if (magn) h*=0.5;
    for (int j=1; j<(dos.size()-3); j+=3) 
        sum += 3.0*sqr(e[j]-EF)/T*dos[j]*df_fermi_dE(e[j],T,EF)
                +3.0*sqr(e[j+1]-EF)/T*dos[j+1]*df_fermi_dE(e[j+1],T,EF)
                +2.0*sqr(e[j+2]-EF)/T*dos[j+2]*df_fermi_dE(e[j+2],T,EF);
    if (magn)
        for (int j=1; j<(dosdown.size()-3); j+=3) 
        sumdown += 3.0*sqr(e[j]-EF)/T*dosdown[j]*df_fermi_dE(e[j],T,EF)
                +3.0*sqr(e[j+1]-EF)/T*dosdown[j+1]*df_fermi_dE(e[j+1],T,EF)
                +2.0*sqr(e[j+2]-EF)/T*dosdown[j+2]*df_fermi_dE(e[j+2],T,EF);
    
    return h*(sum+sumdown)*3.0/8.0/natoms*eV_to_J*NA;
}


void EDOS::calc(ostream& os, DP Tin, DP Tfin, DP Tstep, int model)
{    
    if (Tin<0 || Tfin<0) throw;
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    os<<"X  Electronic thermodynamic properties from DOS"<<endl;
    os<<"X  N. of atoms = "<<natoms<<endl;
    os<<"X  Fermi energy E_F (eV) = "<<EF<<endl;
    os<<"X  Raw DOS integral = "<<integral(0.0)<<endl;
    os<<"X  Number of conduction electrons (per atom) from input DOS = "<<electrons()<<endl;
    if (model==0) {
        os<<"X  Sommerfeld model "<<endl;
        if (magn) 
            os<<"X  DOS up at E_F= "<<dos_ef<<"\tDOS down at E_F= "<<dosdown_ef<<endl;
        else
            os<<"X  DOS at E_F= "<<dos_ef<<endl;
    }
    else
        os<<"X  DOS integral model "<<endl;
     
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    os<<"X\tT\t\t\tEel\t\t\tFel\t\t\tSel\t\t\tCel"<<endl;
    os<<"X\tK\t\t\tJ/mol\t\t\tJ/mol\t\t\tJ/mol/K\t\t\tJ/mol/K"<<endl;
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    os.precision(15);
    os<<scientific;    
    for (DP T=Tin; T<=Tfin; T+=Tstep)
        if (model==0)
            os<<T<<"\t"<<E_Sommerfeld(T)<<"\t"<<F_Sommerfeld(T)<<"\t"<<S_Sommerfeld(T)<<"\t"<<C_Sommerfeld(T)<<endl;
        else
            os<<T<<"\t"<<E(T)<<"\t"<<F(T)<<"\t"<<S(T)<<"\t"<<C(T)<<endl;
    // Just for testing in other units
//        os<<T<<"\t"<<E(T)/Rydberg_to_J/NA<<"\t"<<F(T)/Rydberg_to_J/NA<<
//                "\t"<<S(T)/R<<"\t"<<C(T)/R<<endl;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool List_EDOS::is(const EDOS& edos) const
{
    if (find(edos)==end()) return false;
    return true;
}    

bool List_EDOS::is(const string name) const
{
    if (find(name)==end()) return false;
    return true;
}    

EDOS List_EDOS::get(const string name) const
{ 
    auto it=find(name);
    if (it==end()) throw;
    return *it;
}

string List_EDOS::list() const
{
    if (size()==0) return "";              // maybe better to throw here?
    string temp;
    for (auto it=begin(); it!=end(); it++) 
       temp+=it->name()+",";
    temp.erase(temp.size()-1,temp.size());    // remove last comma
    return temp;
}

void List_EDOS::calc(ostream& os, string name, DP Tin, DP Tfin, DP Tstep)
{
    // set all characters to upright
    name=to_up(name);
    
    // start processing the input string (can be just the function name or the full command CALCULATE NAME T P nmol list_of_site_fractions
    stringstream sss(name);
    string command;
    sss>>command;
    if (command=="CALCULATE")      // full command
        sss>>command>>name>>Tin>>Tfin>>Tstep;        // read full format, command here is =PDOS
    
    // search for the correct edos in the list
    auto it=find(name);
    if (it==end()) throw ;
    
    // create a temp and compute
    EDOS temp(*it);
    temp.calc(os,Tin,Tfin,Tstep);
}

void List_EDOS::print_TDB(ostream& os, bool in_tdb)
{
    for (auto it=begin(); it!=end(); it++) 
       it->print_TDB(os,in_tdb);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////