/* 
 * File:   ev.cpp
 * Author: mauro
 *
 * Created on 27 April 2015, 13:33
 */

#include "ev.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 
// Constructors and basic functions
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


EV::EV(string fname, string name, string lattice, string v_unit, string e_unit, string norm)
        : n(name), chi(big), fitt(0), nminus(0), nplus(0)
{
    if (fname!="") {
        if (!this->read_from_file(fname,lattice,e_unit,v_unit,norm)) 
            cout<<"Error reading from file!"<<endl;
    }
    else throw;
    
    this->initialize_parameters();
} 


EV::EV(vector<DP> tempv, vector<DP> tempe, string name, string lattice, string v_unit, string e_unit, string norm)
        : n(name), chi(big), fitt(0), nminus(0), nplus(0)
{
    if (tempv.size()!=tempe.size()) throw;
    if (tempv.size()==0) throw;  
 
    int pos {0};
    for (int i=0; i<tempv.size(); i++) {    
        DP emin;

        if (tempv[i]<=0.0) throw;   // volume cannot be negative or zero
        DP ttempv=convertV(tempv[i],lattice,v_unit,norm);
        DP ttempe=convertE(tempe[i],e_unit,norm);
        v.push_back(ttempv);      // store input values
        e.push_back(ttempe);
        if (v.size()==1) 
            { emin=ttempe; pos_min=0; }     // first element, initialize emin and pos_min
        if (ttempe < emin) {                // check for the minimum energy
            emin = ttempe;               
            pos_min=pos;
        }      
        pos++;
    }
    
    this->initialize_parameters();
    this->set_fit_limits();
} 

EV::EV(const EV& ev)
    : n(ev.n), chi(ev.chi), fitt(ev.fitt), nminus(ev.nminus), nplus(ev.nplus),
        pos_min(ev.pos_min)
{
    v=ev.v;   
    e=ev.e;   
    
    vfit=ev.vfit;    
    efit=ev.efit;   
    
    p=ev.p;   
    
}

/* This function read the input file... more comments to be added */
bool EV::read_from_file(string fname, string lattice, string v_unit, string e_unit, string norm)
{
    string line;  // line is a buffer string, initially empty, to read input file line by line

    // Open input file
    ifstream ist(fname.c_str());        //  ist read the file fin
    if (!ist) {
        cerr<<"Input file cannot be opened"<<endl;
        return false;
    } else cout<<"Reading from file "<<fname<<endl;

    ist.clear();    
    int pos {0};
    
    // Start reading input file
    while(getline(ist,line))
    {
        stringstream ss(line);
        DP tempv, tempe, emin;
        
        if (ss>>tempv>>tempe) {     // read V and E
            // convert them to SI units
            tempv=convertV(tempv,lattice,v_unit,norm);
            tempe=convertE(tempe,e_unit,norm);
            v.push_back(tempv);     // store them
            e.push_back(tempe);
            if (v.size()==1) 
                { emin=tempe; pos_min=0; }     // first element, initialize emin and pos_min
            if (tempe < emin) {                // check for the minimum energy
                emin = tempe;               
                pos_min=pos;
            }      
            pos++;
        }
        else return false;
        
    }    
    
    this->set_fit_limits();
    return true;        
}

bool EV::write_to_file(string fname, string lattice, string v_unit, string e_unit, string norm)
{
    if (fname=="") fname=n+".out";
    // Open output file
    ofstream ost((fname).c_str());        //  ist read the file fin
    if (!ost) {
        cerr<<"Output file cannot be opened"<<endl;
        return false;
    } 
    ost.clear(); 
    
    this->print_results(ost,lattice,v_unit,e_unit,norm);
    return true;
}

void EV::initialize_parameters()
{    
    // Initialize parameters
    p.clear();
    p.push_back(e[pos_min]);          // minimun energy from input data
    p.push_back(v[pos_min]);          // volume corresponding to the minimun energy from input data
    p.push_back(500/Rydberg_to_kbar);  // a reasonable guess
    p.push_back(5.0);                  // a reasonable guess
    
}

void EV::set_fit_limits(int minus, int plus)
{
    // first set nminus and nplus according to input
    if (minus<0)     // take all data to the left of the minimun
        nminus=pos_min;
    else 
        nminus=minus;
    
     if (plus<0)     // take all data to the right of the minimun
        nplus=v.size()-pos_min-1;
    else 
        nplus=plus;   

    // copy subsets into vfit and efit
    vfit.clear();
    efit.clear();
    for (int i=pos_min-nminus; i<pos_min+nplus+1; i++) {
        vfit.push_back(v[i]);
        efit.push_back(e[i]);
    }        
}    

void EV::print_data(ostream& os)
{
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    os<<"Original EV dataset:"<<endl;
    for (int i=0; i<v.size(); i++)
        os<<v[i]<<"\t"<<e[i]<<"\n";
    os<<"pos_min= "<<pos_min<<"\tnminus= "<<nminus<<"\tnplus= "<<nplus<<endl;
    os<<"EV fitting subset:"<<endl;
    for (int i=0; i<vfit.size(); i++)
        os<<vfit[i]<<"\t"<<efit[i]<<"\n";
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
}

void EV::print_results(ostream& os,  string lattice, string v_unit, string e_unit, string norm)
{
    if (fitt) {
        os.precision(20);
        os<<scientific;
        os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
        os<<"EOS: Murnaghan\t\tChi^2/n= "<<convertEback(chi,e_unit,norm)<<" "<<e_unit<<"/"<<norm<<
                "\tE0= "<<convertEback(p[0],e_unit,norm)<<" "<<e_unit<<"/"<<norm;
        if (lattice!="V") os<<"\ta0= "<<convertVback(p[1],lattice,v_unit,norm)<<" "<<v_unit<<"/"<<norm;
        os<<endl;
        os<<"V0= "<<convertVback(p[1],"V",v_unit,norm)<<" "<<v_unit<<"^3/"<<norm<<"\tB0= "
            <<p[2]*Rydberg_to_kbar/10.0<<" GPa\tB0'= "<<p[3]<<"\tH0= "
            <<convertEback(Hfit(Vfit(0.0)),e_unit,norm)<<" "<<e_unit<<"/"<<norm<<endl;
        os<<"E(1bar)= "<<convertEback(Efit(Vfit(0.001/Rydberg_to_kbar)),e_unit,norm)<<" "<<e_unit<<"/"<<norm<<"\tV(1bar)= "
            <<convertVback(Vfit(0.001/Rydberg_to_kbar),"V",v_unit,norm)<<" "<<v_unit<<"^3/"<<norm<<"\tH(1bar)= "
            <<convertEback(Hfit(Vfit(0.001/Rydberg_to_kbar)),e_unit,norm)<<" "<<e_unit<<"/"<<norm<<endl;
        os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
        string label="V"; if (lattice!="V") label="a";
        string uadd; if (lattice=="V") uadd="^3";

        os<<"X "<<setw(6)<<label<<setw(25)<<"E"<<setw(25)<<"Efit"<<setw(25)<<
            "E-Efit"<<setw(21)<<"P"<<setw(25)<<"H"<<endl;
        os<<"X "<<setw(5)<<v_unit+uadd<<"/"<<norm<<setw(25)<<e_unit<<"/"<<norm<<setw(20)<<e_unit<<
            "/"<<norm<<setw(22)<<e_unit<<"/"<<norm<<setw(20)<<"GPa"<<setw(22)<<e_unit<<"/"<<norm<<endl;
        os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;

        for (int i=0; i<v.size(); i++) {
            string l=""; 
            if ( (i>=pos_min-nminus) && (i<pos_min+nplus+1) ) l="*";   // mark the data really used in the fitting
            os<<setw(10)<<convertVback(v[i],lattice,v_unit,norm)<<l<<"\t"<<convertEback(e[i],e_unit,norm)
                <<"\t"<<convertEback((Efit(v[i])),e_unit,norm)<<"\t"<<convertEback((e[i]-Efit(v[i])),e_unit,norm)
                    <<"\t"<<Pfit(v[i])*Rydberg_to_kbar/10.0<<"\t"<<convertEback(Hfit(v[i]),e_unit,norm)<<"\n";
        }
    }
    else os<<"No fit executed!"<<endl;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 
// Functions for fitting
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

DP EV::calc_E0() {
    DP E0 {0.0};
    for (int i=0; i<vfit.size(); i++) 
        E0 += efit[i]-Efit(vfit[i])+p[0];
    return E0/vfit.size();
}

DP EV::calc_chi() {
    DP chisq {0.0};  
    for (int i=0; i<vfit.size(); i++) 
        chisq += sqr(efit[i]-Efit(vfit[i]));
    return chisq/vfit.size();
}

DP EV::calc_E0(const vector<DP> p) {
    DP E0 {0.0};
    for (int i=0; i<vfit.size(); i++) 
        E0 += efit[i]-Efit(vfit[i],p)+p[0];
    return E0/vfit.size();
}

DP EV::calc_chi(const vector<DP> p, const DP E0) {
    DP chisq {0.0};  
    for (int i=0; i<vfit.size(); i++) 
        chisq += sqr(efit[i]-Efit(vfit[i],p)+p[0]-E0);
    return chisq/vfit.size();
}

void EV::fit()
{
    DP chisqold {0}, chisq {0}; 
    const int maxloops {4} ;
    const long int maxseeks {10000};
    
    this->initialize_parameters();  // reset fitting parameters
    // initialize vector with min and max values for parameters (corresponding to par vector)
    vector<DP> pmin { 0.0, 0.0, 0.0, 1.0 };  
    vector<DP> pmax { 0.0, 100000, 100000/Rydberg_to_kbar, 15.0 }; 
    // initial values for steps in parameters minimization
    vector<DP> deltap { 0.0, 0.1, 100/Rydberg_to_kbar, 1.0 };
    
    // create generator of uniform distributed random numbers from -0.5 to 0.5
    auto rand = bind (uniform_real_distribution<> {-0.5,0.5}, default_random_engine{});
    
    // initialize the chi squared and its "previous" value
    p[0]=calc_E0();
    chisq=calc_chi();
    chisqold=chisq;
    
    for (int loop=0; loop<maxloops; loop++) {
        for (int seek=0; seek<maxseeks; seek++) {
            vector<DP> newp;
            newp.push_back(p[0]);   // put E0
            for (int i=1; i<p.size(); i++) {
                DP temp {-1.0};
                while (temp>pmax[i] || temp<pmin[i])   // check if the new parameter is within range
                    temp=p[i]+rand()*deltap[i];
                newp.push_back(temp);
            }           
            
            newp[0]=calc_E0(newp);      // recalculate E0 with new parameters
            chisq=calc_chi(newp,newp[0]);    // recalculate chisq with new parameters
            
            if (chisq<chisqold) {       // if the new chisq is less than before...
                for (int i=0; i<p.size(); i++)        // update the parameters
                    p[i]=newp[i];
                chisqold=chisq;
            }
        }
        
        // after finishing the "seek" loop, reduce the step in the paramaters
        for (int i=1; i<deltap.size(); i++)        // update the parameters
            deltap[i]=deltap[i]/10.0;
    }
    
    // final call to evaluate chi and E0
    p[0]=calc_E0();
    chi=calc_chi();
    
    fitt=true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool List_EV::is(const EV& sp) const
{
    if (find(sp.name())==end()) return false;
    return true;
}    

bool List_EV::is(const string s) const
{
    if (find(s)==end()) return false;
    return true;
}    


EV List_EV::get(const string sp) const
{ 
    auto it=find(sp);
    if (it==end()) { throw no_such_EV(); }
    return (it->second);
}

EV List_EV::get(const EV& ev) const
{ 
    auto it=find(ev.name());
    if (it==end()) { throw no_such_EV(); }
    return (it->second);
}

void List_EV::fit()
{
    for (auto it=begin(); it!=end(); it++) {
        cout<<"Fitting "<<it->first<<"..."<<endl;
        it->second.fit();
    } 
}

void List_EV::set_fit_limits(int minus, int plus)
{
    for (auto it=begin(); it!=end(); it++) 
        it->second.set_fit_limits(minus,plus);    
}

string List_EV::list() const
{
    if (size()==0) return "";              // maybe better to throw here?
    string temp;
    for (auto it=begin(); it!=end(); it++) 
       temp+=it->first+",";
    temp.erase(temp.size()-1,temp.size());    // remove last comma
    return temp;
}

bool List_EV::write_to_file(string fname, string lattice, string v_unit,  string e_unit, string norm)
{    
    if (fname=="") {        // use identifier as output files plus .out
        for (auto it=begin(); it!=end(); it++) 
            it->second.write_to_file(it->first+".out",lattice,v_unit,e_unit,norm);
    }
    else {
        for (auto it=begin(); it!=end(); it++) 
            it->second.write_to_file(fname+"."+it->first+".out",lattice,v_unit,e_unit,norm);
    }
    return true;    
} 


bool List_EV::write_to_file(vector<string> fname, string lattice, string v_unit,  string e_unit, string norm)
{    
    if (fname.size()<this->size()) {
        cerr<<"Not enough output file names for all data in the list. Aborting writing to files."<<endl;
        return false;
    }
    auto it2=this->begin();
    for (auto it1=fname.begin(); it1!=fname.end(); it1++, it2++) 
        it2->second.write_to_file(*it1,lattice,v_unit,e_unit,norm); 
    return true;    
} 

 
bool List_EV::read_from_file(string fname, string lattice, string v_unit, string e_unit, string norm)
{
    this->insert(make_pair(fname,EV(fname,fname,lattice,v_unit,e_unit,norm))); 
    return true;
}

bool List_EV::read_from_file(vector<string> fname, string lattice, string v_unit, string e_unit, string norm)
{
    for (auto f:fname) 
        this->insert(make_pair(f,EV(f,f,lattice,v_unit,e_unit,norm))); 
    return true;
}

void List_EV::print_data(ostream& os)
{
    for (auto it=begin(); it!=end(); it++) {
        os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
        os<<"X  Data for "<<it->first<<endl;
        it->second.print_data(os);
    }
}

void List_EV::print_results(ostream& os, string lattice, string v_unit, string e_unit, string norm)
{
    for (auto it=begin(); it!=end(); it++) {
        os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
        os<<"X  Results for "<<it->first<<endl;
        it->second.print_results(os,lattice,v_unit,e_unit,norm);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 
// Auxiliary functions

DP convertV(DP V, string lattice, string vunit, string nnorm)
{
    DP fac=1.0;     // defaul normalization per atom
    if (to_up(nnorm)=="MOL") fac=NA;   // normalize per at
    vunit=to_up(vunit);
    if ( (lattice=="V") ) {      // input data are volumes
        if (vunit=="ANG") V = V*sqr3(Bohr_radius)/1.0E-30;
        if (vunit=="M")   V = V/sqr3(Bohr_radius);
        if (vunit=="NM")  V = V*sqr3(Bohr_radius)/1.0E-27;
        return V/fac;   // data already in m^3
    }
    else {                      // input data are lattice parameters
        if (vunit=="ANG") V = V*Bohr_radius/1.0E-10;
        if (vunit=="M")   V = V/(Bohr_radius);
        if (vunit=="NM")  V = V*Bohr_radius/1.0E-9;
        
        // compute volume according to lattice type
        lattice=to_up(lattice);                     // convert to uppercase
        if (lattice=="SC") return sqr3(V)/fac;          // simple cubic
        if (lattice=="FCC") return 0.25*sqr3(V)/fac;    // fcc lattice 
        if (lattice=="BCC") return 0.5*sqr3(V)/fac;     // bcc lattice
    }
}

DP convertVback(DP V, string lattice, string vunit, string nnorm)
{
    DP fac=1.0;     // defaul normalization per atom
    if (to_up(nnorm)=="MOL") fac=NA;   // normalize back per mol
    
    vunit=to_up(vunit);
    if ( (lattice=="V") ) {      // input data are volumes
        if (vunit=="ANG") V = V/sqr3(Bohr_radius)*1.0E-30;
        if (vunit=="M")   V = V*sqr3(Bohr_radius);
        if (vunit=="NM")  V = V/sqr3(Bohr_radius)*1.0E-27;
        return V*fac;   // data already in au^3
    }
    else {                      // input data are lattice parameters
        if (vunit=="ANG") V = V/Bohr_radius*1.0E-10;
        if (vunit=="M")   V = V*Bohr_radius;
        if (vunit=="NM")  V = V/Bohr_radius*1.0E-9;
        
        // compute lattice parameter according to lattice type
        lattice=to_up(lattice);                     // convert to uppercase
        if (lattice=="SC") return pow(V*fac,1.0/3.0);          // simple cubic
        if (lattice=="FCC") return pow(4.0*V*fac,1.0/3.0);     // fcc lattice 
        if (lattice=="BCC") return pow(2.0*V*fac,1.0/3.0);     // bcc lattice
    }
}

DP convertE(DP E, string eunit, string nnorm)
{    
    DP fac=1.0;     // default normalization per atom
    if (to_up(nnorm)=="MOL") fac=NA;   // normalize per mol
    eunit=to_up(eunit);
    if (eunit=="J") return E/Rydberg_to_J/fac;  // data are in J
    else if (eunit=="EV") return E*eV_to_Rydberg/fac;       // data are in eV
    return E/fac;     // data are already in Rydberg
}

DP convertEback(DP E, string eunit, string nnorm)
{
    DP fac=1.0;     // default normalization per atom
    if (to_up(nnorm)=="MOL") fac=NA;   // normalize per mol
    eunit=to_up(eunit);
    if (eunit=="J") return E*Rydberg_to_J*fac;  // data are in J
    else if (eunit=="EV") return E/eV_to_Rydberg*fac;       // data are in eV
    return E*fac;     // data are already in Rydberg
}
