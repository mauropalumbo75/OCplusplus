/* 
 * File:   qha.h
 * Author: mauro
 *
 * Created on 05 May 2015, 13:29
 */

#include "qha.h"


DP Thermophys::r(const string var)
{
    if (var=="T")               return T;
    if (var=="P")               return P;
    if (var=="V")               return V;
    if (var=="B0")              return B0;
    if (var=="B0p")             return B0p;
    if (var=="Fharm")           return Fharm;
    if (var=="F")               return F;
    if (var=="G")               return G;
    if (var=="Sharm")           return Sharm;
    if (var=="S")               return S;
    if (var=="E")               return E;
    if (var=="H")               return H;
    if (var=="Charm")           return Charm;
    if (var=="Cfrom_eq")        return Cfrom_eq;
    if (var=="C")               return C;
    if (var=="beta")            return beta;    
    if (var=="Gruneisen")       return Gruneisen;
    if (var=="chi")             return chi;
    if (var=="temp")            return temp;
}


void Thermophys::w(const string var, const DP x)
{
    if (var=="T")               T=x;
    if (var=="P")               P=x;
    if (var=="V")               V=x;
    if (var=="B0")              B0=x;
    if (var=="B0p")             B0p=x;
    if (var=="Fharm")           Fharm=x;
    if (var=="F")               F=x;
    if (var=="G")               G=x;
    if (var=="Sharm")           Sharm=x;
    if (var=="S")               S=x;
    if (var=="E")               E=x;
    if (var=="H")               H=x;
    if (var=="Charm")           Charm=x;
    if (var=="Cfrom_eq")        Cfrom_eq=x;
    if (var=="C")               C=x;
    if (var=="beta")            beta=x;    
    if (var=="Gruneisen")       Gruneisen=x;    
    if (var=="chi")             chi=x;    
    if (var=="temp")            temp=x;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool QHA::read_alat_files(string ident, string fname)
{
    // Open input file
    if (ident!="") fname=ident+fname;
    ifstream ist(fname.c_str());        //  ist read the file fin
    if (!ist) {
        cerr<<"Alat_values file "<<fname<<" cannot be opened!"<<endl;
        throw;
    } 
    ist.clear();
    
    string values;
    while (ist>>values) alat.push_back(values);
}

bool QHA::read_pdos_files(string ident, string fname, string e_unit, int nat)
{
    if (ident!="") fname=ident+fname;
    for (auto i: alat) {
        pdos.insert("PDOS_"+i,e_unit,nat,fname+"."+i);
    }
}

bool QHA::read_EV(string ident, string f, string lattice, string v_unit, string e_unit, string norm)
{
    if (ident!="") f=ident+f;
    cout<<f<<endl;
    EV temp {f};
    if (!temp.read_from_file(f,lattice,v_unit,e_unit,norm)) return false;  // read from file, if something goes wrong return false
    this->insert(make_pair(f,temp));
}

bool QHA::calc_harmonic(string ident, DP Tin, DP Tfin, DP Tstep)
{
    for (auto i: alat) {
        // Open output file
        string fname {};
        if (ident=="") fname="harm."+i;
        else fname=ident+"harm."+i;
        ofstream ost(fname.c_str());        //  ist read the file fin
        if (!ost) {
            cerr<<"Output file "<<fname<<" cannot be opened!"<<endl;
            throw;
        } 
        ost.clear();        
        pdos.calc(ost,"PDOS_"+i,Tin,Tfin,Tstep);            
    }
}


bool QHA::qha_from_dos(vector<string> vT, string ident, string lattice, string v_unit, string e_unit, int nat)
{
    if (vT.size()<=0) return false;
    
    DP Tmin {0}, Tmax {1};
    DP Tstep {1}, Ttemp {0};
    
    if (!from_string(Tmin,vT[0])) return false;
    if (!from_string(Tmax,vT[vT.size()-1])) return false;
    if (!from_string(Ttemp,vT[1])) return false;
    if (vT.size()>1) Tstep=Ttemp-Tmin;

    read_alat_files(ident);
    read_pdos_files(ident,"phdos",e_unit,nat);
   
    cout<<pdos.list()<<endl;
    calc_harmonic(ident,Tmin,Tmax,Tstep);
    
    read_EV(ident,"EvsV.in","V");

    
    for (auto t:vT) {
        string fname=ident+"FvsV."+t+".in";
        ofstream ost(fname.c_str());        //  ist read the file fin
        if (!ost) {
            cerr<<"Output file "<<fname<<" cannot be opened!"<<endl;
            throw;
        } 
        ost.clear(); 
        ost.precision(20);
        
        int count {0};
        DP tt {0.0};
        from_string(tt,t);
        vector<DP> tempv, tempe;
        for (auto i:alat) {
            tempv.push_back(this->get(ident+"EvsV.in").V(count));
            tempe.push_back(this->get(ident+"EvsV.in").E(count)+pdos.get("PDOS_G"+to_string(count+1)).F_Ryd(tt));
//            tempe.push_back(this->get(ident+"EvsV.in").E(count)+pdos.get("PDOS_"+i).F_Ryd(tt));
//            tempe.push_back(pdos.get("PDOS_"+i).F_Ryd(tt));

            count++;
        }
        
        EV temp(tempv, tempe, fname, "V");
        temp.print_data(ost);
        this->insert(make_pair(fname,temp));
        
    }    
    
    T=vT;
    id=ident;
    return true;
}

bool QHA::read_qha_files(vector<string> vT, string ident, string lattice, string v_unit, string e_unit, string norm)
{
    if (vT.size()<=0) return false;
    
    string f; // auxiliary string
    
    // First read the EvsV data (0 K static energies)
    if (!read_EV(ident,"EvsV.in",lattice,v_unit,e_unit,norm)) return false;
    
    // then loop over the T vector and read the FvsV data (vibrational energies at T K)
    for (auto t:vT) {
        f="FvsV."+t+".in";
        if (!read_EV(ident,f,lattice,v_unit,e_unit,norm)) return false;
    }
    
    T=vT;
    id=ident;
    return true;
}

bool QHA::write_qha_results(vector<string> outfiles)
{
    // loop to print output on output files...
    for (int j=0; j<outfiles.size(); j++) {
        
        // Open output file
        ofstream ost((id+outfiles[j]).c_str());        //  ost write the file outfiles[j]
        if (!ost) {
            cerr<<"Output file cannot be opened"<<endl;
            exit(1);
        }
        ost.clear();
        
        for (int i=0; i<F.size(); i++) 
            ost<<scientific<<setprecision(20)<<F[i].r("T")<<"\t"<<F[i].r(outfiles[j])<<endl;
    }
}


void QHA::calculate(const DP P)
{
    string f; // auxiliary string
    
    cout<<this->list()<<endl;
    
    // First consider the EvsV data (0 K static energies) to get reference values of the energy
    f=id+"EvsV.in";
    EV Est=get(f);
    if (!Est.fitted()) {
        cout<<"Fitting "<<f<<"..."<<endl;
        Est.fit();
    }
    // save the reference quantities in Estat
    E.w("P",P); E.w("E",Est.E_P(P)); E.w("V",Est.V_P(P)); 
    E.w("H",Est.H_P(P));  E.w("B0",Est.B0()); E.w("B0p",Est.B0p());
    E.w("chi",Est.chisq());  
    
    DP E0=Est.E_P(P), H0=Est.H_P(P);
    DP V0=Est.V_P(P);       // initialize Vmin to V0(P) from E_static
     // then loop over the T vector and read the EOS data at each T
    for (auto t:T) {
        f=id+"FvsV."+t+".in";
        EV Fvib=get(f);
        if (!Fvib.fitted()) {
            cout<<"Fitting "<<f<<"..."<<endl;
            Fvib.fit();
        }
        
        if (t=="1") V0=Fvib.V_P(P);  // if available, correct V0 to V0(P) including ZPE
        Thermophys temp;
        temp.w("P",P); temp.w("V",Fvib.V_P(P)); temp.w("T",str_to_double(t));
        temp.w("B0",Fvib.B0()); temp.w("B0p",Fvib.B0p()); temp.w("chi",Fvib.chisq()); 
//        temp.w("F",Fvib.E_P(P)-E0); // Careful here!!! Fvib.E_P(P) is F, not E, because the fit is done on Est+Fvib at each T
        temp.w("temp",Fvib.E_P(P)-E0);
        temp.w("G",Fvib.H_P(P)-H0); // Careful here!!! Fvib.H_P(P) is G, not H, because the fit is done on Est+Fvib at each T
        
        // Compute F at the constant volume V(P) including the ZPE energy. 
        // This should be essentially the same as F harmonic recalculated from the
        // phonons at this volume, but here obtained using the fitted EOS at each T
        temp.w("Fharm",Fvib.E_V(V0)-E0);    
        
        F.push_back(temp);
    }   
    
    deriv("temp", "T", "F", 10, 5, 5, 0, 5);
    
    DP buf;                              // auxiliary variable(s)
    int i;
    
    // First compute beta=1/V*(dV/dT) 
    deriv("V", "T", "beta", 0);
//    deriv("V", "T", "beta", 9, 4, 4, 1, 4);
    for (i=1; i<F.size()-1; i++)  
        F[i].w("beta",1.0/F[i].r("V")*F[i].r("beta"));

    // Now compute Sharm=-dFharm/dT 
    deriv("Fharm", "T", "Sharm", 0);
//    deriv("Fharm", "T", "Sharm", 9, 4, 4, 1, 4);    
    for (i=1; i<F.size()-1; i++)  
        F[i].w("Sharm",-1.0*F[i].r("Sharm"));

    
    // Now compute S=-dF/dT 
    // This is not entirely correct (see next) but a reasonable approximation
    // Compute S either with this loop or the following...
    deriv("F", "T", "S", 0);
//    deriv("F", "T", "S", 9, 4, 4, 1, 4);
    for (i=1; i<F.size()-1; i++)  
        F[i].w("S",-1.0*F[i].r("S"));

/*    
    // Now compute S=-(dF/dT) at V=const (!) from finite differences
    // This is a bit more complicated, because the derivative is at constant V, but
    // we have F as a function of T at constant P. For each T, we must evaluate from
    // EOS the F(T-1) and F(T+1) at the same volume and then calculate the finite difference 
    for (i=1; i<F.size()-1; i++)  {
        // First get the volume at F(T)
        DP Vi=F[i].r("V"); 
        
        // get the EOS at T-1
        string ftemp=f;
        ftemp=ftemp+"."+to_string(i)+".in";    // !! T-1 -> i in EOS
        cout<<ftemp<<endl;
        
        EV EOS1 {get(ftemp)};
        if (!EOS1.fitted()) {
            cout<<"Fitting "<<ftemp<<"..."<<endl;
            EOS1.fit();
        }
        DP F1=EOS1.E_V(Vi);     // get F(T-1) from the EOS at T-1, V=Vi
        
        // get the EOS at T+1
        ftemp=f;
        ftemp=ftemp+"."+to_string(i+2)+".in";   // !! T+1 -> i+2 in EOS
                cout<<ftemp<<endl;
        EV EOS2=get(ftemp);
        if (!EOS2.fitted()) {
            cout<<"Fitting "<<ftemp<<"..."<<endl;
            EOS2.fit();
        }
        DP F2=EOS2.E_V(Vi);     // get F(T+1) from the EOS at T+1, V=Vi
        
        buf=0.0;
        buf=-(F2-F1)/2.0;
        F[i].w("S",buf);
    }
*/    
    // Now compute Charm=TdS/dT at P=0 
    deriv("Sharm", "T", "Charm", 0);
//    deriv("Sharm", "T", "Charm", 9, 4, 4, 1,4);
    for (i=1; i<F.size()-1; i++)  
        F[i].w("Charm",F[i].r("T")*F[i].r("Charm"));
       
    // Now compute C=TdS/dT 
    deriv("S", "T", "C", 0);
//    deriv("S", "T", "C", 9, 4, 4, 1, 4);
    for (i=1; i<F.size()-1; i++)  
        F[i].w("C",F[i].r("T")*F[i].r("C"));
    
    // Now that we have S, we can compute E=F+TS and H=G+TS
    for (i=1; i<F.size()-1; i++)  {
        F[i].w("E",F[i].r("F")+F[i].r("T")*F[i].r("S"));
        F[i].w("H",F[i].r("G")+F[i].r("T")*F[i].r("S"));
    }

    calc=true;
}

bool QHA::deriv(string f, string x, string dfdx, int NP, int NL, int NR, int LD, int M)
{
    if (f==""||x==""||dfdx=="") return false;
    
    DP gi;                              // auxiliary variable(s)
    int i,n;                            // counters
    
    if (NP==0) {    // compute with finite differences        
        for (i=1; i<F.size()-1; i++)  {             
            gi=(F[i+1].r(f)-F[i-1].r(f))/(F[i+1].r(x)-F[i-1].r(x));
            F[i].w(dfdx,gi);
        }
        return true;
    }
    

    // Compute using Savitsky-Golay smoothing
    VODP C(NP);            // array with Savitzky-Golay coefficients
    
    // Call the routine for Savitsky-Golay smoothing, output coefficients in array C
    savgol(C, NP, NL, NR, LD, M); 
        
    for (i=0; i<NL; i++) { F[i].w(dfdx,0.0); }      // first points
    for (i=NL; i<F.size()-NR; i++) {
        gi=0.0;
        for (n=-NL; n<=0; n++) gi += C[-n]*F[i+n].r(f);
        for (n=1; n<=NR; n++) gi += C[NP-n]*F[i+n].r(f);                  
        F[i].w(dfdx,gi);
    }
    for (i=F.size()-NR; i<F.size(); i++) { F[i].w(dfdx,0.0); }  // last points
    
    return true;
    
}

/*
bool QHA::write_qha_set(ostream& os, DP P)
{
    if (!calc) return false;
    
    string f; // auxiliary string
    
    // First consider the EvsV data (0 K static energies) to get reference values of the energy
    if (id=="") f="EvsV.in";
    else f=id+"EvsV.in";  
    EV Estat=get(f);
    if (!Estat.fitted()) Estat.fit();
    DP E0=Estat.E_P(P);
    DP H0=Estat.H_P(P);
    

    // set the stream
    os.precision(12);
    os<<scientific;
    // print headers
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    os<<"Quasi-harmonic approximation for "<<id<<endl;
    os<<"P= "<<P<<" Pa\t\tEOS: Murnaghan"<<endl;
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    os<<"Reference values from 0 K static energies "<<id<<endl;
    os<<"E_st(P)= "<<Estat.E_P(P)<<" J/mol\tV(P)= "<<Estat.V_P(P)<<
            " m^3/mol\tG_st(P)=H_st(P)= "<<Estat.H_P(P)<<" J/mol"<<endl;  
    os<<"B0= "<<Estat.B0()<<" Pa\tB0'= "<<Estat.B0p()<<" Chi^2/n= "<<
            Estat.chisq()<<" J/mol"<<endl;    
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;   
    os<<"X "<<setw(4)<<"T"<<setw(25)<<"Fvib(T,P)-Est(0,P)"<<setw(25)<<"V(T,P)"<<setw(25)<<
            "Hvib(T,P)-Hst(0,P)"<<setw(25)<<"B0(T,P)"<<setw(25)<<"Chi^2/n"<<endl;
    os<<"X "<<setw(4)<<"K"<<setw(25)<<"J/mol"<<setw(25)<<"m^3/mol"<<setw(25)<<
            "J/mol"<<setw(25)<<"Pa"<<setw(25)<<" J/mol"<<endl;
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
     // then loop over the T vector and read the FvsV data (vibrational energies at T K)
    if (id=="") f="FvsV";
    else f=id+"FvsV"; 
    for (auto t:T) {
        string ftemp=f;
        ftemp=ftemp+"."+t+".in";
        EV Fvib=get(ftemp);
        if (!Fvib.fitted()) Fvib.fit();
        
        os<<setw(6)<<t<<setw(25)<<Fvib.E_P(P)-E0<<setw(25)<<Fvib.V_P(P)<<setw(25)<<
                Fvib.H_P(P)-H0<<setw(25)<<Fvib.B0()<<setw(25)<<Fvib.chisq()<<endl;
    }   
    
    os<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        <<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    
}
   
*/