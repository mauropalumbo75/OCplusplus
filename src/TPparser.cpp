/* 
 * File:   TPparser.cpp
 * Author: Mauro Palumbo
 *
 *
 * Created on July 18, 2014, 9:08 PM
 * 
 */

#include "../include/TPparser.h"


// Implementation of putback function
void Token_stream::putback(Token t)
{
    buffer=t;       // copy t to buffer
    full=true;      // buffer is now full
}

// Ignore all charachters in the Token_stream up to c (included)
void Token_stream::ignore(stringstream& ss, char c)
{
    // first look in buffer
    if (full && c==buffer.kind) {
        full=false;
        return;
    }
    full=false;

    // now search input
    char ch=0;
    while (ss>>ch)
        if (ch==c) return;
}

// Get a Token from Token_stream...
Token Token_stream::get(stringstream& ss, vector<Function>& fun_table)
{
    if (full) {     // do we already have a Token ready?
        // Remove Token from buffer
        full=false;
        return buffer;
    }

    char ch;
    ss >> ch;      // Note: operator >> skip whitespaces, newlines, tabs,...

    switch(ch) {
        case '(':
        case ')':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '^':
        case ';':
            return Token(ch);       // Let each character represent itself
        case '.':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            ss.putback(ch);            // put digit back into the input stream
            double val;
            ss>>val;                   // read a floating point number using the stringstream ss and >>
            return Token(number,val);      // return numeric Token
        }
        default:
            if (isalpha(ch)) {
                string s;
                s+=ch;
                while(ss.get(ch)&&(isalpha(ch)||isdigit(ch))) s+=ch;
                ss.putback(ch);
                if (TPparser::is_fun(s,fun_table)) return Token(func,s);         // function
                return Token(name,s);
            }
            TPparser::error("Bad Token");
    }
}

namespace TPparser {

//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Functions to handle Variables

// Return the value of the Variable named s
double get_value(string s, vector<Variable>& var_table)
{
    for (int i=0;i<var_table.size(); i++)
        if (var_table[i].name==s) return var_table[i].value;
    error("get: undefined variable ",s);
}


// Set the value of Variable s
void set_value(string s, double d, vector<Variable>& var_table)
{
    for (int i=0; i<var_table.size();i++)
        if (var_table[i].name==s) {
            var_table[i].value=d;
            return;
        }
    error("set: undefined variable ",s);
}

// Check if variable s is already defined in var_table
bool is_declared(string s, vector<Variable>& var_table)
{
    for (int i=0;i<var_table.size();i++)
        if (var_table[i].name==s) return true;
    return false;
}

// Add a new variable s to var_table and set its value to val
double define_var(string s, double val, vector<Variable>& var_table)
{
    
    char r='Y';

    if (is_declared(s,var_table)) {
        cout<<"Warning, variable already defined, value= "<<get_value(s,var_table)<<endl;
        cout<<"Assign new value? (Y/N) "<<endl;
        cin>>r;
        if (r=='y'||r=='Y') {
            set_value(s,val,var_table);
            return val;
        }
    }
    var_table.push_back(Variable(s,val));
    return val;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// Functions to handle mathematical functions

// Implementation of a function
double calc_fun(stringstream& ss, Token_stream& ts, vector<Variable>& var_table, vector<Function>& fun_table)
{
    Token t=ts.get(ss,fun_table);
    if (t.kind!=func) error("function expected in calc_fun");

    Token t2=ts.get(ss,fun_table);
    if (t2.kind!='(') error("( expected after a function");

    double d=expression(ss,ts,var_table,fun_table);

    Token t3=ts.get(ss,fun_table);
    if (t3.kind!=')') error(") expected after expression in a function");

    if (t.name=="exp") return exp(d);
    else if (t.name=="log") return log(d);
        else if (t.name=="sin") return sin(d);
            else if (t.name=="cos") return cos(d);
                else if (t.name=="tan") return tan(d);
            else  error("undefined function ",t.name);
}

// Return the true if s is a function, false otherwise
bool is_fun(string s, vector<Function>& fun_table)
{
    for (int i=0;i<fun_table.size(); i++)
        if (fun_table[i].name==s) return true;
    return false;
}

// Add a new variable s to var_table and set its value to val
string define_fun(string s, string fun_expr, vector<Function>& fun_table)
{
    if (is_fun(s,fun_table)) error(s," function already existing ");
    fun_table.push_back(Function(s,fun_expr));
    return s;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
                                            // implementation later...\


double primary(stringstream& ss, Token_stream& ts,vector<Variable>& var_table,vector<Function>& fun_table)
{
    Token t=ts.get(ss,fun_table);
    switch (t.kind) {
        case '(':       // handle expression
        {
            double d=expression(ss,ts,var_table,fun_table);
            t=ts.get(ss,fun_table);
            if (t.kind!=')') error("')' expected");
            return d;
        }
        case number:
            return t.value;
        case name:
            return get_value(t.name,var_table);
        case func:
            ts.putback(t);
            return calc_fun(ss,ts,var_table,fun_table);
        case '-':
            return -primary(ss,ts,var_table,fun_table);
        case '+':
            return primary(ss,ts,var_table,fun_table);
        default:
            error("primary expected");
    }
}


double pot(stringstream& ss, Token_stream& ts,vector<Variable>& var_table,vector<Function>& fun_table)
{
    double left=primary(ss,ts,var_table,fun_table);
    Token t=ts.get(ss,fun_table);           // get the next token from Token_stream

    while(true) {
        switch (t.kind) {
            case '^':
            {
                double d=primary(ss,ts,var_table,fun_table);
                left = pow(left,d);
                t=ts.get(ss,fun_table);
                break;
            }
            default:
                ts.putback(t);      // put back t into the Token_stream
                return left;
        }
    }
}



double term(stringstream& ss, Token_stream& ts,vector<Variable>& var_table,vector<Function>& fun_table)
{
    double left=pot(ss,ts,var_table,fun_table);
    Token t=ts.get(ss,fun_table);           // get the next token from Token_stream

    while(true) {
        switch (t.kind) {
            case '*':
                left *=term(ss,ts,var_table,fun_table);
                t=ts.get(ss,fun_table);
                break;
            case '/':
            {
                double d=pot(ss,ts,var_table,fun_table);
                if (d==0) error ("divide by zero");
                left /= d;
                t=ts.get(ss,fun_table);
                break;
            }
            case '%':
            {
                double d=pot(ss,ts,var_table,fun_table);
                int i1=int(left);
                if (i1!=left) error("left-hand operand of % not int");
                int i2=int(d);
                if (i2!=d) error("right-hand operand of % not int");
                if (i2==0) error("%: divide by zero");
                left=i1%i2;
                t=ts.get(ss,fun_table);
                break;
            }
            default:
                ts.putback(t);      // put back t into the Token_stream
                return left;
        }
    }
}

double expression(stringstream& ss, Token_stream& ts,vector<Variable>& var_table,vector<Function>& fun_table)
{
    double left=term(ss,ts,var_table,fun_table);             // read and evaluate a term
    Token t=ts.get(ss,fun_table);               // get the next Token from the Token_stream

    while(true) {
        switch(t.kind) {
            case '+':
                left += term(ss,ts,var_table,fun_table);     // evaluate term and add
                t=ts.get(ss,fun_table);
                break;
            case '-':
                left -= term(ss,ts,var_table,fun_table);     // evaluate term and subtract
                t=ts.get(ss,fun_table);
                break;
            default:
                ts.putback(t);      // put t back into the Token_stream
                return left;        // finally: no more + or -; return the answer
        }
    }
}

void clean_up(stringstream& ss, Token_stream& ts)
{
    ts.ignore(ss,print);
}


double calculate(string expr, double T, double P) 
{
    Token_stream ts;
    vector<Variable> var_table;
    vector<Function> fun_table;

    try {

        // predefined names:
        define_var("pi",3.1415926535,var_table);
        define_var("e",2.7182818284,var_table);
        define_var("R",8.314,var_table);
                
        define_var("T",T,var_table);
        define_var("P",P,var_table);

        // predefined mathematical functions
        define_fun("exp","exp",fun_table);
        define_fun("log","log",fun_table);
        define_fun("sin","sin",fun_table);
        define_fun("cos","cos",fun_table);
        define_fun("tan","tan",fun_table);


//         for (int i=0;i<fun_table.size(); i++)
//           cout<<"Function name:  "<<fun_table[i].name<<"  expression: "<<fun_table[i].fun_expr<<endl;

         stringstream ss(expr);
    
         while(ss)
         try {
           Token t=ts.get(ss,fun_table);
           while (t.kind==print) t=ts.get(ss,fun_table);   // first discard all "prints"
           ts.putback(t);
           return expression(ss,ts,var_table,fun_table);
         }
//         catch (exception& e) {
         catch (runtime_error& e) {
           cerr<<e.what()<<endl;
           clean_up(ss,ts);
           throw runtime_error("");
         }
    }
    catch (runtime_error& e) {
//        cerr<<e.what()<<endl;
//        return 1;
          throw runtime_error("Something went wrong with this expression");
    }
    catch(...) {
        cerr<<"unknown exception! \n";
        return 2;
    }
}


}