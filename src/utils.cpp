#include "../include/utils.h"

// Another function to convert a string to a double, throw str_to_double_error
// in case of error
double str_to_double(std::string s)
{
    std::istringstream is(s);   // make a stream so that we can read from string s
    double d;
    is>>d;
    if (!is) throw str_to_double_error {};
    return d;
}

// Another function to convert a string to a int, throw str_to_int_error
// in case of error
int str_to_int(std::string s)
{
    std::istringstream is(s);   // make a stream so that we can read from string s
    int i;
    is>>i;
    if (!is) throw str_to_int_error {};
    return i;
}

////////////////////////////////////////////////////////////////////////////////
//
// Some utilities functions
//
////////////////////////////////////////////////////////////////////////////////

// Convert the input string in uppercase
std::string to_up(std::string s)
{
    for (int i=0; i<s.size(); i++)
        s[i]=toupper(s[i]);
    return s;
}

// Convert the input string in lowercase
std::string to_low(std::string s)
{
    for (int i=0; i<s.size(); i++)
        s[i]=tolower(s[i]);
    return s;
}


std::string trim(const std::string& str,
                 const std::string& whitespace)
{
    std::size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    std::size_t strEnd = str.find_last_not_of(whitespace);
    std::size_t strRange = strEnd - strBegin + 1;
    
    return str.substr(strBegin, strRange);;
}


std::vector<std::string> separate(std::string s,std::string sep)
{
    std::vector<std::string> t;
    std::string temp;   
    std::size_t pos=std::string::npos;
    
    if (sep.size()==0) return t;
    
    std::string ssep=sep.substr(0,1);  // get the first (or only) separation character
    while (true) {        
        pos=s.find(ssep);               // find the "sep" string (separation string)
        if ((pos == 0) && (pos != std::string::npos))   // get rid of possible "sep" at the beginning
            s.erase(0,pos+1);
        else if (pos != std::string::npos) {  
            temp=s.substr(0,pos);        
            s.erase(0,pos+1);
            t.push_back(temp);
        } 
        else {                        // Last element or only one
            temp=s.substr(0,s.size());
            if ((temp!=ssep) && (temp!="")) 
                t.push_back(temp);
            break;                     // exit
        }
    }  
    
    if (sep.size()>1) {
        ssep.clear(); ssep=sep.substr(1,std::string::npos);
        std::vector<std::string> vv;
        for (auto x : t) {
            std::vector<std::string> v=separate(x,ssep);
            for (auto y : v) vv.push_back(y);
        }
        return vv;
    }
    
    return t; 
    
}


// Is s1 a subcommand of s2? (including _ separators)
std::string get_subcommand_extended(std::string s1, std::string s2)
{
    std::vector<std::string> sub_strings1=separate(s1,"_-");  // separate into substrings (separation character _)
    std::vector<std::string> sub_strings2=separate(s2,"_-");
    
    if (sub_strings1.size()>sub_strings2.size()) return "";   // s1 cannot have more substrings than s2
    
    for (int i=0; i<sub_strings1.size(); i++)
        for (int j=0; j<sub_strings1[i].size(); j++)
            if (sub_strings1[i][j]!=sub_strings2[i][j]) return "";
    
    return s2;
}

// Is s1 a subcommand of s2? (including _ separators)
std::string get_subcommand_extended(std::string s1, std::vector<std::string> vs2)
{
    std::string full_command;
    int count {0};
    
    for (auto s2 : vs2) {
         std::string s=get_subcommand_extended(s1,s2);
         if (s!="")  { full_command=s; count++; }
    }
    
    if (count==1) return full_command;
    return "";
}

/*
bool is_only_numbers(std::string s)
{
    bool is_num=true;
    bool is_not_white=false;

    for(int i=0; i<s.size(); i++) {
        if (isalpha(s[i])&&(s[i]!='e')&&(s[i]!='E')) is_num=false;     // if the line contains alphanumerical characters
                                                                                        // (except e and E), ignore it...
        if (!isspace(s[i])) is_not_white=true;      // check if the whole line is empty
    }
    
    if ((!is_num)&&(is_not_white)) return true;
}
*/


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


double convertdos(double E, std::string eunit)
{    
    eunit=to_up(eunit);
    if (eunit=="RYDBERG") return E*eV_to_Rydberg;               // data are in 1/Rydberg
    else if (eunit=="J") return E*eV_to_J;                      // data are in 1/J
    else if (eunit=="CM-1") return E/1.0E2/inversem_to_eV;       // data are in 1/cm-1    
    return E;     // data are already in 1/eV
}


double convertE(double E, std::string eunit)
{    
    eunit=to_up(eunit);
    if (eunit=="RYDBERG") return E/eV_to_Rydberg;               // data are in Rydberg
    else if (eunit=="J") return E/eV_to_J;                      // data are in J
    else if (eunit=="CM-1") return E*1.0E2*inversem_to_eV;       // data are in cm-1    
    return E;     // data are already in eV
}

