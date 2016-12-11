/* 
 * File:   species.cpp
 * Author: Mauro Palumbo
 *
 * 
 * Created on January 15, 2015, 11:35 AM
 */

#include "../include/species.h"

// Constructor for species
// Accept in input the species definition def and its name
// alternatively, accept the full string definition as first parameter
// (SPECIES H1O1  H1O1)
Species::Species(string def, string name)
{
    int i=0,j=0;
    DP temp_mass,temp_charge;

    def=to_up(def);
    
    // start processing the input string (can be just the element name or the full definition)
    stringstream sss(def);
    string command,definition,nam;
    sss>>command;
    command=get_subcommand_extended(command,"SPECIES");
    if (command=="SPECIES") {
        sss>>nam>>definition;   // read full format
        def=definition; name=nam;
    }     
    
    while (i<def.size()) {
        try {
         string buf_symbol, buf_stoich, buf_charge;
         
         // handle characters
         while (isalpha(def[i])&&(def[i]!='/')&&(i<def.size())) {
           buf_symbol+=def[i];
           i++;
         }
         el.push_back(*new Element(buf_symbol));
         n+=buf_symbol;         // add to symbol name

         // handle digits 
         while ((isdigit(def[i])||def[i]=='.')&&def[i]!='/'&&i<def.size()) {
           buf_stoich+=def[i];
           i++;
         }
         if (buf_stoich=="") throw wrong_species();   // you must define the stoichiometry!
         n+=buf_stoich;         // add to symbol name
         DP temp;
         from_string(temp,buf_stoich);
         stoich.push_back(temp);  
         temp_mass+=el[j].mass()*stoich[j];
        
         // handle charge
         if (def[i]=='/') {       // add charge
            i++;
            while ((isdigit(def[i])||def[i]=='+'||def[i]=='-')&&i<def.size()) {
                buf_charge+=def[i];
                i++;
            }
         temp=0.0;
         from_string(temp,buf_charge);
         temp_charge+=temp*stoich[j];
         }
         j++;
        }
        catch (no_such_element)  { throw wrong_species(); }
    }
    if (name!="") n=name;
    m=temp_mass;
    c=temp_charge;
}

void Species::print_TDB(ostream& os)
{
    os<<" SPECIES "<<n<<" ";
    string temp;
    for (int i=0; i<el.size(); i++)
        temp+=el[i].symbol()+to_string(stoich[i]);
    os<<setw(24)<<temp<<"!\n";
}

// Print in TDB format (const version)
void Species::print_TDB(ostream& os) const
{
    os<<" SPECIES "<<n<<" ";
    string temp;
    for (int i=0; i<el.size(); i++)
        temp+=el[i].symbol()+to_string(stoich[i]);
    os<<setw(24)<<temp<<"!\n";
}

ostream& operator<<(ostream& os, const Species& s)
{
    os<<s.symbol()<<"\t"<<s.mass()<<"\t"<<s.charge();
//    for (int i=0; i<s.size(); i++)
//        os<<"\n"<<s.element(i).symbol()<<s.stoichiometry(i)<<"  ";
    return os;   
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void List_species::insert(const List_elements& listel)
{   
    for (auto it=listel.begin(); it!=listel.end(); it++) 
        insert(*new Species((*it).symbol()+"1"));    
}

bool List_species::is(const Species& sp) const
{
    if (find(sp)==end()) return false;
    return true;
}    

bool List_species::is(const string s) const
{
    if (find(*new Species(to_up(s)))==end()) return false;
    return true;
}    

Species List_species::get(const string sp) const
{ 
    auto it=find(to_up(sp));
    if (it==end()) { throw no_such_species(); }
    return (*it);
}

string List_species::list() const
{
    if (size()==0) return "";              // maybe better to throw here?
    string temp;
    for (auto it=begin(); it!=end(); it++) 
       temp+=(*it).symbol()+",";
    temp.erase(temp.size()-1,temp.size());    // remove last comma
    return temp;
}

void List_species::print_TDB(ostream& os)
{
    for (auto it=begin(); it!=end(); it++) 
        (*it).print_TDB(os);
}






