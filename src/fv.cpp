/* 
 * File:   fv.cpp
 * 
 * Author: Mauro Palumbo
 *
 * Created on 20 August 2015, 13:33
 */

#include "fv.h"
#include "../ev/ev.h"
#include <iostream>


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// 
// Constructors and basic functions
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


FV::FV(string fname, string name)
        : n(name), chi(big), fitt(0)
{
    if (fname!="") {
        if (!this->read_from_file(fname)) 
            cout<<"Error reading from file!"<<endl;
    }
    else throw;
    
    this->initialize_parameters();
} 


FV::FV(vector<DP> tempv, vector<DP> tempe, string name)
        : n(name), chi(big), fitt(0)
{
    if (tempv.size()!=tempe.size()) throw;
    if (tempv.size()==0) throw;  
 
    for (int i=0; i<tempv.size(); i++) {    

        if (tempv[i]<=0.0) throw;   // volume cannot be negative or zero
        v.push_back(tempv[i]);      // store input values
        f.push_back(tempe[i]);
    }
    
    this->initialize_parameters();
} 

FV::FV(const FV& fv)
    : n(fv.n), chi(fv.chi), fitt(fv.fitt)
{
    v=fv.v;   
    f=fv.f;     
    p=fv.p;       
}

/* This function read the input file... more comments to be added */
bool FV::read_from_file(string fname)
{
    string line;  // line is a buffer string, initially empty, to read input file line by line

    // Open input file
    ifstream ist(fname.c_str());        //  ist read the file fin
    if (!ist) {
        cerr<<"Input file cannot be opened"<<endl;
        return false;
    } else cout<<"Reading from file "<<fname<<endl;

    ist.clear();    
    
    // Start reading input file
    while(getline(ist,line))
    {
        stringstream ss(line);
        DP tempv, tempe;
        
        if (ss>>tempv>>tempe) {     // read V and E
            v.push_back(tempv);     // store them
            f.push_back(tempe);
        }
        else return false;        
    }    

    return true;        
}

bool FV::write_to_file(string fname)
{
    if (fname=="") fname=n+".out";
    // Open output file
    ofstream ost((fname).c_str());        //  ist read the file fin
    if (!ost) {
        cerr<<"Output file cannot be opened"<<endl;
        return false;
    } 
    ost.clear(); 
    
    this->print_results(ost);
    return true;
}

void FV::initialize_parameters()
{    
    p.clear();
    p.push_back(-1.0);          // minimun energy from input data
    p.push_back(1.0);           // volume corresponding to the minimun energy from input data
    p.push_back(1.0);           // a reasonable guess
}
 

void FV::print_data(ostream& os)
{
    for (int i=0; i<v.size(); i++)
        os<<v[i]<<"\t"<<f[i]<<"\n";
}

void FV::print_results(ostream& os)
{
    if (fitt) {
        os.precision(20);
        os<<scientific;
        os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
        os<<"Polynomial a+bV+cV^2\t\tChi^2/n= "<<chi<<endl;
        os<<"a= "<<p[0]<<" \tb= "<<p[1]<<" \tc= "<<p[2]<<endl;
        os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
        os<<"X "<<setw(6)<<"V"<<setw(25)<<"F"<<setw(25)<<"Ffit"<<setw(25)<<
            "F-Ffit"<<endl;
        os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;

        for (int i=0; i<v.size(); i++) {
            os<<setw(10)<<v[i]<<"\t"<<f[i]<<"\t"<<Ffit(v[i])<<"\t"<<(f[i]-Ffit(v[i]))<<endl;
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


DP FV::calc_chi() {
    DP chisq {0.0};  
    for (int i=0; i<v.size(); i++) 
        chisq += sqr(Ffit(v[i])-f[i]);
    return chisq/v.size();
}


void FV::fit()
{
    int i,j,k,l,m;
    DP ym,wt;
    
    MDP beta(p.size(),1);    
    MDP covar(p.size(),p.size());
    
    cout<<"Fitting..."<<endl;
    
    // initialize the covariance matrix and beta
    for (i=0; i<p.size(); i++)    {
        for (j=0; j<p.size(); j++)
            covar[i][j]=0.0;
        beta[i][0]=0.0;
    }
    
    for (i=0; i<v.size(); i++) {
        ym=f[i];
        for (j=0,l=0; l<p.size(); l++) {
            wt=Fpoly(v[i],l);
            for (k=0,m=0; m<=l; m++)
                covar[j][k++] += wt*Fpoly(v[i],m);
            beta[j++][0] += ym*wt;
        }
    }
    
    for (j=1; j<p.size(); j++)
        for (k=0; k<j; k++)
            covar[k][j]=covar[j][k];
    
/*    for (i=0; i<p.size(); i++)
        for (j=0; j<p.size(); j++)
            cout<<covar[i][j]<<endl;
    
    for (i=0; i<p.size(); i++)
        cout<<beta[i][0]<<endl;
*/  


    gaussjordan(covar,beta);
    
    for (i=0; i<p.size(); i++)  {
        p[i]=beta[i][0];
    }
    
    
    chi=calc_chi();    
  
    fitt=true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


bool List_FV::is(const FV& sp) const
{
    if (find(sp.name())==end()) return false;
    return true;
}    

bool List_FV::is(const string s) const
{
    if (find(s)==end()) return false;
    return true;
}    


FV List_FV::get(const string sp) const
{ 
    auto it=find(sp);
    if (it==end()) { throw no_such_FV(); }
    return (it->second);
}

FV List_FV::get(const FV& fv) const
{ 
    auto it=find(fv.name());
    if (it==end()) { throw no_such_FV(); }
    return (it->second);
}

void List_FV::fit()
{
    for (auto it=begin(); it!=end(); it++) {
        cout<<"Fitting "<<it->first<<"..."<<endl;
        it->second.fit();
    } 
}

string List_FV::list() const
{
    if (size()==0) return "";              // maybe better to throw here?
    string temp;
    for (auto it=begin(); it!=end(); it++) 
       temp+=it->first+",";
    temp.erase(temp.size()-1,temp.size());    // remove last comma
    return temp;
}

bool List_FV::write_to_file(string fname)
{    
    if (fname=="") {        // use identifier as output files plus .out
        for (auto it=begin(); it!=end(); it++) 
            it->second.write_to_file(it->first+".out");
    }
    else {
        for (auto it=begin(); it!=end(); it++) 
            it->second.write_to_file(fname+"."+it->first+".out");
    }
    return true;    
} 


bool List_FV::write_to_file(vector<string> fname)
{    
    if (fname.size()<this->size()) {
        cerr<<"Not enough output file names for all data in the list. Aborting writing to files."<<endl;
        return false;
    }
    auto it2=this->begin();
    for (auto it1=fname.begin(); it1!=fname.end(); it1++, it2++) 
        it2->second.write_to_file(*it1); 
    return true;    
} 

 
bool List_FV::read_from_file(string fname)
{
    this->insert(make_pair(fname,FV(fname))); 
    return true;
}

bool List_FV::read_from_file(vector<string> fname)
{
    for (auto f:fname) 
        this->insert(make_pair(f,FV(f,f))); 
    return true;
}

void List_FV::print_data(ostream& os)
{
    for (auto it=begin(); it!=end(); it++) {
        os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
        os<<"X  Data for "<<it->first<<endl;
        it->second.print_data(os);
    }
}

void List_FV::print_results(ostream& os)
{
    for (auto it=begin(); it!=end(); it++) {
        os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
            <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
        os<<"X  Results for "<<it->first<<endl;
        it->second.print_results(os);
    }
}