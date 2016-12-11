/* 
 * File:   phases.h
 * Author: Mauro Palumbo
 *
 * This file contains several classes and methods for handling crystallographic 
 * structures. Not all classes and methods are full implemented yet.
 * The most important use of this file for now is in importing DFT enthalpies of
 * formation, so it is mainly used for structures names and Wyckoff positions.
 * In the future, other data such as atomic coordinates, lattice vectors, etc.
 * could be implemented and used.
 * 
 * Created on March 15, 2015, 4:55 PM
 */

#ifndef STRUCTURE_H
#define	STRUCTURE_H

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include "../commoninclude/utils.h"
#include "../commoninclude/math_utils.h"


class Vect3D  {
public:
    Vect3D(DP xx, DP yy, DP zz)
        : X(xx), Y(yy), Z(zz)
    {        
    }
    
    Vect3D()
        : X(0.0), Y(0.0), Z(0.0)
    {        
    }
    
    DP x() const { return X; }
    DP y() const { return Y; }
    DP z() const { return Z; }    
        
private:
    DP X,Y,Z;
};

ostream& operator<<(ostream& os, const Vect3D& v);
istream& operator>>(istream& is, Vect3D& vv);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct no_such_structure {};    // to throw an exception

class Structure {
public:
    enum What {
        name, prototype, Pearson, struk, spacegroup, spacegroupnum, nWyck, Wyck, mWyck
    };
    
    
    Structure(string nn,string pp,string pP,string sstr,string ssg,int ssgn,int nnW, vector<string> wW, vector<DP> mmW)
        :n(nn), p(pp), P(pP), str(sstr), sg(ssg), sgn(ssgn), nW(nnW), W(wW), mW(mmW)
    {
    }
    
    Structure(string nn)
        : sgn(0), nW(0)    
    {  
        stringstream s(nn); string command;
        
        s>>command; 
        if (command=="PHASE") s>>command; // get the second string in command sequence
        n=command;
    }
       
       
    Structure()
        : sgn(0), nW(0)    
    {  
    }
    
    string get(const What what, int pos) const;       // get data from class
    int nWyckoff() const { return nW; }
    DP mWyckoff(const int i) const { return mW[i]; } 

private:
    string n;                   // name
    string p;                   // prototype
    string P;                   // Pearson
    string str;                 // Strukturebericht
    string sg;                  // spage group
    int    sgn;                 // space group number
    int    nW;                  // number of Wyckoff positions
    vector<string> W;        // Wyckoff positions (2a, 4f, etc..)
    vector<DP>    mW;       // multiplicity of each Wyckoff position (as real to be more general)
};

ostream& operator<<(ostream& os, const Structure& d);
istream& operator>>(istream& is, Structure& dd);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct Structure_order_alphabetic {
    bool operator()(const Structure& aa, const Structure& bb) const
    {
        return aa.get(Structure::name,0)<bb.get(Structure::name,0);
    }
}; 
    
class List_structures : public set<Structure, Structure_order_alphabetic> {
public:
    Structure get(const string n) const;     
    
    bool read(string fin);              // read structures from file fin
    void print();                       // print all structures in the list
       
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Vect_coord {
public:
    Vect_coord(vector<Vect3D> vv)
       : v(vv)
    {        
    }
    
    Vect_coord()
    {        
    }
    
    size_t size() const  { return v.size(); }
    Vect3D& operator[](const size_t);
    const Vect3D& operator[](const size_t) const;    
    
    void add(Vect3D p);
    
private:
    vector<Vect3D> v;
};

ostream& operator<<(ostream& os, const Vect_coord& d);
istream& operator>>(istream& is, Vect_coord& dd);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Structure_ext : public Structure {
public:
   Structure_ext(Structure& s, Vect3D& vv1, Vect3D& vv2, Vect3D& vv3, vector<Vect_coord>& cc)
   : Structure(s), v1(vv1), v2(vv2), v3(vv3), c(cc)
   {       
   }
           
   Structure_ext()
   {        
   }    
   
    DP v1x() const { return v1.x(); }
    DP v1y() const { return v1.y(); }
    DP v1z() const { return v1.z(); }   
    DP v2x() const { return v2.x(); }
    DP v2y() const { return v2.y(); }
    DP v2z() const { return v2.z(); }  
    DP v3x() const { return v3.x(); }
    DP v3y() const { return v3.y(); }
    DP v3z() const { return v3.z(); }  
    
    Vect_coord& operator[](const size_t);
    const Vect_coord& operator[](const size_t) const;   
   
private:
    Vect3D v1,v2,v3;        // vectors defining the structure
    vector<Vect_coord> c;
};

ostream& operator<<(ostream& os, const Structure_ext& d);

bool read_vectors(vector<Vect3D>& vv, string fin2);
bool read_structures_ext(vector<Structure_ext> & s, string fin);
void print_structures_ext(vector<Structure_ext> & s);


#endif	/* STRUCTURE_H */

