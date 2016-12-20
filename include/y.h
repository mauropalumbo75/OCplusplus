/* 
 * File:   y.h
 * Author: Mauro Palumbo
 *
 * This file contains classes and methods for handling the site fractions Ys.
 * 
 * Created on January 29, 2015, 4:22 PM
 */

#ifndef Y_H
#define	Y_H

#include <string>
#include <set>
#include "../include/utils.h"
#include "../include/math_utils.h"
#include "../include/elements.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// A generic class for a site fraction of element A,B,C.. in sublattice 1,2,3 (ex. y_A^1)

class Y {
public:
    Y(Element el, DP yy=0)
      :element(el), site_fraction(yy)
    {        
    }
    
    Y(string el, DP yy=0)
      :element(Element(el)), site_fraction(yy)
    {        
    }
 
    string e() { return element.symbol(); }
    const string e() const { return element.symbol(); }
    Element& el() { return element; }
    const Element& el() const { return element; }
    DP y() const { return site_fraction; }
    string y_s() const { return to_string(site_fraction); }
    
private:
    Element element;
    DP site_fraction;
};

ostream& operator<<(ostream& os, const Y& yy);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct Y_order_alphabetic {
    bool operator()(const Y& a, const Y& b) const
    {
        return a.el().symbol()<b.el().symbol();
    }
}; 

class List_Y : public set<Y, Y_order_alphabetic> {
public:
    List_Y(const Y& s)
    {        
        set<Y, Y_order_alphabetic>::insert(s);   // explicitly refer to set<Y, Y_order_alphabetic>::insert to avoid using 
                                                 // the redefined local one below!
    }
    
    List_Y(const string& s, const DP y=0)
    {        
        set<Y, Y_order_alphabetic>::insert(Y(s,y));
    }
         
    List_Y()
    {        
    }
    
    bool is(const Element& el) const;
    bool is(const string el) const;
    
    Y get(const string el) const; 
    DP get_Y(const string el) const;
    Y& operator[](const size_t);
    const Y& operator[](const size_t) const; 
    
    // for a List_Y, redefine the meaning of insert: if the element is already
    // in the list, overwrite the corresponding site fraction with the new one
    // This is implemented by erasing the old element before inserting the new one
    void insert(const string el, const DP yy=0.0) { erase(Y(el,yy)); 
                     set<Y, Y_order_alphabetic>::insert(Y(el,yy)); }
    size_t insert(const Y& s) { erase(Y(s)); set<Y, Y_order_alphabetic>::insert(Y(s));  }
  
    string list() const;                                // list Ys separated by comma
            
};

ostream& operator<<(ostream& os, const List_Y& yy);

#endif	/* Y_H */

