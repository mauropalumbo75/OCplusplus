/* 
 * File:   TPparser.h
 * Author: Mauro Palumbo
 *
 * This is a simple parser of expression
 * It works, but it must be revised before being distributed...
 * 
 * It handles general operators (+,-,*,/). For powers, it used the ^ symbol, not
 * the Fortran **. Be aware of it!
 *  
 * Standard functions defined are exp, log, sin, cos, tan. More can be easily 
 * implemented, if needed.
 * 
 * Created on July 20, 2014, 8:29 PM
 */

#ifndef TPPARSER_H
#define	TPPARSER_H

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>

using namespace std;

const char number='8';      // t.kind==number means that t is a number Token, t.value is its value
const char quit='q';        // t.kind==quit means that t is a quit Token
const char print=';';       // t.kind==print means that t is a print Token
const char name='a';        // t.kind==name means that t is a name Token, t.name is the name
const char let='L';         // t.kind==let means that t is a let Token
const char func='f';        // t.kind==let means that t is a func Token

const string result=";";    // use to indicate that what follows is a result

class Variable {
public:
    string name;
    double value;
    Variable (string n, double v):name(n),value(v) {}
};

class Function {
public:
    string name;
    string fun_expr;
    Function (string n,string fun):name(n),fun_expr(fun) {}
};




class Token {
public:
    char kind;              // what kind of Token?
    double value;           // for numbers, a value
    string name;            // for variables and functions, a name
    Token(char ch)          // constructor, make a Token from a char
            :kind(ch),value(0) {}
    Token(char ch,double val)   // constructor, make a Token from a char and a double
            :kind(ch),value(val) {}
    Token(char ch,string n)   // constructor, make a Token from a char and a name
            :kind(ch),name(n) {}
};

class Token_stream {
public:
    Token_stream()              // Make a Token_stream that reads input expression
      :full(false),buffer(0)  // no Token in buffer
    {
    }
    
    Token get(stringstream& ss, vector<Function>& fun_table);           // get a Token (the function is defined later)
    void putback(Token t);  // put a Token back in the stream
    void ignore(stringstream& ss, char c);    // discard characters up to and including a c
private:
    bool full;              // is there a Token in the buffer?
    Token buffer;           // here is where we keep a Token put back using putback()
};

namespace TPparser {
    

// Parsing functions
double primary(stringstream& ss, Token_stream& ts,vector<Variable>& var_table,vector<Function>& fun_table);
double pot(stringstream& ss, Token_stream& ts,vector<Variable>& var_table,vector<Function>& fun_table);
double term(stringstream& ss, Token_stream& ts,vector<Variable>& var_table,vector<Function>& fun_table);
double expression(stringstream& ss, Token_stream& ts,vector<Variable>& var_table,vector<Function>& fun_table);

// Functions to handle mathematical functions
bool is_fun(string s, vector<Function>& var_fun);
double calc_fun(stringstream& ss, Token_stream& ts, vector<Variable>& var_table, vector<Function>& fun_table);
string define_fun(string s, string fun_expr, vector<Function>& fun_table);

// Functions to handle Variables
double get_value(string s, vector<Variable>& var_table);
void set_value(string s, double d, vector<Variable>& var_table);
bool is_declared(string s, vector<Variable>& var_table);
double define_var(string s, double val, vector<Variable>& var_table);


void clean_up(stringstream& ss, Token_stream& ts);
double calculate(string expr, double T=298.15, double P=1e5);

// error() simply disguises throws:
inline void error(const string& s)
{
 //       cout<<"wrong"<<s<<endl;
 //   throw exception(s);
    throw runtime_error(s);
}

inline void error(const string& s, const string& s2)
{
	error(s+s2);
}

}

#endif	/* TPPARSER_H */

