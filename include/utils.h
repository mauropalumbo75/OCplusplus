/* 
 * File:   utils.h
 * Author: Mauro Palumbo
 *
 * C++ general utilities library
 * To avoid to pollute the calling namespace, I do not use "using std" here...
 * 
 * Created on January 22, 2015, 10:20 AM
 */

#ifndef UTILS_H
#define	UTILS_H

#include <string>
#include <sstream>
#include <vector>
#include "physconsts.h"

#include <iostream>  // only for now

// Error classes for throw
class str_to_double_error {};
class str_to_int_error {};

// template class for value to string conversion
template <class T> 
std::string to_string(T& t)
{
  std::ostringstream os;
  os<<t;
  return os.str();
}

// class for string to/from int or double conversion
template <class T>
bool from_string(T& t,
                 const std::string& s,
                 std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

template <class T>
bool from_string(T& t,
                 const std::string& s)
{
  std::istringstream iss(s);
  return !(iss >> t).fail();
}

// Additional functions for conversion
// maybe to delete later if not used... 
double str_to_double(std::string s);
int str_to_int(std::string s);

// Convert input strint into capital letters
std::string to_up(std::string s);
// Convert input strint into small letters
std::string to_low(std::string s);

// Trim "whitespace" characters from the beginning and end of the input string
// and return the resulting string
// By default, "whitespace" are white spaces, tabulation, carriage return
std::string trim(const std::string& str,
                 const std::string& whitespace = " %\t\n\r");

// Separate string s elements separated by "sep"
// For example, from the string "Cr:Cr:Fe:Cr" and sep=":" returns
// a vector of 4 strings ("Cr","Cr","Fe","Cr")
// Or from the string "Cr,Cr,Fe,Cr" and sep="," returns
// a vector of 4 strings ("Cr","Cr","Fe","Cr")
std::vector<std::string> separate(std::string s,std::string sep);


// get s2 if s1 a subcommand of s2 (including _- characters), else return ""
std::string get_subcommand_extended(std::string s1, std::string s2);

// get s2 if s1 a subcommand of s2 (including _- characters), else return ""
std::string get_subcommand_extended(std::string s1, std::vector<std::string> s2);

// NOT WORKING YET
// check if the input string contains only numbers, mathematical symbols and 
// "e" or "E" for scientific notation
// bool is_only_numbers(std::string s);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Auxiliary functions
// Convert back and forth energy and dos units

double convertdos(double V, std::string eunit="Rydberg");
double convertE(double V, std::string eunit="Rydberg");

#endif	/* UTILS_H */

