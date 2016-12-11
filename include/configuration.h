/* 
 * File:   configuration.h
 * Author: Mauro Palumbo
 *
 * A generic class for a configuration of whatever phase (ex. Fe:Fe:Cr:Cr:Cr sigma phase)
 * Extended to include interactions in one or more sublattices (ex. Fe:Fe,Cr:Cr:Cr:Cr )
 * 
 * Created on January 29, 2015, 9:14 AM
 */

#ifndef CONFIGURATION_H
#define	CONFIGURATION_H


#include <string>
#include <vector>
#include <set>
#include <map>
#include "utils.h"
#include "math_utils.h"
#include "physconsts.h"
#include "elements.h"

struct empty_configuration {};
struct wrong_configuration {};

class Configuration {
public:
    // The first two constructors handle only endmember type configurations
    Configuration(List_elements eel) { for (auto x:eel) c.push_back(x); } 
    
    Configuration(vector<string> eel) { for (auto x:eel) c.push_back(Element(x)); }
    
    Configuration(vector<List_elements> lee) : c(lee) { }
   
    // This constructor handles the most general case, including interactions
    Configuration(string conf);
    
    size_t size() const { return c.size(); }
    List_elements& operator[](const size_t index) { return c[index]; }
    const List_elements& operator[](const size_t index) const { return c[index]; }  
//    vector<List_elements>::iterator begin() { return c.begin(); }
//    const vector<List_elements>::iterator end() { return c.end(); }

    bool is_endmember();
    bool is_endmember() const;
//    bool is_ternary();
    
    void push_back(const List_elements& le) { c.push_back(le); }
    
//    vector<bool> flip(const string A, const string B);
    
    string to_string() const;
    string to_string_nocolons() const;
    
private:
    vector<List_elements> c;     // List of elements identifying the configuration (Ex. Fe:Fe:Cr:Cr:Cr)
 };

ostream& operator<<(ostream& os, const Configuration& e);
//vector<Configuration> get_binary_configurations(const Configuration& e);


#endif	/* CONFIGURATION_H */

