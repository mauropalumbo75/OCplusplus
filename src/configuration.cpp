#include "../include/configuration.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// This version of the constructor assume there is a : symbol among the elements
// which define a configuration (ex. Cr:Re:Cr:Re:Cr) and eventually commas to 
// define interactions among elements in a sublattice (ex. Cr,Re:Re:Cr:Re:Cr)
Configuration::Configuration(string conf)
{
    try {
        vector<string> sublattices;
        sublattices=separate(conf,":");    // split sublattices separated by :
          
        for (auto x : sublattices ) 
            c.push_back(List_elements(separate(x,",")));  // split elements separated by ,
                                // construct a List_elements from the returning vector<string> 
                                // and put it in c   
        if (c.size()==0) throw empty_configuration {};
    }
    catch (no_such_element) {
        throw;
    }
}

bool Configuration::is_endmember()
{
    for (auto x:c)     // loop over the sublattices
        if (x.size()>1) return false;  // if any of the sublattices has more than 1 element, it is not an endmember
    return true;
}

bool Configuration::is_endmember() const
{
    for (auto x:c)     // loop over the sublattices
        if (x.size()>1) return false;  // if any of the sublattices has more than 1 element, it is not an endmember
    return true;
}

/*
bool Configuration::is_ternary()
{
    if (c.size()<3) return false;   // if less than 3 components cannot be a ternary

    // copy to a set of elements
    set<string> temp;
    for (int i=0; i<c.size(); i++)
      temp.insert(c[i].symbol());
   
    if (temp.size()==3) return true;
    else return false;
}
*/

string Configuration::to_string() const
{
    string temp;   
    for (int i=0; i<c.size(); i++) {
        for (int j=0; j<c[i].size(); j++) {
            temp+=c[i][j].symbol()+",";
        }
        if (c[i].size()>0) temp.erase(temp.size()-1,temp.size());  // remove last comma
        temp+=":";        
    }    
    if (temp.size()>0) temp.erase(temp.size()-1,temp.size());    // Remove last :
    
    return temp;
}

string Configuration::to_string_nocolons() const
{
    string temp;   
    for (int i=0; i<c.size(); i++) {
        for (int j=0; j<c[i].size(); j++) {
            temp+=c[i][j].symbol()+",";
        }
        if (c[i].size()>0) temp.erase(temp.size()-1,temp.size());  // remove last comma      
    }       
    return temp;
}

/*
vector<bool> Configuration::flip(const string A, const string B)
{
    vector<bool> temp;
    for (int i=0; i<c.size(); i++) 
        if (c[i].symbol()==A) { c[i]=(*new Element(B)); temp.push_back(true); }
        else temp.push_back(false);
    return temp;
}

ostream& operator<<(ostream& os, const Configuration& e)
{
    for (int i=0; i<e.size(); i++)
        os<<e[i].symbol();
   return os;
}

vector<Configuration> get_binary_configurations(const Configuration& e)
{
    vector<Configuration> tempvec;
    // create a set of unique elements contained in the configuration
    set<string> temp;
    for (int i=0; i<e.size(); i++)
      temp.insert(e[i].symbol());

    set<string>::iterator it, it2;
    for (it=temp.begin(); it!=temp.end(); it++)    // loop over the component to change
        for (it2=temp.begin(); it2!=temp.end(); it2++)  { // loop over the component to change into
            Configuration temp2=e;
            if (*it!=*it2) {
              temp2.flip(*it,*it2);      // flip the component *it with the componenent *it2
              tempvec.push_back(temp2);    // put the flipped configuration in tempvec
            }
        }  
    return tempvec;
}
*/