/* 
 * File:   phase.cpp
 * Author: Mauro Palumbo
 *
 * Created on March 15, 2015, 2:55 PM
 * 
 * Read phase data info 
 */

#include "phases.h"


void Vec_par::push_back(TPfunction f,int ord)
{
    while (size()<ord)
        vector::push_back(TPfunction(""));    // insert an empty TPfunction for missing parameters in between
    vector::push_back(f);
}


vector<string> generate_endmembers(vector<List_elements> x) 
{
    vector<string> temp;

    // Count total number of endmembers
    int sub,n_endmembers=1;    
    for (sub=0; sub<x.size(); sub++) 
        n_endmembers*=x[sub].size();

    // For testing only    
//    cout<<"Input number of sublattice:  "<<x.size()<<endl;
//    cout<<"N. endmembers="<<n_endmembers<<endl;
    
    // Initializations
    string endmember;                    // keep temporary endmember  
    vector<int> v_j;                    // auxiliary vector to generate configurations, keep trace of the elements already considered
    for (sub=0; sub<x.size(); sub++) 
        v_j.push_back(0);               // initialize with all zeros, no elements in any subattice has beeen considered yet   
    
    // Loop to generate configurations
    sub=0;
    for (int k=0; k<n_endmembers; ) {                    // k count current endmember
        endmember+=(x[sub])[v_j[sub]].symbol()+":";      // add the current element to conf
        sub++;
        if (sub==x.size()) {                            // finished generating one full endmember...
            v_j[sub-1]++;                               // in last sublattice do not consider anymore elements already done  
            while (v_j[sub-1]==x[sub-1].size()) {       // all elements in the sublattice sub-1 have been considered, move on updating the previous one
                if (k==(n_endmembers-1)) break;         // avoid update when finished. Otherwise pointers will be bad
                v_j[sub-1]=0;                           // reset to 0 the "considered" elements in sublattice sub-1
                sub--;                                  // move to the previous sublattice
                v_j[sub-1]++;                           // in current sublattice sub-1 do not consider anymore elements already done  
            }
            endmember=trim(endmember,":");   // remove last : from current endmember
            temp.push_back(endmember);      // store endmember in temporary vector
            endmember="";                   // reset current endmember
            sub=0;                          // reset current sublattice
            k++;                            // updated current endmember counter
        }
    }
  
    return temp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Phase::Phase(string phase_def, string constituent_def, List_types typ)
     : Structure(phase_def)
{
    // set all characters to upright
    phase_def=to_up(phase_def);
    constituent_def=to_up(constituent_def);
    
    // remove eventually exclamation mark
    phase_def=trim(phase_def,"!");
    constituent_def=trim(constituent_def,"!");
    
    // start processing the input string (can be just the element name or the full definition)
    stringstream sp(phase_def);
    string command, name, types;

    sp>>command;
    command=get_subcommand_extended(command,"PHASE");
    if (command=="PHASE") {
        sp>>name>>types>>ns;      // read full format             
        nm=0;
        for (int i=0; i<ns; i++)  {       // read multiplicities
            DP temp;
            sp>>temp;
            ms.push_back(temp);
            nm+=temp;
        }
        
        // Process TYPE_DEFINITIONs
        for (int i=0; i<types.size(); i++)  {
            Type temp_type=typ.get(types[i]);
            t.insert(temp_type);    // store them
            // for now only only process TYPE_DEFINITIONs as GES A_P_D BCC_A2 MAGNETIC  -1.0    4.00000E-01 
            if (temp_type.size()==7 && temp_type[1]=="GES" && 
                    get_subcommand_extended(temp_type[2],"AMEND_PHASE_DESCRIPTION")!="" &&       // check if the command is an abbreviation of AMEND_PHASE_DESCRIPTION
                    temp_type[3]==remove_option(name) &&
                    get_subcommand_extended(temp_type[4],"MAGNETIC")!="") {    // set magnetic parameters in Inden model 
                AFM=str_to_double(temp_type[5]);     // antiferromagnetic factor (-1.0 for bcc phases, -3.0 for fcc/hcp phases)
                if (AFM==-1.0) { D=1.55828482;  p=str_to_double(temp_type[6]); }  // for bcc phases 
                if (AFM==-3.0) { D=2.342456517; p=str_to_double(temp_type[6]); }  // for fcc and other phases 
            }
        }
        
        option=get_option(name);   // store eventually option :F or :B
        cout<<"Option: "<<option<<endl;
        // Now process constituent_def
        stringstream sc(constituent_def);
        string command2, name2, constituents, temp;
        
        sc>>command2>>name2;
        command2=get_subcommand_extended(command2,"CONSTITUENT");
        if (command2!="CONSTITUENT") cerr<<"Error: expected command CONSTITUENT in phase constructor"<<endl;
  //      if (name2!=name) cerr<<"Error: PHASE and CONSTITUENT commands must refer to the same phase in phase constructor"<<endl;
        while (sc>>temp)
            constituents+=temp+" ";    // put the constituents list in string constituents
        
      
        // Now browse the string constituents to separate between : and ,
        vector<string> colon_separated;        
        colon_separated=separate(constituents,": ");
        
        cout<<"ns: "<<ns<<" Size colon_separated: "<<colon_separated.size()<<endl;
        cout<<"Constituents: "<<constituents<<endl;
        if (ns!=colon_separated.size()) cerr<<"The number of sublattices in PHASE and CONSTITUENT does not match in phase "<<name<<endl;
        
        // loop over sublattices (colon_separated)
        for (int i=0; i<colon_separated.size(); i++) {
            vector<string> comma_separated;
            comma_separated=separate(colon_separated[i],", ");
            
            // loop over elements listed between , to create an element list "el_list"
            List_elements el_list;
            for (int j=0; j<comma_separated.size(); j++) 
                el_list.insert(Element(trim(comma_separated[j])));
            
            elsub.push_back(el_list);    // put elements in el_list in sublattice i
         }
        initialize();
    }   
}

bool Phase::check_conf(const Configuration& c)
{   
        
    try {
      if (c.size()!=ns) throw too_many_or_too_few_components();
      if (to_up(c.to_string())=="UNKNOWN") throw unknown_structure();
      else
        for (int i=0; i<c.size(); i++)
            for (int j=0; j<c[i].size(); j++)
          if (!elsub[i].is(c[i][j])) throw no_such_element_in_phase();
    }
    catch (unknown_structure) 
      { cerr<<"Unknown structure in ab initio file!\n"<<endl; return false;}
    catch (too_many_or_too_few_components) 
      { cerr<<"too many or too few components!\n"<<endl; return false;}
    catch (no_such_element_in_phase) 
      { cerr<<"no such element in phase!\n"<<endl; return false;}        
    return true;
}

string handle_par(string par, string& definition, string& conf, int& order, string& kind)
{
        // set all characters to upright
    par=to_up(par);
    
    // start processing the input string (can be just the element name or the full definition)
    stringstream ss(par);
    string command;
    
    ss>>command>>definition;   // first input string should be the command "PARAMETER" 
                               // the second input string should be the definition, for example G(bcc,al:ni;0),
    command=get_subcommand_extended(command,"PARAMETER");
    if (command!="PARAMETER") throw wrong_parameter_command {};
    size_t n=par.find("PARAMETER");
    if (n!=par.size()) par.erase(n,n+9);
            
    
    // Handle the definition
    conf=trim(definition);   // trim whitespaces and \t\n\r
       
    vector<string> what=separate(conf,"(");   // what[0] is G or TC or BMAGN (and eventually more options)
    kind=what[0];                             // put what[0] in kind 
    
    conf=trim(what[what.size()-1],"()");   // trim parenthesis only for the second part of what
    
    vector<string> semicolon_separated=separate(conf,";");   // separate by ; the last element should be the order
                                           // if this parameter
    
    order=str_to_int(semicolon_separated[semicolon_separated.size()-1]);  // get the order of the parameter, last of temp
    if (order<0) throw wrong_parameter_order {};
    
    vector<string> comma_separated=separate(semicolon_separated[0],",");     // separate by comma to get the first element
    string phase_name=comma_separated[0];                    // which should be the phase name
    
    conf.clear();                                            // reuse s for storing the configuration
    for (int i=1; i<comma_separated.size(); i++)             // put back together in s the "," separated except the first one
        conf+=comma_separated[i]+",";
    if (comma_separated.size()>1) conf.erase(conf.size()-1,conf.size());  // erase last , if more than one element
    
    definition=par;   // put in definition the full parameter definition (except for the PARAMETER command)
    return phase_name;
}

string get_option(string name)
{
    vector<string> colon_separated=separate(name,":");     // separate by comma to get the first element
    string opt=colon_separated[colon_separated.size()-1];                    // which should be the phase name
    if (opt=="F"||opt=="B"||opt=="L") return ":"+opt;            
    else return "";
}

string remove_option(string name)
{
    vector<string> colon_separated=separate(name,":");     // separate by comma to get the first element
    return colon_separated[0];            
}


void Phase::set_parameter(const string& par)
{
    string phase_name, parameter_definition, configuration, kind;
    int order {0};
    phase_name=handle_par(par, parameter_definition, configuration, order, kind);    
    if (option!="") phase_name+=option;   // eventually add option :F or :B
    
    try {
//        cout<<"Phase name: "<<phase_name<<" from Structure: "<<this->get(Structure::name,-1)<<endl;    
//        cout<<"Configuration: "<<configuration<<endl;
//        cout<<"Order: "<<order<<"\t Kind: "<<kind<<endl;
//        cout<<"Parameter definition: "<<parameter_definition<<endl;
        
        if (phase_name!=this->get(Structure::name,-1)) throw no_such_phase {};
        if (!check_conf(configuration)) throw wrong_configuration {};
        
        if (kind=="G") {
            if ((order==0)&&Configuration(configuration).is_endmember())               //  put it in G_ref
                G_ref[configuration]=TPfunction("FUNCTION "+parameter_definition);     
                                    // once done with string processing, create a TPfunction from more and store it in G_ref
                                    // the definition will be the name of the TPfunction even if it will not be used
                                    // The TPfunction constructor handles the creation of one or more ranges
            
            else if ((order!=0)&&Configuration(configuration).is_endmember()) throw wrong_parameter_order {};
            else {                      // if the parameter order is >0, put it in G_ex
                Vec_par temp;
            
                auto it=G_ex.find(configuration);
                if (it!=G_ex.end())
                    temp=G_ex.at(configuration);
            
                temp.push_back(TPfunction("FUNCTION "+parameter_definition),order);
                G_ex[configuration]=temp;
            }
        }
        else if (kind=="TC") {
            if ((order==0)&&Configuration(configuration).is_endmember())               //  put it in TC_ref
                TC_ref[configuration]=TPfunction("FUNCTION "+parameter_definition);     
                                    // once done with string processing, create a TPfunction from more and store it in TC_ref
                                    // the definition will be the name of the TPfunction even if it will not be used
                                    // The TPfunction constructor handles the creation of one or more ranges
            
            else if ((order!=0)&&Configuration(configuration).is_endmember()) throw wrong_parameter_order {};
            else {                      // if the parameter order is >0, put it in TC_ex
                Vec_par temp;
            
                auto it=TC_ex.find(configuration);
                if (it!=TC_ex.end())
                    temp=TC_ex.at(configuration);
            
                temp.push_back(TPfunction("FUNCTION "+parameter_definition),order);
                TC_ex[configuration]=temp;
            }
        }   
        else if (kind=="BMAGN") {
            if ((order==0)&&Configuration(configuration).is_endmember())               //  put it in BMAGN_ref
                BMAGN_ref[configuration]=TPfunction("FUNCTION "+parameter_definition);     
                                    // once done with string processing, create a TPfunction from more and store it in BMAGN_ref
                                    // the definition will be the name of the TPfunction even if it will not be used
                                    // The TPfunction constructor handles the creation of one or more ranges
            
            else if ((order!=0)&&Configuration(configuration).is_endmember()) throw wrong_parameter_order {};
            else {                      // if the parameter order is >0, put it in BMANG_ex
                Vec_par temp;
            
                auto it=BMAGN_ex.find(configuration);
                if (it!=BMAGN_ex.end())
                    temp=BMAGN_ex.at(configuration);
            
                temp.push_back(TPfunction("FUNCTION "+parameter_definition),order);
                BMAGN_ex[configuration]=temp;
            }
        }
    }
    catch(...) {}   // to be revised...
}
 

/*
void Phase::get_binary_configurations(const Configuration& e, vector<Configuration>& bin_conf, vector<DP>& weights)
{
    // create a set of unique elements contained in the configuration
    int i;
    set<string> temp;
    for (i=0; i<e.size(); i++)
          temp.insert(e[i].symbol());

    set<string>::iterator it, it2;
    for (it=temp.begin(); it!=temp.end(); it++)    // loop over the component to change
        for (it2=temp.begin(); it2!=temp.end(); it2++)  { // loop over the component to change into
            Configuration temp2=e;
            vector<bool> t;
            if (*it!=*it2) {
              t=temp2.flip(*it,*it2);      // flip the component *it with the componenent *it2
              bin_conf.push_back(temp2);    // put the flipped configuration in vconf
              // compute the weight for this flipping... using the vector t
              DP weight=0;
              for (i=0; i<t.size(); i++)
                  if (t[i]) weight+=ms[i];        // if the i element was flipped, count its multiplicity
              weights.push_back(nm/weight);
 //             cout<<temp2<<"\t"<<(this->get_E_tot(temp2)/30)<<"\t"<<weight<<endl;
            }
        }    
}

DP Phase::estimate_ternary_E_tot(const Configuration& c)
{
    vector<Configuration> bin_conf;
    vector<DP> weights;
    this->get_binary_configurations(c,bin_conf,weights);
    
    DP sum_E_tot=0, sum_weights=0;
    for (int i=0; i<bin_conf.size(); i++) {
//        cout<<bin_conf[i]<<"\t"<<weights[i]<<"\t"<<this->get_E_tot(bin_conf[i])/this->nmol()<<endl;
        sum_E_tot+=weights[i]*this->get_E_tot(bin_conf[i])/30;
        sum_weights+=weights[i];
    }    
    return sum_E_tot/sum_weights;
}

DP Phase::estimate_ternary_G_ref(const Configuration& c)
{
    vector<Configuration> bin_conf;
    vector<DP> weights;
    this->get_binary_configurations(c,bin_conf,weights);
    
    DP sum_E_tot=0, sum_weights=0;
    for (int i=0; i<bin_conf.size(); i++) {
 //       cout<<bin_conf[i]<<"\t"<<weights[i]<<"\t"<<this->get_E_tot(bin_conf[i])/this->nmol()<<endl;
        sum_E_tot+=weights[i]*this->get_E_tot(bin_conf[i])/30;
        sum_weights+=weights[i];
    }    
    return (this->get_G_ref(c)/this->nmol()+(sum_E_tot/sum_weights-this->get_E_tot(c)/this->nmol())*96485);
}


/*
ostream& operator<<(ostream& os, const Phase& p)
{
    // Structure part
    os<<p.get(Structure::name,-1)<<"\t"<<p.get(Structure::prototype,-1)<<"\t"<<p.get(Structure::Pearson,-1)
     <<"\t"<<p.get(Structure::struk,-1)<<"\t"<<p.get(Structure::spacegroup,-1)<<"\t"<<p.get(Structure::spacegroupnum,-1)
     <<"\t"<<p.get(Structure::nWyck,-1)<<"\t"<<p.get(Structure::Wyck,-1)<<"\n";
   return os<<p.nsub()<<"\t"<<"\n";
}
*/


void Phase::initialize()
{
    // Just for testing...
    cout<<"Initializing phase "<<get(Structure::name,-1)<<"..."<<endl;

    // Initialize endmember parameters to 0 
    if (G_ref.size()==0) {              // if !=0, initialization already done
        cout<<"Initializing endmembers..."<<endl;
        vector<string> endmembers=generate_endmembers(elsub); 
        for (auto x : endmembers) {
            cout<<"Initializing "<<x<<"..."<<endl;
            set_parameter("PARAMETER G("+get(Structure::name,-1)+","+x+";0)  2.98150E+02  0;  6.00000E+03   N REF0 !");
        }
    }
       
    // Initialize site fractions
    if (y.size()==0) {                   // if !=0, initialization already done
        cout<<"Initializing site fractions..."<<endl;
        for (int sub=0; sub<ns; sub++) {
            List_Y temp;
            for (int el=0; el<((elsub[sub]).size()-1); el++) {                         // loop over all elements but the last
                temp.insert((elsub[sub])[el].symbol(),1e-12);                          // set the site fractions to 0
                cout<<"Setting sublattice "<<sub<<" element "<<(elsub[sub])[el].symbol()<<"= 1E-12"<<endl;      
            }
            temp.insert((elsub[sub])[(elsub[sub]).size()-1].symbol(),1.0);
            cout<<"Setting sublattice "<<sub<<" element "<<(elsub[sub])[(elsub[sub]).size()-1].symbol()<<"= 1.0"<<endl;   
            y.push_back(temp);
        }
    }
    
}

ostream& operator<<(ostream& os, const Phase& p)
{
   // Structure part
    
    // Eventually print TYPE_DEFINITIONs for this phase
    for (auto x : p.get_types())  
        if (x.size()==7 && x[3]==p.get(Structure::name,-1))
            x.print_TDB(os);

    // Phase definition
   os<<" PHASE "<<p.get(Structure::name,-1);
   if ((p.get(Structure::name,-1)=="bcc")) os<<":B ";
   else if ((p.get(Structure::name,-1)=="fcc")||
      (p.get(Structure::name,-1)=="hcp"))  os<<":F ";
   os<<" "<<p.get_types().list()<<" "<<p.nsub()<<" ";
   for (int i=0; i<p.nsub(); i++)
       os<<fixed<<p.msub(i)<<" ";
   for (int i=0; i<p.nWyckoff(); i++)
       os<<p.get(Structure::Wyck,i)<<" ";
   os<<"!\n";
    // Constituent definition
   os<<"   CONSTITUENT  "<<p.get(Structure::name,-1);
   if (p.get(Structure::name,-1)=="bcc") os<<":B ";
   else if ((p.get(Structure::name,-1)=="fcc") ||
      (p.get(Structure::name,-1)=="hcp")) os<<":F ";
   os<<" :";
   for (int i=0; i<p.nsub(); i++)
       os<<p.get_elements_in_sub(i).list()<<":";
   os<<" !\n";
   
   // Parameters from G_ref (order 0)
   for (auto it=p.get_G_ref().begin(); it!=p.get_G_ref().end(); it++) 
      os<<"PARAMETER G("<<remove_option(p.get(Structure::name,-1))<<","<<((it->first).to_string())
              <<";0) "<<it->second;  
   
    // Parameters from G_ex (order >0)  
    for (auto it=p.get_G_ex().begin(); it!=p.get_G_ex().end(); it++) 
        for (int i=0; i!=(it->second).size(); i++)
            os<<"PARAMETER G("<<remove_option(p.get(Structure::name,-1))<<","<<((it->first).to_string())
              <<";"<<i<<") "<<(it->second)[i];  
              
    // TC parameters   
    for (auto it=p.get_TC_ref().begin(); it!=p.get_TC_ref().end(); it++) 
        os<<"PARAMETER TC("<<remove_option(p.get(Structure::name,-1))<<","<<((it->first).to_string())
              <<";0) "<<it->second;  
              
    for (auto it=p.get_TC_ex().begin(); it!=p.get_TC_ex().end(); it++) 
        for (int i=0; i!=(it->second).size(); i++)
            os<<"PARAMETER TC("<<remove_option(p.get(Structure::name,-1))<<","<<((it->first).to_string())
              <<";"<<i<<") "<<(it->second)[i];  
              
    // BMAGN parameters   
    for (auto it=p.get_BMAGN_ref().begin(); it!=p.get_BMAGN_ref().end(); it++) 
        os<<"PARAMETER BMAGN("<<remove_option(p.get(Structure::name,-1))<<","<<((it->first).to_string())
              <<";0) "<<it->second;  
              
    for (auto it=p.get_BMAGN_ex().begin(); it!=p.get_BMAGN_ex().end(); it++) 
        for (int i=0; i!=(it->second).size(); i++)
            os<<"PARAMETER BMAGN("<<remove_option(p.get(Structure::name,-1))<<","<<((it->first).to_string())
              <<";"<<i<<") "<<(it->second)[i];  
              
   return os;
}

/*
void print_estimates(ostream& os, const Phase& p)
{
   Phase pp=p;
   map<Configuration, DP, Phase::Configuration_order_alphabetic>::const_iterator it;
   for (it=p.get_G_ref().begin(); it!=p.get_G_ref().end(); it++) {
       Configuration temp=it->first;
       if (temp.is_ternary())
           os<<(temp.to_string())<<" & "<<(pp.get_E_tot(it->first)/pp.nmol())<<" & "
                   <<pp.estimate_ternary_E_tot(it->first)<<" & "<<((it->second)/pp.nmol()/1000)
                   <<" & "<<pp.estimate_ternary_G_ref(it->first)/1000<<" & "
                   <<((it->second)/pp.nmol()-pp.estimate_ternary_G_ref(it->first))/1000<<" \\\\"<<endl; 
   }
}

*/

DP Phase::calc_deriv_ref_type(const G_ref_type& ref, List_TPfunctions& list, const DP T, const DP P, int sub, string deriv, int sub2, string deriv2) const
{
    if (deriv2!="" && sub==sub2) return 0.0;  // second derivative on the same sublattice always zero
    
    DP G_temp=0.0;
    
    // Second derivative
    if (deriv2!="") {
        for (auto it=ref.begin(); it!=ref.end(); it++) {    // loop over all endmembers
           DP y_product=1;
           bool is_not_zero=false;
           bool is_not_zero2=false;
           
           // Evaluate the y product for this endmember
           for (int i=0; i<ns; i++) 
               if (((it->first)[i][0].symbol()==deriv)&&(i==sub)) is_not_zero=true;
               else if (((it->first)[i][0].symbol()==deriv2)&&(i==sub2)) is_not_zero2=true;
               else y_product*= get_Y((it->first)[i][0].symbol(),i);   // compute the y product term (since it is after else, skip the site fraction with respect to we are derivating)
           
        if (is_not_zero && is_not_zero2) G_temp+=(it->second).calc(list,T,P)*y_product;         
        }        
        return G_temp;
    }
    
    // First derivative
    for (auto it=ref.begin(); it!=ref.end(); it++) {    // loop over all endmembers
        DP y_product=1;
        bool is_not_zero=false;
           
        // Evaluate the y product for this endmember
        for (int i=0; i<ns; i++) 
            if (((it->first)[i][0].symbol()==deriv)&&(i==sub)) is_not_zero=true;   // check if the site fraction with respect to we are derivating is in here... if so, derivative not zero
            else y_product*= get_Y((it->first)[i][0].symbol(),i);                  // compute the y product term (since it is after else, skip the site fraction with respect to we are derivating)
            
        if (is_not_zero) G_temp+=(it->second).calc(list,T,P)*y_product;
    }
    return G_temp;
}

DP Phase::calc_ref_type(const G_ref_type& ref, List_TPfunctions& list, const DP T, const DP P, int sub, string deriv) const
{
    DP G_temp=0.0;
   for (auto it=ref.begin(); it!=ref.end(); it++) {    // loop over all endmembers
       // calculate the product of site fractions
       DP y_product=1;
       
       // Evaluate the y product for this endmember
       for (int i=0; i<ns; i++)                             // loop over the sublattices
            y_product*= get_Y((it->first)[i][0].symbol(),i);              // (it->first)[i] refers to the sublattice i in the configuration (it->first)
                                                                          // (it->first)[i][0] refers to the (single) element in the ith sublattice in the configuration (it->first)
       
       // Evaluate G_ref for this endmember and add it up to Gtemp
       G_temp+=(it->second).calc(list,T,P,deriv)*y_product;         // (it->second).calc(list,T,P,deriv,order) is the endmember energy (a TPfunction)
   }
   return G_temp;
}

DP Phase::calc_ex_type(const G_ex_type& ex, List_TPfunctions& list, const DP T, const DP P, int sub, string deriv) const
{
   DP G_temp=0.0;
   for (auto it=ex.begin(); it!=ex.end(); it++) {    // loop over all excess terms
       // calculate the product of site fractions
       DP y_product=1;
       for (int i=0; i<it->first.size(); i++) 
           for (int j=0; j<it->first[i].size(); j++) 
               y_product*= get_Y((it->first)[i][j].symbol(),i);
       
       G_temp+=(it->second)[0].calc(list,T,P,deriv)*y_product;                 // 0th order interaction term
       
       // higher order interaction terms
       for (int ord=1; ord<(it->second).size(); ord++) {                       // eventually loop over higher order interactions (order ordth)
           DP y_product2=1;
           for (int i=0; i<(it->first).size(); i++) 
               if ((it->first)[i].size()>1) {
                   DP diff=(get_Y((it->first)[i][0].symbol(),i)-get_Y((it->first)[i][1].symbol(),i));   // this is the difference yi-yj
                   y_product2*= pow(diff,ord);
               }
           G_temp+=(it->second)[ord].calc(list,T,P,deriv)*y_product2*y_product; 
       }
   } 
   return G_temp;
}

DP Phase::calc_deriv_ex_type(const G_ex_type& ex, List_TPfunctions& list, const DP T, const DP P, int sub, string deriv, int sub2, string deriv2) const
{
    DP G_temp=0.0;
    
        // Second derivative
    if (deriv2!="") {
        for (auto it=ex.begin(); it!=ex.end(); it++) {    // loop over all endmembers
            DP y_product {1}, dy_product {1};
            bool is_not_zero=false;
            bool is_not_zero2=false;
        
            // Evaluate the y product for this endmember
            for (int i=0; i<ns; i++) {
                for (int j=0; j<it->first[i].size(); j++) {
                    y_product*= get_Y((it->first)[i][j].symbol(),i);
                    if (((it->first)[i][j].symbol()==deriv)&&(i==sub)) is_not_zero=true;   // check if the site fraction with respect to we are derivating is in here... if so, derivative not zero
                    else if (((it->first)[i][j].symbol()==deriv2)&&(i==sub2)) is_not_zero2=true;
                    else dy_product*= get_Y((it->first)[i][j].symbol(),i);                  // compute the y product term (since it is after else, skip the site fraction with respect to we are derivating)
                }
            }
            
//           cout<<"Endmember "<<(it->first).to_string()<<" is not zero? "<<is_not_zero<<" yproduct= "<<y_product<<" G0="<<(it->second)[0].calc(list,T,P)<<endl;
            if (is_not_zero && is_not_zero2) G_temp+=(it->second)[0].calc(list,T,P,deriv)*dy_product;                 // 0th order interaction term
    
            // higher order interaction terms
            for (int ord=1; ord<(it->second).size(); ord++) {                       // eventually loop over higher order interactions (order ordth)
                DP y_product2 {1}, dy_product2 {1};
                for (int i=0; i<(it->first).size(); i++)
                    if ((it->first)[i].size()>1) {
                        y_product2*=pow(get_Y((it->first)[i][0].symbol(),i)-get_Y((it->first)[i][1].symbol(),i),ord);
                        if (((it->first)[i][0].symbol()==deriv)&&(i==sub))    // check if the site fraction with respect to we are derivating is in here... if so, derivative not zero
                            dy_product2*= ord*pow(get_Y((it->first)[i][1].symbol(),i),ord-1);
                        else if ((((it->first)[i][1].symbol()==deriv))&&(i==sub))
                            dy_product2*= ord*pow(-get_Y((it->first)[i][1].symbol(),i),ord-1);
                    }
        
            G_temp+=(it->second)[ord].calc(list,T,P,deriv)*(dy_product*y_product2+y_product*dy_product2); 
            }
        }
        return G_temp;  
    }
    
    
    // First derivative     
    for (auto it=ex.begin(); it!=ex.end(); it++) {    // loop over all endmembers
        DP y_product {1}, dy_product {1};
        bool is_not_zero=false;
           
        // Evaluate the y product for this endmember
        for (int i=0; i<ns; i++) {
            for (int j=0; j<it->first[i].size(); j++) {
                y_product*= get_Y((it->first)[i][j].symbol(),i);
                if (((it->first)[i][j].symbol()==deriv)&&(i==sub)) is_not_zero=true;   // check if the site fraction with respect to we are derivating is in here... if so, derivative not zero
                else dy_product*= get_Y((it->first)[i][j].symbol(),i);                  // compute the y product term (since it is after else, skip the site fraction with respect to we are derivating)
            }
        }
            
//           cout<<"Endmember "<<(it->first).to_string()<<" is not zero? "<<is_not_zero<<" yproduct= "<<y_product<<" G0="<<(it->second)[0].calc(list,T,P)<<endl;
        if (is_not_zero) G_temp+=(it->second)[0].calc(list,T,P,deriv)*dy_product;                 // 0th order interaction term
           
        // higher order interaction terms
        for (int ord=1; ord<(it->second).size(); ord++) {                       // eventually loop over higher order interactions (order ordth)
            DP y_product2 {1}, dy_product2 {1};
            for (int i=0; i<(it->first).size(); i++)
                if ((it->first)[i].size()>1) {
                    y_product2*=pow(get_Y((it->first)[i][0].symbol(),i)-get_Y((it->first)[i][1].symbol(),i),ord);
                    if (((it->first)[i][0].symbol()==deriv)&&(i==sub))    // check if the site fraction with respect to we are derivating is in here... if so, derivative not zero
                        dy_product2*= ord*pow(get_Y((it->first)[i][1].symbol(),i),ord-1);
                    else if ((((it->first)[i][1].symbol()==deriv))&&(i==sub))
                        dy_product2*= ord*pow(-get_Y((it->first)[i][1].symbol(),i),ord-1);
                }
               
            G_temp+=(it->second)[ord].calc(list,T,P,deriv)*(dy_product*y_product2+y_product*dy_product2); 
        }
    }
    return G_temp;      
}


DP Phase::calc_G_ref(List_TPfunctions& list, const DP T, const DP P, int sub, string deriv, int sub2, string deriv2) const
{  
   if (deriv!="" && deriv!="T" && deriv!="P") {     // derivative with respect to site fraction y of sublattice "sub" for element "deriv"   
       return calc_deriv_ref_type(G_ref,list,T,P,sub,deriv,sub2,deriv2);
   }
       
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // T, P derivative or simple G evaluation     
    return calc_ref_type(G_ref,list,T,P,sub,deriv);
}

DP Phase::calc_S_conf(List_TPfunctions& list, int sub, string deriv, int sub2, string deriv2) const
{
   DP temp1=0;
   if (deriv=="T"||deriv=="P") return 0;     // first derivative dy1 
   if (deriv!="" && deriv2!="" && sub==sub2 && deriv==deriv2) 
       return -R*ms[sub]*(1.0/y[sub].get(deriv).y());;    // second derivative dy1/dy1 
   if (deriv!="" && deriv2!="") return 0;    // second derivative dy1/dy2, always zero
   if (deriv!="" && deriv2=="") return -R*ms[sub]*(1+log(y[sub].get(deriv).y()));
   for (int i=0; i<ns; i++) {                   // loop over the sublattices
       DP temp2=0;
       for (int j=0; j<elsub[i].size(); j++)    // loop over the elements in a given sublattice
         temp2+=ylny(get_Y((elsub[i])[j].symbol(),i)); 
       temp1+=ms[i]*temp2;
   }
   return -R*temp1;
}

DP Phase::calc_G_ex(List_TPfunctions& list, const DP T, const DP P, int sub, string deriv, int sub2, string deriv2) const
{
   if (deriv!="" && deriv!="T" && deriv!="P")      // derivative with respect to site fraction y of sublattice "sub" for element "deriv"
       return calc_deriv_ex_type(G_ex,list,T,P,sub,deriv,sub2,deriv2);
   
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // T, P derivative or simple G evaluation    
   return calc_ex_type(G_ex,list,T,P,sub,deriv);
}

DP Phase::calc_TC(List_TPfunctions& list, const DP T, const DP P, int sub, string deriv, int sub2, string deriv2) const
{  
   DP TC = calc_ref_type(TC_ref,list,T,P)+calc_ex_type(TC_ex,list,T,P);
    
   if (deriv!="" && deriv!="T" && deriv!="P")      // derivative with respect to site fraction y of sublattice "sub" for element "deriv"   
       if (TC<0)
           return (calc_deriv_ref_type(TC_ref,list,T,P,sub,deriv,sub2,deriv2)
               +calc_deriv_ex_type(TC_ex,list,T,P,sub,deriv,sub2,deriv2))/AFM;
       else
           return calc_deriv_ref_type(TC_ref,list,T,P,sub,deriv,sub2,deriv2)
               +calc_deriv_ex_type(TC_ex,list,T,P,sub,deriv,sub2,deriv2);
       
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // simple TC evaluation 
   if (deriv=="" && TC<0) return TC/AFM;
   
   return 0;
}

DP Phase::calc_BMAGN(List_TPfunctions& list, const DP T, const DP P, int sub, string deriv, int sub2, string deriv2) const
{  
   DP BMAGN = calc_ref_type(BMAGN_ref,list,T,P)+calc_ex_type(BMAGN_ex,list,T,P);
   
   if (deriv!="" && deriv!="T" && deriv!="P")      // derivative with respect to site fraction y of sublattice "sub" for element "deriv"
       if (BMAGN<0)
           return (calc_deriv_ref_type(BMAGN_ref,list,T,P,sub,deriv,sub2,deriv2)
               +calc_deriv_ex_type(BMAGN_ex,list,T,P,sub,deriv,sub2,deriv2))/AFM;
       else
           return calc_deriv_ref_type(BMAGN_ref,list,T,P,sub,deriv,sub2,deriv2)
               +calc_deriv_ex_type(BMAGN_ex,list,T,P,sub,deriv,sub2,deriv2);
       
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // simple BMAGN evaluation
   if (deriv=="" && BMAGN<0) return BMAGN/AFM;
           
   return 0;
}

DP Phase::calc_Gmagn(List_TPfunctions& list, const DP T, const DP P, int sub, string deriv, int sub2, string deriv2) const
{  
   if (calc_TC(list,T,P)==0.0) return 0;
    
   if (deriv!="" && deriv!="T" && deriv!="P")      // derivative with respect to site fraction y of sublattice "sub" for element "deriv"   
       return R*T/(1.0+calc_BMAGN(list,T,P))*g_tau(T/calc_TC(list,T,P),p,D)*calc_BMAGN(list,T,P,sub,deriv,sub2,deriv2)
               -R*T/calc_TC(list,T,P)*T/calc_TC(list,T,P)*log(calc_BMAGN(list,T,P)+1.0)
               *dgdtau(T/calc_TC(list,T,P),p,D)*calc_TC(list,T,P,sub,deriv,sub2,deriv2);
       
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // T, P derivative or simple TC evaluation 
   return Gmagn(T,calc_TC(list,T,P),calc_BMAGN(list,T,P),p,D,deriv);
}

DP Phase::calc_G(List_TPfunctions& list, const DP T, const DP P, int sub, string deriv, int sub2, string deriv2) const
{
   if (deriv=="T")      // explicity distinguish the T derivative
       return calc_G_ref(list,T,P,sub,deriv,sub2,deriv2)-calc_S_conf(list)-calc_S_conf(list,sub,deriv,sub2,deriv2)
               +calc_G_ex(list,T,P,sub,deriv,sub2,deriv2)+calc_Gmagn(list,T,P,sub,deriv,sub2,deriv2);
   return calc_G_ref(list,T,P,sub,deriv,sub2,deriv2)-T*calc_S_conf(list,sub,deriv,sub2,deriv2)
           +calc_G_ex(list,T,P,sub,deriv,sub2,deriv2)+calc_Gmagn(list,T,P,sub,deriv,sub2,deriv2);
}

string Phase::calc(List_TPfunctions& list, DP T, DP P, DP nmol, vector<DP> yy)
{
    cout<<"In PHASE::calc"<<endl;
    cout<<"T= "<<T<<" P= "<<P<<" nmol= "<<nmol<<endl;
    cout<<"Input site fractions: "<<endl;
    for (auto it : yy)
        cout<<it<<" ";
    cout<<endl<<"N. sub.= "<<ns<<endl;

    // First set the site fractions in the phase as in input vector yy
    int yy_counter=0;
    DP last_site_fraction;  
    y.clear();                  // clear previous values    
    for (int sub=0; sub<ns; sub++) {
        List_Y temp;
        last_site_fraction=1.0;
        for (int el=0; el<((elsub[sub]).size()-1); el++) {                  // loop over all elements but the last
//            cout<<"Setting sublattice "<<sub<<" element "<<(elsub[sub])[el].symbol()<<"= "<<yy[yy_counter]<<endl;   
            temp.insert((elsub[sub])[el].symbol(),yy[yy_counter]);          // set the site fractions to input value
            last_site_fraction-=yy[yy_counter++];                           // update last_site_fraction and yy_counter
        }
//        cout<<"Setting sublattice "<<sub<<" element "<<(elsub[sub])[(elsub[sub]).size()-1].symbol()<<"= "<<last_site_fraction<<endl;   
        temp.insert((elsub[sub])[(elsub[sub]).size()-1].symbol(),last_site_fraction);      // set the last site fraction to 1-y(el1)-y(el2)-...
        y.push_back(temp);                  // put the list of site fractions for this sublattice in y
    }

    for (int sub=0; sub<ns; sub++) 
        cout<<"Sublattice "<<sub<<": "<<y[sub];    
    
    cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;    
    cout<<"G_ref= "<<calc_G_ref(list,T,P)<<"  dG_ref/dT= "<<calc_G_ref(list,T,P,0,"T",1)<<endl;
    cout<<"S_conf= "<<calc_S_conf(list)<<"  dS_conf/dT= "<<calc_S_conf(list,0,"T",1)<<endl;
    cout<<"G_ex= "<<calc_G_ex(list,T,P)<<"  dG_ex/dT= "<<calc_G_ex(list,T,P,0,"T",1)<<endl;
    cout<<"TC= "<<calc_TC(list,T,P)<<"  dTC/dT= "<<calc_TC(list,T,P,0,"T",1)<<endl;
    cout<<"BMAGN= "<<calc_BMAGN(list,T,P)<<"  dBMAGN/dT= "<<calc_BMAGN(list,T,P,0,"T",1)<<endl;
    cout<<"G_magn= "<<calc_Gmagn(list,T,P)<<"  dG_magn/dT= "<<calc_Gmagn(list,T,P,0,"T",1)<<endl;
    cout<<"G= "<<calc_G(list,T,P)<<"  dG/dT= "<<calc_G(list,T,P,0,"T",1)<<endl;
    
    cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    cout<<"G_ref derivatives:"<<endl;
    for (int sub=0; sub<ns; sub++)
        for (int el=0; el<(elsub[sub]).size(); el++) {
            cout<<"1st derivatives:"<<endl;
            cout<<"Sub. "<<sub<<"\t"<<(elsub[sub])[el].symbol()<<"\t"<<calc_G_ref(list,T,P,sub,(elsub[sub])[el].symbol())<<endl;
            cout<<"...2nd derivatives:"<<endl;
            for (int sub2=0; sub2<ns; sub2++)
                for (int el2=0; el2<(elsub[sub2]).size(); el2++)
                    cout<<"...Sub. "<<sub2<<"\t"<<(elsub[sub2])[el2].symbol()
                        <<"\t"<<calc_G_ref(list,T,P,sub,(elsub[sub])[el].symbol(),sub2,(elsub[sub2])[el2].symbol())<<endl;
        }
    cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    cout<<"S_conf derivatives:"<<endl;
    for (int sub=0; sub<ns; sub++)
        for (int el=0; el<(elsub[sub]).size(); el++) {
            cout<<"1st derivatives:"<<endl;
            cout<<"Sub. "<<sub<<"\t"<<(elsub[sub])[el].symbol()<<"\t"<<calc_S_conf(list,sub,(elsub[sub])[el].symbol())<<endl;
            cout<<"...2nd derivatives:"<<endl;
            for (int sub2=0; sub2<ns; sub2++)
                for (int el2=0; el2<(elsub[sub2]).size(); el2++)
                    cout<<"...Sub. "<<sub2<<"\t"<<(elsub[sub2])[el2].symbol()
                        <<"\t"<<calc_S_conf(list,sub,(elsub[sub])[el].symbol(),sub2,(elsub[sub2])[el2].symbol())<<endl;
        }
            
    cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    cout<<"G_ex derivatives:"<<endl;    
    for (int sub=0; sub<ns; sub++)
        for (int el=0; el<(elsub[sub]).size(); el++) {
            cout<<"1st derivatives:"<<endl;
            cout<<"Sub. "<<sub<<"\t"<<(elsub[sub])[el].symbol()<<"\t"<<calc_G_ex(list,T,P,sub,(elsub[sub])[el].symbol())<<endl;
            cout<<"...2nd derivatives:"<<endl;
            for (int sub2=0; sub2<ns; sub2++)
                for (int el2=0; el2<(elsub[sub2]).size(); el2++)
                    cout<<"...Sub. "<<sub2<<"\t"<<(elsub[sub2])[el2].symbol()
                        <<"\t"<<calc_G_ex(list,T,P,sub,(elsub[sub])[el].symbol(),sub2,(elsub[sub2])[el2].symbol())<<endl;
        }

    cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    cout<<"BMAGN derivatives:"<<endl;   
    for (int sub=0; sub<ns; sub++)
        for (int el=0; el<(elsub[sub]).size(); el++) {
            cout<<"1st derivatives:"<<endl;
            cout<<"Sub. "<<sub<<"\t"<<(elsub[sub])[el].symbol()<<"\t"<<calc_BMAGN(list,T,P,sub,(elsub[sub])[el].symbol())<<endl;
            cout<<"...2nd derivatives:"<<endl;
            for (int sub2=0; sub2<ns; sub2++)
                for (int el2=0; el2<(elsub[sub2]).size(); el2++)
                    cout<<"...Sub. "<<sub2<<"\t"<<(elsub[sub2])[el2].symbol()
                        <<"\t"<<calc_BMAGN(list,T,P,sub,(elsub[sub])[el].symbol(),sub2,(elsub[sub2])[el2].symbol())<<endl;
        }

    cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    cout<<"TC derivatives:"<<endl; 
    for (int sub=0; sub<ns; sub++)
        for (int el=0; el<(elsub[sub]).size(); el++) {
            cout<<"1st derivatives:"<<endl;
            cout<<"Sub. "<<sub<<"\t"<<(elsub[sub])[el].symbol()<<"\t"<<calc_TC(list,T,P,sub,(elsub[sub])[el].symbol())<<endl;
            cout<<"...2nd derivatives:"<<endl;
            for (int sub2=0; sub2<ns; sub2++)
                for (int el2=0; el2<(elsub[sub2]).size(); el2++)
                    cout<<"...Sub. "<<sub2<<"\t"<<(elsub[sub2])[el2].symbol()
                        <<"\t"<<calc_TC(list,T,P,sub,(elsub[sub])[el].symbol(),sub2,(elsub[sub2])[el2].symbol())<<endl;
        }
            
    cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    cout<<"G_magn derivatives:"<<endl;   
    for (int sub=0; sub<ns; sub++)
        for (int el=0; el<(elsub[sub]).size(); el++) {
            cout<<"1st derivatives:"<<endl;
            cout<<"Sub. "<<sub<<"\t"<<(elsub[sub])[el].symbol()<<"\t"<<calc_Gmagn(list,T,P,sub,(elsub[sub])[el].symbol())<<endl;
            cout<<"...2nd derivatives:"<<endl;
            for (int sub2=0; sub2<ns; sub2++)
                for (int el2=0; el2<(elsub[sub2]).size(); el2++)
                    cout<<"...Sub. "<<sub2<<"\t"<<(elsub[sub2])[el2].symbol()
                        <<"\t"<<calc_Gmagn(list,T,P,sub,(elsub[sub])[el].symbol(),sub2,(elsub[sub2])[el2].symbol())<<endl;
        }
            
    cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    cout<<"G derivatives:"<<endl;
    for (int sub=0; sub<ns; sub++)
        for (int el=0; el<(elsub[sub]).size(); el++) {
            cout<<"1st derivatives:"<<endl;
            cout<<"Sub. "<<sub<<"\t"<<(elsub[sub])[el].symbol()<<"\t"<<calc_G(list,T,P,sub,(elsub[sub])[el].symbol())<<endl;
            cout<<"...2nd derivatives:"<<endl;
            for (int sub2=0; sub2<ns; sub2++)
                for (int el2=0; el2<(elsub[sub2]).size(); el2++)
                    cout<<"...Sub. "<<sub2<<"\t"<<(elsub[sub2])[el2].symbol()
                        <<"\t"<<calc_G(list,T,P,sub,(elsub[sub])[el].symbol(),sub2,(elsub[sub2])[el2].symbol())<<endl;
        }
    
    return "";
}

/*
void Phase::add_vib(DP T, DP& maxF)
{
    maxF=-1e9;
    string phase=to_up(get(Structure::name,-1));
    cout<<"Adding Fvib to phase "<<phase<<"..."<<endl;
    for (auto it=G_ref.begin(); it!=G_ref.end(); it++) {
        if ((it->first).is_endmember()) {         // add vib only to endmembers
            DP Fvib {0};
            string conf {to_up((it->first).to_string_nocolons())};
            string parameter_definition {"G("+phase+","+(it->first).to_string()+";0) 298.15 "};
            
            if (get_Fvib(phase+"_"+conf,T,Fvib)) {
                Fvib=30.0*Fvib;
                if (Fvib>maxF) maxF=Fvib;
                cout<<"Conf. "<<conf<<" T= "<<T<<" K  Fvib= "<<Fvib<<" E0= "<<(it->second).calc(T)<<endl;
                Fvib=Fvib+(it->second).calc(T);
                parameter_definition+=to_string(Fvib)+"; 6000 N !";
            }
            else {
                cout<<"Conf. "<<conf<<" mechanically unstable!"<<endl;
                parameter_definition+=to_string((it->second).calc(T))+"+UNSTABLE; 6000 N !";
            }

            cout<<parameter_definition<<endl;
            G_ref[it->first]=TPfunction("FUNCTION "+parameter_definition); 
        }
    }
}
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Phase List_phases::get(const string ph)
{
    auto it=find(ph);
    if (it!=end()) return *it;
    else throw no_such_phase {};
}
    
bool List_phases::is(const Phase& ph) const
{
    if (find(ph)==end()) return false;
    return true;
}    

bool List_phases::is(const string ph) const
{
    if (find(*new Phase(ph))==end()) return false;
    return true;
} 

string List_phases::list() const
{
    string temp;
    for (auto it=begin(); it!=end(); it++) 
       temp+=(*it).name()+",";
    // remove last comma
    temp.erase(temp.size()-1,temp.size());
    return temp;
}

void List_phases::print_TDB(ostream& os)
{
    for (auto it=begin(); it!=end(); it++) 
       os<<*it<<endl;
}

void List_phases::set_G_ref(Phase& p, Configuration& c, DP g_ref)
{
    auto it=find(p);
    if (it!=end()) {
        Phase temp(*it);
//        temp.set_G_ref(c,g_ref);
        erase(it);
        insert(temp);
    }
}

void List_phases::set_G_ref(const string& p,const string& c, DP g_ref)
{
    auto it=find(*new Phase(p));
    if (it!=end()) {
        Phase temp(*it);
//        temp.set_G_ref(c,g_ref);
        erase(it);
        insert(temp);
    }
}


void List_phases::set_parameter(const string& par)
{
    string phase_name, parameter_definition, configuration, kind;
    int order {0};
    phase_name=handle_par(par, parameter_definition, configuration, order,kind);
    
    if (is(phase_name+":B")) phase_name+=":B";
    if (is(phase_name+":F")) phase_name+=":F";
    if (is(phase_name+":L")) phase_name+=":L";
    if (!is(phase_name)) throw no_such_phase {};
            
    Phase temp=get(phase_name);
    temp.set_parameter("PARAMETER "+parameter_definition);    
    erase(get(phase_name));     // erase old phase in the set of phases
    insert(temp);               // reinsert the modified phase
}

void List_phases::set_Y(const string& p, const string el, const int sub, const DP yy)
{
    auto it=find(*new Phase(p));
    if (it!=end()) {
        Phase temp(*it);
        temp.set_Y(el,sub,yy);
        erase(it);
        insert(temp);
    }
}

DP List_phases::get_Y(const string& p, const string el, const int sub) const
{ 
    auto it=find(*new Phase(p));
    if (it==end()) throw no_such_phase();
    return (*it).get_Y(el,sub);
//    return (*new Phase(*it)).get_Y(el,sub);
}

Y List_phases::get_Y_full(const string& p, const string el, const int sub) const
{ 
    auto it=find(*new Phase(p));
    if (it==end()) throw no_such_phase();
    return (*new Phase(*it)).get_Y_full(el,sub);
}
/*
void List_phases::add_vib(const string ph, DP T, DP& maxF)
{    
    auto it=find(ph);
    if (it!=end()) {
        Phase temp(*it);
        temp.add_vib(T,maxF);
        erase(it);
        insert(temp);
    }
    else throw no_such_phase {};     
}
*/

string List_phases::calc(List_TPfunctions& list, string ph, DP T, DP P, DP nmol, vector<DP> yy)
{
    // set all characters to upright
    ph=to_up(ph);
    
    cout<<"Full command in List_phases::calc: "<<ph<<endl;
    
    // start processing the input string (can be just the function name or the full command CALCULATE NAME T P nmol list_of_site_fractions
    stringstream sss(ph);
    string command;
    sss>>command;
    if (command=="CALCULATE") {     // full command
        sss>>command>>ph>>T>>P>>nmol;        // read full format, command here is =PHASE
        DP temp;
        while (sss>>temp)           // read site fractions (until the end of string)
            yy.push_back(temp);      // put them into y
    }   
    
    for (auto it : yy)
        cout<<it<<" ";
    cout<<endl;
    
    cout<<"Phase name: "<<ph<<endl;
    
    auto it=find(Phase(ph));
    if (it==end()) throw no_such_phase();
    
    Phase temp_ph {*it};
    cout<<temp_ph<<endl;
    string booo {temp_ph.calc(list,T,P,nmol,yy)};
    return booo;
}