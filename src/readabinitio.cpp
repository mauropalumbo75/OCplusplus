/* 
 * File:   readabinitio.cpp
 * Author: Mauro Palumbo
 *
 * Created on June 15, 2014, 8:52 PM
 */

#include "../include/readabinitio.h"
#include <map>

bool is_in_flip(int k,vector<int> flip)
{
    for (int i=0; i<flip.size(); i++)
        if (k==flip[i]) return true;
    return false;
}

istream& operator>>(istream& is, Composition& r)
{
    string t1;
    DP t2;
    
    is>>t1>>t2;
    if (!is) cerr<<"Bad composition in input file...";
    r.t=t1;
    r.c=t2;    
}

ostream& operator<<(ostream& os, Composition& r)
{
    return os<<r.t<<"\t"<<r.c<<"\n";
}


istream& operator>>(istream& is, ABLine& l)
{
    DP EEtot=0, eeV=0, eJ=0;                                 // Variables to read
    string c;
    string p;
    
    Composition skip_c;                         // Auxiliary variables
    string skip_s;
    DP skip_d;
    char ch1;

    if (!is) return is;

    while (is>>ch1 && ch1=='x') {               // Skip fields for compositions
        is.unget();
        is>>skip_c;
    }
    is.unget();
    
    is>>skip_d>>skip_d>>skip_s>>EEtot>>skip_s>>skip_s>>eeV>>    
            skip_s>>skip_s>>eJ>>skip_s>>skip_s>>skip_s>>c>>p;
    
  ///////////////////////////////////////////////////////////////////////////////
    
    // Remove ".FM" from phase name (if present)
    size_t FMpos=string::npos;
    FMpos=p.find(".FM");
    if (FMpos != string::npos)  p.erase(FMpos,3); 
   
    // Remove ".FM" from configuration name (if present)
    FMpos=c.find(".FM");
    if (FMpos != string::npos)  c.erase(FMpos,3);   
    
    // Remove phase name from configuration name (if present)
    FMpos=c.find(p);
    if (FMpos != string::npos)  c.erase(FMpos,p.size()+1);   

 ///////////////////////////////////////////////////////////////////////////////
    
    if (!is) {
//        cerr<<"Bad reading in input file..."<<endl;
        return is;
    }
    
    l.ph=to_up(p);
    l.conf=c;  
    l.Etot=EEtot;
    l.enteV=eeV;
    l.entJ=eJ;
    
    return is;
}


ostream& operator<<(ostream& os, ABLine& l)
{
    return os<<l.ph<<"\t"<<l.conf<<"\t"<<l.Etot<<" eV/at\t"<<l.enteV<<" meV/at\t"<<l.entJ<<" J/mol\n";
}


istream& operator>>(istream& is, ABLine_reference& l)
{
    string let, el, comm;
                      
    string skip_s;         // Auxiliary variables
    DP skip_d;

    if (!is) return is;

    // Read line as for example:
    // reference A: Co hcp.FM -7.107694
    is>>comm>>let>>el>>skip_s>>skip_d;
    
  ///////////////////////////////////////////////////////////////////////////////
    
    // Remove ":" from the letter (if present)
    size_t pos=string::npos;
    pos=let.find(":");
    if (pos != string::npos)  let.erase(pos,1); 
   
    // Remove all characters from "_" till end of string from element name (if present)
    pos=el.find("_");
    if (pos != string::npos)  el.erase(pos,string::npos); 
    

 ///////////////////////////////////////////////////////////////////////////////
    
    if (!is) {
//        cerr<<"Bad reading in input file..."<<endl;
        return is;
    }
    
    l.command=comm;
    l.letter=let[0];
    l.element=el;  
    
    return is;
}


ostream& operator<<(ostream& os, ABLine_reference& l)
{
    return os<<l.letter<<" = "<<l.element;
}

ostream& operator<<(ostream& os, multiplicities& l)
{
    os<<l.phase<<" / ";
    for (int i=0; i<l.m.size(); i++) os<<l.m[i]<<" ";
    os<<" / ";
    for (int i=0; i<l.flip.size(); i++) os<<l.flip[i]<<" ";
    os<<endl;
    return os;
}

// Convert configuration for special phases (fcc,bcc,hcp)
// Only up to binaries for now
string convert_special(string conf, map<char,string>& refs)
{
    string temp;

    // fcc phase    
    if (conf=="A1") temp=refs['A']+':'+refs['A']+':'+refs['A']+':'+refs['A'];          // unary case
    if (conf=="L12-A3B") temp=refs['B']+':'+refs['A']+':'+refs['A']+':'+refs['A'];
    if (conf=="L10-NiAl-AB") temp=refs['B']+':'+refs['B']+':'+refs['A']+':'+refs['A'];
    if (conf=="L12-AB3") temp=refs['B']+':'+refs['B']+':'+refs['B']+':'+refs['A'];
    
    // bcc phase
    if (conf=="A2") temp=refs['A']+':'+refs['A']+':'+refs['A']+':'+refs['A'];          // unary case
    if (conf=="D03-A3B") temp=refs['B']+':'+refs['A']+':'+refs['A']+':'+refs['A'];
    if (conf=="B32-AB") temp=refs['B']+':'+refs['A']+':'+refs['B']+':'+refs['A'];
    if (conf=="B2-AB") temp=refs['B']+':'+refs['B']+':'+refs['A']+':'+refs['A'];
    if (conf=="D03-AB3") temp=refs['B']+':'+refs['B']+':'+refs['B']+':'+refs['A'];

    // hcp phase 
    if (conf=="A3") temp=refs['A']+':'+refs['A']+':'+refs['A']+':'+refs['A'];          // unary case
    if (conf=="D0_19-A3B") temp=refs['B']+':'+refs['A']+':'+refs['A']+':'+refs['A'];
    if (conf=="hcp-AB"||conf=="AB") temp=refs['B']+':'+refs['B']+':'+refs['A']+':'+refs['A'];
    if (conf=="D0_19-AB3") temp=refs['B']+':'+refs['B']+':'+refs['B']+':'+refs['A'];    
    
    if (temp=="") temp="unknown";
    temp+=':';    // add a final semicolon, will be removed by the caller...
    cout<<"Conf in convert_special: "<<temp<<endl;
    return temp;

}

bool read_abinitio_file(List_elements& le, vector<ABLine>& v, List_structures& structures, string& fin,  vector<string>& lines)
{
    ABLine buf;
    ABLine_reference buf2;
    map<char,string> refs;
    string line;
    int old_v_size;
    
    vector<multiplicities> mult;
    vector<int> flip;    
    
    // Open input file
    ifstream ist(fin.c_str());        //  ist read the file fin
    if (!ist) {
        cerr<<"Input file cannot be opened"<<endl;
        return false;
    } else cout<<"Reading from file "<<fin<<endl;

    ist.clear();
    old_v_size=v.size();                  // last element of v
    string current_phase;
    
    while(getline(ist,line))            // Read line by line
    {
        char ch1;
        lines.push_back(line+"\n");
        cout<<"Reading line:\n "+line<<endl;
        stringstream ss(line);
        
        if (ss>>ch1 && ch1!='#') {                  //   if not a comment line
           ss.unget();
        
           if (ss>>buf) {                  // Process line with overloaded op >>
             v.push_back(buf);
             buf=ABLine();
           } 
        
           stringstream ss2(line);
           if (ss2>>buf2) {          // Process line defining A,B,C.. with overloaded op >>
              if (buf2.command=="reference") refs[buf2.letter]=buf2.element;
              buf2=ABLine_reference();
           }
           
           // Process line identifying the phase... (just read it)
           stringstream ss3(line);
           string buf3;
           if (ss3>>buf3 && buf3!="x(A)=") { 
                   size_t pos=string::npos;
                   pos=buf3.find("-");
                   if (pos != string::npos)  buf3.erase(0,pos+1); 
                   pos=buf3.find(".");
                   if (pos != string::npos)  buf3.erase(pos,string::npos); 
                   current_phase=buf3; 
                   cout<<"Current phase: "<<current_phase<<endl; 
           } 
        } else {    // here read the multiplicities
            string is_multiplicities;
            if (ss>>is_multiplicities && is_multiplicities=="Multiplicities") {
                string eq;
                if (ss>>eq && eq=="=") {
                    multiplicities temp;
                    temp.phase=current_phase;
                    DP buf4;
                    while (ss>>buf4) temp.m.push_back(buf4);
                    mult.push_back(temp);
                } else
                    cerr<<"Missing = after multiplicities... Is everything ok?"<<endl;
            }
        }
     }
    
    // Reference elements from ab initio files
    for (auto it=refs.begin(); it!=refs.end(); it++) {
        cout<<it->first<<"="<<it->second<<endl;
        le.insert(it->second);
    }
    
    // change A,B,C in the configurations with proper elements...
    // do it only to the latest loaded configurations
    string temp;
    for (int i=old_v_size; i<v.size(); i++) {
      if ((v[i].ph)=="BCC"||(v[i].ph)=="FCC"||(v[i].ph)=="HCP")
        temp=convert_special(v[i].conf, refs);    
      else 
        for (int j=0; j<v[i].conf.size(); j++)
          temp+=refs[v[i].conf[j]]+":";
      if (temp.size()>0) temp.erase(temp.size()-1,temp.size());
      v[i].conf=temp;
      temp="";
    }

/*    
/////////////   Still working on the flipping... 
    for (int i=0; i<mult.size(); i++) {
        Structure temp;
        temp=structures.get(mult[i].phase);   
        
        for (int j=0; j<mult[i].m.size(); j++) 
            for (int k=0; k<mult[i].m.size(); k++) {
                if (mult[i].m[j]==temp.mWyckoff(k) && !is_in_flip(k,mult[i].flip)) 
                  { mult[i].flip.push_back(k); break; }
        } 
    }
  
    cout<<"\nMultiplicities loaded from file "<<fin<<": "<<endl;    
    for (int i=0; i<mult.size(); i++) 
        cout<<mult[i];
*/
            
//    cout<<"\nEnergies read from file "<<fin<<": "<<endl;    
//    for (int i=old_v_size; i<v.size(); i++)
//        cout<<v[i];

    // line at the end of lines vector to separate different files read    
    lines.push_back("\n####################### END OF AB INITIO RAW DATA FILE ####################################\n");   

    return true;
}