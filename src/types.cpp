/* 
 * File:   types.cpp
 * Author: Mauro Palumbo
 *
 * Created on February 21, 2015, 8:11 AM
 * 
 */

#include "types.h"


// Most used constructor, expand on an input string
// Examples of use: ss=element name or ss=TDB command for inizializing an element
// (ELEMENT AL         FCC_A1	 2.698154e+01	 4.540000e+03	 2.830000e+01!)
Type::Type(string ss) {
    
    ss=to_up(ss);
    
    // start processing the input string (can be just the element name or the full definition)
    stringstream sss {ss};
    string command, temp;
    sss>>command;
    command=get_subcommand_extended(command,"TYPE_DEFINITION");
    if (command=="TYPE_DEFINITION") {
        sss>>ident;   // read full format
        while (sss>>temp) v.push_back(temp);
    }
}

string Type::full() const
{
    string temp;
    for (auto x : v ) temp+=x+" ";
    return temp;
}    

// Print in TDB format
void Type::print_TDB(ostream& os)
{
   os<<" TYPE_DEFINITION "<<ident<<" "<<this->full()<<"!\n";        
}

// Print in TDB format (const version)
void Type::print_TDB(ostream& os) const
{
   os<<" TYPE_DEFINITION "<<ident<<" "<<this->full()<<"!\n";       
}

ostream& operator<<(ostream& os, const Type& e)
{
   return os<<" TYPE_DEFINITION "<<e.id()<<" "<<e.full()<<"\n";
}

string Type::operator[](const size_t index)
{
    if (index==0) return to_string(ident);
        
    if (index>0 && index<v.size()+1) return v[index-1];
    return "";
}
    
const string Type::operator[](const size_t index) const
{
    if (index==0) return to_string(ident);
        
    if (index>0 && index<v.size()+1) return v[index-1];
    return "";
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool List_types::is(const Type& el) const
{
    if (find(el)==end()) return false;
    return true;
}    

bool List_types::is(const char el) const
{
    if (find(Type(toupper(el)))==end()) return false;
    return true;
}  

Type List_types::get(const char el) const
{ 
    auto it=find(Type(toupper(el)));
    if (it==end()) throw no_such_type("");
    return (*it);
}

string List_types::list() const
{
    if (size()==0) return "";              // maybe better to throw here?
    string temp;
    for (auto it=begin(); it!=end(); it++) 
       temp+=(*it).id();
    return temp;
}

void List_types::print_TDB(ostream& os)
{
    for (auto it=begin(); it!=end(); it++)
        it->print_TDB(os);
}
