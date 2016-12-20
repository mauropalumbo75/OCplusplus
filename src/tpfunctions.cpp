/* 
 * File:   tpfunctions.cpp
 * Author: Mauro Palumbo
 *
 * Created on January 20, 2015, 17:11 AM
 * 
 * 
 */

#include "../include/tpfunctions.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

string s1_to_s2(string s, string s1, string s2)
{
    if(s.empty())
        return "";
    size_t start_pos = 0;
    string temp;
    s2="("+s2+")";
    while((start_pos = s.find(s1)) != std::string::npos) { 
        if (((start_pos==0)||((start_pos>0)&&(!isalpha(s[start_pos-1]))))&&    // check the character before and after the string to substitute to make sure it is not part of the name of another function
          ((start_pos==s.size())||(((start_pos+s1.size())<std::string::npos) && (!isalpha(s[start_pos+s1.size()]))))) {
            s.replace(start_pos, s1.size(),s2);             // replace the string
            temp.append(s,0,start_pos+s2.size());           // append to temp 
            s.erase(0,start_pos+s2.size());
        }
        else {
            temp.append(s,0,start_pos+s1.size());          
            s.erase(0,start_pos+s1.size());
        }
    }
    temp.append(s,0,std::string::npos);           // append to rest of s  
    return temp;
}

string ln_to_log(string s)
{
    if(s.empty())
        return "";
    size_t start_pos = 0;
    string temp;
    while((start_pos = s.find("ln")) != std::string::npos) { 
        if (((start_pos==0)||(start_pos>0)&&(!isalpha(s[start_pos-1])))&&    // check the character before and after LN to make sure LN is not part of the name of another function
          ((start_pos+2)<std::string::npos) && (!isalpha(s[start_pos+2]))) {
            s.replace(start_pos, 2,"log");     // replace LN
            temp.append(s,0,start_pos+3);           // append to temp 
            s.erase(0,start_pos+3);
        }
        else {
            temp.append(s,0,start_pos+2);          
            s.erase(0,start_pos+2);
        }
    }
    temp.append(s,0,std::string::npos);           // append to rest of s  
    return temp;
}

string log_to_ln(string s)
{
    if(s.empty())
        return "";
    size_t start_pos = 0;
    string temp;
    while((start_pos = s.find("log")) != std::string::npos) { 
        if ((start_pos>0)&&(!isalpha(s[start_pos-1]))&&    // check the character before and after LN to make sure LN is not part of the name of another function
          ((start_pos+3)<std::string::npos) && (!isalpha(s[start_pos+3]))) {
            s.replace(start_pos, 3,"ln");           // replace LOG
            temp.append(s,0,start_pos+2);           // append to temp 
            s.erase(0,start_pos+2);
        }
        else {
            temp.append(s,0,start_pos+3);          
            s.erase(0,start_pos+3);
        }
    }
    temp.append(s,0,std::string::npos);           // append to rest of s  
    return temp;
}


string pow_to_C_style(string s)
{
    if(s.empty())
        return "";
    size_t start_pos = 0;
    while((start_pos = s.find("**")) != std::string::npos) 
        s.replace(start_pos, 2,"^");
    return s;
}

string pow_to_fortran_style(string s)
{
    if(s.empty())
        return "";
    size_t start_pos = 0;
    while((start_pos = s.find("^")) != std::string::npos) 
        s.replace(start_pos, 1,"**");
    return s;
}

////////////////////////////////////////////////////////////////////////////////
//
//  class Range 
//
////////////////////////////////////////////////////////////////////////////////

// Member function to print in TDB format
void Range::print_TDB(ostream& os)
{
   os<<scientific<<setw(5)<<pow_to_fortran_style(e)<<";  "<<setw(5)<<Th;        
}

void Range::print_TDB(ostream& os) const
{
   os<<scientific<<setw(5)<<pow_to_fortran_style(e)<<";  "<<setw(5)<<Th;        
}

ostream& operator<<(ostream& os, const Range& r)
{
    return os<<scientific<<setw(5)<<r.expr()<<";  "<<setw(5)<<r.Thigh();  
}

////////////////////////////////////////////////////////////////////////////////
//
//  class TPfunction
//
////////////////////////////////////////////////////////////////////////////////

TPfunction::TPfunction(string name, string expr, DP Tlow, DP Thigh, string rref)
{
    // set all characters to upright
    name=to_up(name);
    
    // start processing the input string (can be just the function name or the full definition)
    stringstream ss(name);
    string command,more;  
    ss>>command;
    command=get_subcommand_extended(command,"FUNCTION");
    if (command=="FUNCTION") {      // assume only the string ss with the full definition is provided
        ss>>name>>Tlow>>expr>>Thigh>>more;   // read full format
        n=name;  Range temp(expr,Tlow,Thigh);   r.push_back(temp); 
        while (more=="Y") {     // read more T ranges
            Tlow=Thigh;               // next range lower limit start from previous higher limit
            ss>>expr>>Thigh>>more;   // read full format
            Range temp(expr,Tlow,Thigh);   r.push_back(temp); 
        }
        if (more=="N") {                             // after N there can be a reference tag 
           ss>>more; if (more!="!"&&more!="N") rr=more;        // if the last "more" string is not N, then it should be a REF tag 
        } 
    } else {        // all parameters in the constructor...
        n=to_up(name);
        rr=to_up(rref);
        Range temp(to_up(expr),Tlow,Thigh);
        r.push_back(temp);    
    }   
}
    
    
// Member function to print in TDB format
void TPfunction::print_TDB(ostream& os)
{
   os<<" FUNCTION  "<<n<<"\t"<<scientific<<setw(5)<<r[0].Tlow();  
   for (int i=0; i<r.size(); i++) {
       os<<"\t";
       r[i].print_TDB(os);
       if (i<(r.size()-1)) os<<" Y \n";
       else os<<" N "<<rr<<" ";
   }
   os<<"!\n";        
}

void TPfunction::print_TDB(ostream& os) const
{
   os<<" FUNCTION  "<<n<<"  "<<scientific<<setw(5)<<r[0].Tlow();  
   for (int i=0; i<r.size(); i++) {
       os<<"  ";
       r[i].print_TDB(os);
       if (i<(r.size()-1)) os<<" Y \n";
       else os<<" N "<<rr<<" ";
   }
   os<<"!\n";        
}

ostream& operator<<(ostream& os, const TPfunction& tp)
{
   os<<"  "<<scientific<<setw(5)<<tp.range(0).Tlow();  
   for (int i=0; i<tp.size(); i++) {
       os<<"  ";
       os<<tp.range(i);
       if (i<(tp.size()-1)) os<<" Y \n";
       else os<<" N "<<tp.ref()<<" ";
   }
   os<<"!\n";
   return os;
}

string TPfunction::get_expression(DP T, DP P) const
{
    for (auto x : r)                // loop over possible ranges
        if (x.in_range(T))         // is in range?
            return x.expr();      
    
    // The ideal here would be to throw, since T is out of range
//    throw tpfunction_out_of_range ("Out of range in function "+this->name());  
    
    // However, ThermoCalc is rather loose on these T ranges. So, I do the same for now
    if (T<r[0].Tlow()) return r[0].expr();                        // if T is lower than the lowest bound 
    if (T>r[r.size()-1].Thigh()) return r[r.size()-1].expr();     // if T is higher than the highest bound
    
    return "0";                                 // you should never get here...
}

DP TPfunction::calc(List_TPfunctions& list, DP T, DP P, string deriv, int order) const
{
    string expression;
    
    // Search for the appropriate expression to evaluate (according to T ranges)
    for (auto x : r)                // loop over possible ranges
        if (x.in_range(T))         // is in range?
            expression = x.expr();
 
    // The ideal here would be to throw, since T is out of range
    // However, ThermoCalc is rather loose on these T ranges. So, I do the same for now
    if (T<r[0].Tlow()) expression = r[0].expr();                        // if T is lower than the lowest bound 
    if (T>r[r.size()-1].Thigh()) expression = r[r.size()-1].expr();     // if T is higher than the highest bound
    
    // Evaluate the selected expression...
    expression = list.substitute_functions(expression,T,P);      // call the C++ function to eventually substitute in the expression 
                                                                 // functions from the current List_functions object
                                                                 // return an expression containing only T,P variables and "simple"
                                                                 // functions as log, sin, etc.
                        
//    cout<<"Expression: "<<expression<<endl;                    

    // parse function with GiNaC parser and evaluate it
    GiNaC::symbol varT,varP;
    GiNaC::symtab table;
    table["t"] = varT;
    table["p"] = varP;  

    GiNaC::parser reader(table);                // define a GiNaC symbol table
            
    // To use GiNaC, convert ln to log and the whole expression in small letters (for functions such as sin, exp, etc.)
    expression = ln_to_log(to_low(expression));

    GiNaC::ex f = reader(expression);  

    if (to_up(deriv)=="T")         // T derivative
        f = GiNaC::diff(f,varT,order);
    else if (to_up(deriv)=="P")    // P derivative
        f = GiNaC::diff(f,varP,order);
    // if not a T or P derivative, then normal function evaluation
      
    stringstream ss; ss<<f.subs(GiNaC::lst(varT==T,varP==P)).evalf();  // evaluate function f (or its derivatives)
    DP temp; ss>>temp;      // convert to DP
    return temp;    
}


////////////////////////////////////////////////////////////////////////////////
//
//  class List_TPfunctions
//
////////////////////////////////////////////////////////////////////////////////

bool List_TPfunctions::is(const TPfunction& n) const
{
    if (find(n)==end()) return false;
    return true;
}    

bool List_TPfunctions::is(const string n) const
{
    if (find(*new TPfunction(to_up(n)))==end()) return false;
    return true;
} 

TPfunction List_TPfunctions::get(const string n) const
{ 
    auto it=find(TPfunction(n));
    if (it==end()) throw no_such_function();
    return (TPfunction(*it));
}

string List_TPfunctions::list() const
{
     if (size()==0) return "";              // maybe better to throw here?
    string temp;
    for (auto it=begin(); it!=end(); it++) 
       temp+=(*it).name()+",";
    temp.erase(temp.size()-1,temp.size());      // remove last comma
    return temp;
}

void List_TPfunctions::print_TDB(ostream& os)
{
    for (auto it=begin(); it!=end(); it++) 
        (*it).print_TDB(os);
}

void List_TPfunctions::print_TDB(ostream& os) const
{
    for (auto it=begin(); it!=end(); it++) 
        (*it).print_TDB(os);
}

string List_TPfunctions::get_expression(const string n, DP T, DP P) const
{
    auto it=find(TPfunction(n));
    if (it==end()) throw no_such_function();
    return (TPfunction(*it).get_expression(T,P));    
}

string List_TPfunctions::substitute_functions(string expression, DP T, DP P)
{
    // This is not optimal! Just for now.
    string old_expression {expression};
    for (auto it2=begin(); it2!=end(); it2++)   {   
//        cout<<" Expr: "<<expression<<endl;   
        expression = s1_to_s2(expression,it2->name(),get_expression(it2->name(), T, P));
    }    
    if (old_expression!=expression) expression=substitute_functions(expression,T,P); 
    return expression;
}


DP List_TPfunctions::calc(string ph, DP T, DP P, string deriv, int order)
{
    // set all characters to upright
    ph=to_up(ph);
    
    // start processing the input string (can be just the function name or the full command CALCULATE NAME T P DER_VAR ORDER
    stringstream sss(ph);
    string command;
    sss>>command;
    if (command=="CALCULATE") {     // full command
        sss>>ph>>T>>P>>deriv;       // read full format
        if (deriv!="")              // if it is a derivative, read the order (optional, default=1)
           sss>>order; 
    }   
    
    // Search the function in the current list
    auto it=find(TPfunction(ph));
    if (it==end()) throw no_such_function();    
    
    // call the TPfunction::calc function for evaluation and return
    return (*it).calc(*this,T,P,deriv,order);
    
}


