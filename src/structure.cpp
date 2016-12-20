/* 
 * File:   structure.cpp
 * Author: Mauro Palumbo
 *
 * Created on May 26, 2014, 3:56 PM
 * 
 */

#include <set>
#include <string>

#include "../include/structure.h"


ostream& operator<<(ostream& os, const Vect3D& v)
{
   return os<<v.x()<<"\t"<<v.y()<<"\t"<<v.z();
}

istream& operator>>(istream& is, Vect3D& vv)
{
    DP xx, yy, zz;              // Auxiliary variables
        
    is>>xx>>yy>>zz;             // read  vectors's coordinates
    
    if(!is) return is;          // something went wrong...
    
    vv=Vect3D(xx,yy,zz);        // all ok, store data and return is            
    return is;  
    
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

string Structure::get(const What what, int pos) const
{
    if (what==name) return n;        
    if (what==prototype) return p; 
    if (what==Pearson) return P; 
    if (what==struk) return str;
    if (what==spacegroup) return sg;
    if (what==spacegroupnum) return to_string(sgn);
    if (what==nWyck) return to_string(nW);  
    if ((what==Wyck)&&pos==-1) {
        string b;
        for (int i=0;i<W.size();i++) 
             b=b+' '+W[i]+' '+to_string(mW[i]);
        return b;
    } else if (what==Wyck) return W[pos];
    else if (what==mWyck)  return to_string(mW[pos]);
        
};

ostream& operator<<(ostream& os, const Structure& d)
{
   os<<d.get(Structure::name,-1)<<"\t"<<d.get(Structure::prototype,-1)<<"\t"<<d.get(Structure::Pearson,-1)
     <<"\t"<<d.get(Structure::struk,-1)<<"\t"<<d.get(Structure::spacegroup,-1)<<"\t"<<d.get(Structure::spacegroupnum,-1)
     <<"\t"<<d.get(Structure::nWyck,-1)<<"\t"<<d.get(Structure::Wyck,-1)<<"\n";
   return os;
}


istream& operator>>(istream& is, Structure& dd)
{
    string n,p,P,str,sg,buf;                                 // Auxiliary variables
    int sgn,nW;                                           
    vector<string> wyck;
    vector<DP> mwyck;
        
    is>>n>>p>>P>>str>>sg>>sgn>>nW;              // read all data up to the  number of Wyckoff positions
    for (int i=0; i<nW; i++)                    // read now Wyckoff positions and multiplicities
    {
        is>>buf;
        wyck.push_back(buf);
        DP temp;
        is>>temp;
        mwyck.push_back(temp);
    }

    if(!is) {                                        // something went wrong..
        return is;
    }                              

    n=to_up(n);
    dd=Structure(n,p,P,str,sg,sgn,nW,wyck,mwyck);   // all ok, store data and return is            
    return is;           
}


///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Structure List_structures::get(const string n) const
{
    auto it=find(*new Structure(n));
    if (it==end()) { cerr<<"Structure "<<n<<"not found!"<<endl;  throw no_such_structure(); }
    return (*it);        
}

bool List_structures::read(string fin)
{
    Structure temp;
    
    // Open input file
    ifstream ist {fin};        //  ist read the file fname
    if (!ist) return false;
    ist.clear();

    string line;
    while(getline(ist,line))            // Read line by line
    {
        stringstream ss(line);        
        if (ss>>temp) {                  // Process line with overloaded op >>
           emplace(temp);
           temp=Structure();   // reset temp
        } else {
            cerr<<"In file "<<fin<<" cannot read line: "<<line<<endl;
            return false;
        }
     }   
    return true;    
}

void List_structures::print()
{
    for (auto it=begin(); it!=end(); it++) 
       cout<<*it;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Vect_coord::add(Vect3D p)
{
    v.push_back(p);
}

Vect3D& Vect_coord::operator[](const size_t index)
{
    return v[index];
}
    
const Vect3D& Vect_coord::operator[](const size_t index) const
{
    return v[index];
}

    
ostream& operator<<(ostream& os, const Vect_coord& d)
{
   for (int i=0; i<d.size(); i++)
       os<<d[i]<<"\n";
   
   return os;
}


istream& operator>>(istream& is, Vect_coord& dd)
{
    Vect3D temp;                                 // Auxiliary variables

    while (is>>temp) {
        if(!is) return is;                       // something went wrong...
        dd.add(temp);       
    } 
       
    return is;           
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Vect_coord& Structure_ext::operator[](const size_t index)
{
    return c[index];
}

const Vect_coord& Structure_ext::operator[](const size_t index) const   
{
    return c[index];
}   

ostream& operator<<(ostream& os, const Structure_ext& d)
{
   // Print structure's info
   os<<d.get(Structure::name,-1)<<"\t"<<d.get(Structure::prototype,-1)<<"\t"<<d.get(Structure::Pearson,-1)
     <<"\t"<<d.get(Structure::struk,-1)<<"\t"<<d.get(Structure::spacegroup,-1)<<"\t"<<d.get(Structure::spacegroupnum,-1)
     <<"\t"<<d.get(Structure::nWyck,-1)<<"\t"<<d.get(Structure::Wyck,-1)<<"\n";
   // Print vectors of the structure
   os<<d.v1x()<<"\t"<<d.v1y()<<"\t"<<d.v1z()<<"\n";
   os<<d.v2x()<<"\t"<<d.v2y()<<"\t"<<d.v2z()<<"\n";
   os<<d.v3x()<<"\t"<<d.v3y()<<"\t"<<d.v3z()<<"\n";
   // Print coordinates of Wyckoff positions
   int n=str_to_int(d.get(Structure::nWyck,-1));
   cout<<"n= "<<n<<"\n";
   for (int i=0; i<n; i++)
       os<<d[i]<<"\n";
   return os;
}


bool read_vectors(vector<Vect3D>& vv, string fin)
{
    Vect3D temp;
       
    // Open input file
    ifstream ist {fin};        //  ist read the file fname
    if (!ist) {
       cerr<<"Input file "<<fin<<" cannot be opened"<<endl;
       return false;
    }

    ist.clear();
    
    string line;
    while(getline(ist,line))            // Read line by line
    {

        stringstream ss(line);
        
        if (ss>>temp) {                  // Process line with overloaded op >>
        vv.push_back(temp);
        temp=Vect3D();
        } else cerr<<"In file "<<fin<<" cannot read line: "<<line<<endl;
        
     }    

//    cout<<"\nRead data from file \""<<fin2<<"\"\n"<<endl;
    return true;
}

bool read_structures_ext(vector<Structure_ext> & s, string fin)
{
    Structure temp;
    
    // Open input file
    ifstream ist {fin};        //  ist read the file fname
    if (!ist) {
       cerr<<"Input file cannot be opened"<<endl;
       return false;
    }

    ist.clear();
 
    string line;
    while(getline(ist,line))            // Read line by line
    {

        stringstream ss(line);
        vector<Vect3D> temp2, temp3;
        vector<Vect_coord> temp4;
    
        // Read Structure data from main database file        
        ss>>temp;               // Process line with overloaded op >>
        
        if (ss) {       // If current line in main database file is correct
                        // read other files
        
           // Read other files (for each structures, file "_vectors" and one file 
           // for each Wyckoff position)
           
            /////////////////////////////////////   read structure vectors
            read_vectors(temp2,temp.get(Structure::name,-1)+"_vectors"); 
        
            /////////////////////////////////////    read coordinates for each Wyckoff position
            int n=str_to_int(temp.get(Structure::nWyck,-1));                         // number of Wyckoff position in the structure
            for (int i=0; i<n; i++)
            {
               temp3.clear();
               read_vectors(temp3,temp.get(Structure::name,-1)+"_"+temp.get(Structure::Wyck,i));    // the second argument is the name of the file to open, ex. C15_2a,..
              temp4.push_back(*new Vect_coord(temp3));
            }
        
            s.push_back(*new Structure_ext(temp,temp2[0],temp2[1],temp2[2],temp4));       // put everything in s...
            temp=Structure();
        } else cerr<<"In file "<<fin<<" cannot read line: "<<line<<endl;
    }

    return true;
}

void print_structures_ext(vector<Structure_ext> & s)
{
    cout<<"Structures loaded:\n"<<endl;    
    for (int i=0;i<s.size();i++) {
       cout<<s[i]<<endl;
    };
}