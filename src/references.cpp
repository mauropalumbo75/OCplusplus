/* 
 * File:   references.cpp
 * Author: Mauro Palumbo
 *
 * Created on February 12, 2015, 2:26 PM
 */

#include "../include/references.h"


Reference::Reference(string refall)
{
    stringstream ss {refall};
    char c;
    
    ss>>n;   // read the first string, should be the reference tag, for example REF1
    ss>>r;
    if (r.size()==0) throw wrong_ref {"Incomplete reference "+n};
    if (r[0]!='\'') throw wrong_ref {"Missing \' in reference "+n};
    while (ss.get(c)) r+=c;    // put the rest of the input string (several substrings of 
                            // the full reference, for example 'P. Gustafson, TRITA-MAC 342, (1987); CR-FE-W') in ref
    if (r[r.size()-1]!='\'') throw wrong_ref {"Missing \' in reference "+n};
      
}


void Reference::print_TDB(ostream& os)
{
    os<<n<<"\t"<<r<<endl;
}

void Reference::print_TDB(ostream& os) const
{
    os<<n<<"\t"<<r<<endl;
}

vector<Reference> handle_refs_list(string s)
{
    vector<Reference> temp;
    stringstream ss {s};
    string tag, ref;
    char c;
    
    ss>>tag;
    tag=get_subcommand_extended(tag,"LIST_OF_REFERENCES");
    if (tag!="LIST_OF_REFERENCES") throw wrong_ref {"Missing or wrong LIST_OF_REFERENCES command"};   // first line must start with LIST_OF_REFERENCES
    while (ss>>tag) {   // read the first string, should be the reference tag, for example REF1
        if (tag=="NUMBER") { ss>>tag; ss>>tag; } // get another string (SOURCE) and do nothing with it 
        ss>>ref;
        if (ref.size()==0) throw wrong_ref {"Incomplete reference "+tag};
        if (ref[0]!='\'') throw wrong_ref {"Missing \' in reference "+tag};
        while (ss.get(c)) {
            ref+=c;    // put the rest of the input string (several substrings of 
                       // the full reference, for example 'P. Gustafson, TRITA-MAC 342, (1987); CR-FE-W') in ref
            if (c=='\'') break;
        }
        if (ref[ref.size()-1]!='\'') throw wrong_ref {"Missing \' in reference "+tag};
        temp.push_back(Reference{tag,ref});
    }
    return temp;
}

List_references::List_references(string s)
{
    vector<Reference> refs;
    refs=handle_refs_list(s);
    for (auto x : refs) insert(x);
}

void List_references::insert(string s)
{
    vector<Reference> refs;
    refs=handle_refs_list(s);
    for (auto x : refs) insert(x);
}

void List_references::print_TDB(ostream& os)
{
    if (this->size()!=0) {
        os<<"LIST_OF_REFERENCES"<<endl;
        os<<"NUMBER  SOURCE"<<endl;
        for (auto it=begin(); it!=end(); it++) 
            it->print_TDB(os);
        os<<" ! ";
    }
}