/* 
 * File:   elements.cpp
 * Author: Mauro Palumbo
 *
 * Created on January 29, 2015, 4:22 PM
 * 
 * 
 */

#include "y.h"


ostream& operator<<(ostream& os, const Y& yy)
{
   return os<<"Y("<<yy.e()<<")="<<scientific<<setw(5)<<yy.y()<<"\n";
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool List_Y::is(const Element& el) const
{
    if (find(Y(el))==end()) return false;
    return true;
}    

bool List_Y::is(const string el) const
{
    if (find(Y(to_up(el)))==end()) return false;
    return true;
}    

Y List_Y::get(const string el) const
{ 
    auto it=find(Element(to_up(el)));
    if (it==end()) throw no_such_element();
    return (*it);
}

DP List_Y::get_Y(const string el) const
{ 
    auto it=find(Element(to_up(el)));
    if (it==end()) throw no_such_element();
    return ((*it).y());
}

Y& List_Y::operator[](const size_t index)
{
    size_t counter=0;
    for (auto it=begin(); it!=end(); it++, counter++) 
        if (counter==index) return (*new Y(*it));
    throw out_of_range();
}
    
const Y& List_Y::operator[](const size_t index) const
{
    size_t counter=0;
    for (auto it=begin(); it!=end(); it++, counter++) 
        if (counter==index) return (*new Y(*it));
    throw out_of_range();
}


string List_Y::list() const
{
    string temp;
    for (auto it=begin(); it!=end(); it++) 
      temp+="Y("+(*it).el().symbol()+")="+(*it).y_s()+"\n";
    return temp;
}


ostream& operator<<(ostream& os, const List_Y& yy)
{
   for (auto it=yy.begin(); it!=yy.end(); it++) 
      os<<"Y("<<(*it).e()<<")="<<scientific<<setw(5)<<(*it).y()<<"\n";
   return os;
}


