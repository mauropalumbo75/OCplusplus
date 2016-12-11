/* 
 * File:   elements.cpp
 * Author: Mauro Palumbo
 *
 * Created on January 13, 2015, 11:35 AM
 *  
 */

#include "../include/elements.h"

// Full constructor
Element::Element(string nn, int aa, DP ww=0.0, string rr="", DP hh=0.0, DP ss=0.0)
       : n(nn), a(aa), w(ww), r(rr), h(hh), s(ss)
    {
           // check atomic number
           if ((aa<-1||aa>103)) {
               cerr<<"Wrong atomic number";
               a=0;
           }
    }

// Most used constructor, expand on an input string
// Examples of use: ss=element name or ss=TDB command for inizializing an element
// (ELEMENT AL         FCC_A1	 2.698154e+01	 4.540000e+03	 2.830000e+01!)
Element::Element(string ss) {
    
    ss=to_up(ss);
    
    // start processing the input string (can be just the element name or the full definition)
    stringstream sss {ss};
    string command,element,ref;
    DP at_mass, H, S;  
    sss>>command;
    command=get_subcommand_extended(command,"ELEMENT");
    if (command=="ELEMENT") {
        sss>>element>>ref>>at_mass>>H>>S;   // read full format
        ss=element;
    }

    //  Special element, electron                   
    if (ss=="/-")        { n="/-";   r="ELECTRON_GAS";     a=-1;      w=0;           h=0.0;   s=0.0;    }
    //  Special element, vacancy  
    else if (ss=="VA")  { n="VA";  r="VACUUM";     a=0;       w=0;           h=0.0;   s=0.0;    }
    //                 Symbol,  ref. phase, At. num,    At. mass,    H298,           S298
    else if (ss=="H")  { n="H";  r="GAS";     a=1;      w=1.00794;     h=0.0;        s=0.0;    }
    else if (ss=="HE") { n="HE"; r="GAS";     a=2;      w=4.002602;    h=0.0;        s=0.0;    }
    
    else if (ss=="LI") { n="LI"; r="BCC_A2";  a=3;      w=6.941;       h=4632.00;    s=29.12;  }
    else if (ss=="BE") { n="BE"; r="HCP_A3";  a=4;      w=9.01218;     h=1950.0;     s=9.50;   }
    else if (ss=="B")  { n="B";  r="BET";     a=5;      w=10.81;       h=1222.0;     s=5.9;    }
    else if (ss=="C")  { n="C";  r="HEX_A9";  a=6;      w=12.011;      h=1054.0;     s=5.7423; } 
    else if (ss=="N")  { n="N";  r="GAS";     a=7;      w=14.00674;    h=0.0;        s=0.0;    }
    else if (ss=="O")  { n="O";  r="GAS";     a=8;      w=15.9994;     h=0.0;        s=0.0;    }
    else if (ss=="F")  { n="F";  r="GAS";     a=9;      w=18.9984032;  h=0.0;        s=0.0;    }
    else if (ss=="NE") { n="NE"; r="GAS";     a=10;     w=20.1797;     h=0.0;        s=0.0;    }
    
    else if (ss=="NA") { n="NA"; r="BCC_A2";  a=11;     w=22.98977;    h=6460.0;     s=51.3000;}
    else if (ss=="MG") { n="MG"; r="HCP_A3";  a=12;     w=24.305;      h=4998.0;     s=32.671; }
    else if (ss=="AL") { n="AL"; r="FCC_A1";  a=13;     w=26.98154;    h=4540;       s=28.30;  }
    else if (ss=="SI") { n="SI"; r="DIA_A4";  a=14;     w=28.0855;     h=3217.0;     s=18.81;  }
    else if (ss=="P")  { n="P";  r="WHITE";   a=15;     w=30.97376;    h=5360.0;     s=41.09;  }
    else if (ss=="S")  { n="S";  r="ORTHO";   a=16;     w=32.06;       h=4412.0;     s=32.054; }
    else if (ss=="CL") { n="CL"; r="GAS";     a=17;     w=35.4527;     h=0.0;        s=0.0;    }
    else if (ss=="AR") { n="AR"; r="GAS";     a=18;     w=39.948;      h=0.0;        s=0.0;    }
    
    else if (ss=="K")  { n="K";  r="BCC_A2";  a=19;     w=39.0983;     h=7088.0;     s=64.68;  }
    else if (ss=="CA") { n="CA"; r="FCC_A1";  a=20;     w=40.08;       h=5736.0;     s=41.588; }
    else if (ss=="SC") { n="SC"; r="HCP_A3";  a=21;     w=44.9559;     h=5217.448;   s=34.6435;}
    else if (ss=="TI") { n="TI"; r="HCP_A3";  a=22;     w=47.88;       h=4824.0;     s=30.72;  }
    else if (ss=="V")  { n="V";  r="BCC_A2";  a=23;     w=50.9415;     h=4507.0;     s=30.89;  }
    else if (ss=="CR") { n="CR"; r="BCC_A2";  a=24;     w=51.996;      h=4050.0;     s=23.5429;}
    else if (ss=="MN") { n="MN"; r="BCC_A12"; a=25;     w=54.9380;     h=4995.696;   s=32.2206;}
    else if (ss=="FE") { n="FE"; r="BCC_A2";  a=26;     w=55.847;      h=4489.0;     s=27.2797;}
    else if (ss=="CO") { n="CO"; r="HCP_A3";  a=27;     w=58.9332;     h=4765.567;   s=30.0400;}
    else if (ss=="NI") { n="NI"; r="FCC_A1";  a=28;     w=58.6934;     h=4787.0;     s=29.7955;}
    else if (ss=="CU") { n="CU"; r="FCC_A1";  a=29;     w=63.546;      h=5004.0;     s=33.15;  }
    else if (ss=="ZN") { n="ZN"; r="HCP_A3";  a=30;     w=65.39;       h=5657.0;     s=41.63;  }
    else if (ss=="GA") { n="GA"; r="ORT";     a=31;     w=69.72;       h=5572.0;     s=40.7271;}
    else if (ss=="GE") { n="GE"; r="DIA_A4";  a=32;     w=72.59;       h=4636.0;     s=31.09;  }
    else if (ss=="AS") { n="AS"; r="RHO_A7";  a=33;     w=74.922;      h=5117.032;   s=35.6895;}
    else if (ss=="SE") { n="SE"; r="HEX_A8";  a=34;     w=78.96;       h=5514.512;   s=41.9655;}
    else if (ss=="BR") { n="BR"; r="GAS";     a=35;     w=79.904;      h=0.0;        s=0.0;    }
    else if (ss=="KR") { n="KR"; r="GAS";     a=36;     w=83.80;       h=0.0;        s=0.0;    }
        
    else if (ss=="RB") { n="RB"; r="BCC_A2";  a=37;     w=85.4678;     h=7489;       s=76.78;  }
    else if (ss=="SR") { n="SR"; r="FCC_A1";  a=38;     w=87.62;       h=6568.0;     s=55.694; }
    else if (ss=="Y")  { n="Y";  r="HCP_A3";  a=39;     w=88.9059;     h=5966.384;   s=44.4341;}
    else if (ss=="ZR") { n="ZR"; r="HCP_A3";  a=40;     w=91.22;       h=5566.27;    s=39.1809;}
    else if (ss=="NB") { n="NB"; r="BCC_A2";  a=41;     w=92.9064;     h=5220.0;     s=36.27;  }
    else if (ss=="MO") { n="MO"; r="BCC_A2";  a=42;     w=95.94;       h=4589.0;     s=28.56;  }
    else if (ss=="TC") { n="TC"; r="HCP_A3";  a=43;     w=98.0;        h=0.0;        s=32.9856;}
    else if (ss=="RU") { n="RU"; r="HCP_A3";  a=44;     w=101.07;      h=4602.4;     s=28.6144;}
    else if (ss=="RH") { n="RH"; r="FCC_A1";  a=45;     w=102.9055;    h=4920.384;   s=31.5556;}
    else if (ss=="PD") { n="PD"; r="FCC_A1";  a=46;     w=106.42;      h=5468.488;   s=37.8234;}
    else if (ss=="AG") { n="AG"; r="FCC_A1";  a=47;     w=107.8682;    h=5745.0;     s=42.55;  }
    else if (ss=="CD") { n="CD"; r="HCP_A3";  a=48;     w=112.41;      h=6247.0;     s=51.80;  }
    else if (ss=="IN") { n="IN"; r="TET_A6";  a=49;     w=114.82;      h=6610.0;     s=57.65;  }
    else if (ss=="SN") { n="SN"; r="BCT_A5";  a=50;     w=118.69;      h=6323.0;     s=51.18;  }
    else if (ss=="SB") { n="SB"; r="RHO_A7";  a=51;     w=121.75;      h=5870.152;   s=45.5219;}
    else if (ss=="TE") { n="TE"; r="HEX_A8";  a=52;     w=127.60;      h=6121.192;   s=49.4967;}
    else if (ss=="I")  { n="I";  r="";        a=53;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="XE") { n="XE"; r="GAS";     a=54;     w=0.0;         h=0.0;        s=0.0;    }
    
    else if (ss=="CS") { n="CS"; r="BCC_A2";  a=55;     w=132.9054;    h=7710.000;   s=85.23;  }
    else if (ss=="BA") { n="BA"; r="BCC_A2";  a=56;     w=137.33;      h=6910.0;     s=62.50;  }
    else if (ss=="LA") { n="LA"; r="DHCP";    a=57;     w=138.9055;    h=6665.112;   s=56.9024;}
    else if (ss=="CE") { n="CE"; r="FCC_A1";  a=58;     w=140.12;      h=7280.16;    s=69.4544;}
    else if (ss=="PR") { n="PR"; r="DHCP";    a=59;     w=140.9077;    h=7418.232;   s=73.9313;}
    else if (ss=="ND") { n="ND"; r="DHCP";    a=60;     w=144.24;      h=7133.72;    s=71.0862;}
    else if (ss=="PM") { n="PM"; r="";        a=61;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="SM") { n="SM"; r="RHO";     a=62;     w=150.36;      h=7573.04;    s=69.4962;}
    else if (ss=="EU") { n="EU"; r="BCC_A2";  a=63;     w=151.96;      h=8004.0;     s=80.79304;}
    else if (ss=="GD") { n="GD"; r="HCP_A3";  a=64;     w=157.25;      h=9087.648;   s=67.9482;}
    else if (ss=="TB") { n="TB"; r="HCP_A3";  a=65;     w=158.9254;    h=9426.552;   s=73.3037;}
    else if (ss=="DY") { n="DY"; r="HCP_A3";  a=66;     w=162.50;      h=8865.896;   s=75.5630;}
    else if (ss=="HO") { n="HO"; r="HCP_A3";  a=67;     w=164.9304;    h=7995.624;   s=75.0191;}
    else if (ss=="ER") { n="ER"; r="HCP_A3";  a=68;     w=167.26;      h=7392.2912;  s=73.17816;}
    else if (ss=="TM") { n="TM"; r="HCP_A3";  a=69;     w=168.9342;    h=7397.312;   s=74.01496;}
    else if (ss=="YB") { n="YB"; r="FCC_A1";  a=70;     w=173.04;      h=6711.136;   s=59.8312;}
    else if (ss=="LU") { n="LU"; r="HCP_A3";  a=71;     w=174.967;     h=6388.968;   s=50.9611;}
    else if (ss=="HF") { n="HF"; r="HCP_A3";  a=72;     w=178.49;      h=5845.0;     s=43.56;  }
    else if (ss=="TA") { n="TA"; r="BCC_A2";  a=73;     w=180.9479;    h=5681.872;   s=41.4718;}
    else if (ss=="W")  { n="W";  r="BCC_A2";  a=74;     w=183.85;      h=4970.0;     s=32.6176;}
    else if (ss=="RE") { n="RE"; r="HCP_A3";  a=75;     w=186.207;     h=5355.52;    s=36.5263;}
    else if (ss=="OS") { n="OS"; r="HCP_A3";  a=76;     w=190.2;       h=0;          s=32.6352;}
    else if (ss=="IR") { n="IR"; r="FCC_A1";  a=77;     w=192.22;      h=5267.656;   s=35.5054;}
    else if (ss=="PT") { n="PT"; r="FCC_A1";  a=78;     w=195.08;      h=5723.712;   s=41.6308;}
    else if (ss=="AU") { n="AU"; r="FCC_A1";  a=79;     w=196.9665;    h=6016.592;   s=47.4884;}
    else if (ss=="HG") { n="HG"; r="LIQUID";  a=80;     w=200.59;      h=9342.0;     s=75.90;  }
    else if (ss=="TL") { n="TL"; r="HCP_A3";  a=81;     w=204.383;     h=6831.97;    s=64.2997;}
    else if (ss=="PB") { n="PB"; r="FCC_A1";  a=82;     w=207.2;       h=6870.0;     s=64.80;  }
    else if (ss=="BI") { n="BI"; r="RHO_A7";  a=83;     w=208.9804;    h=6426.624;   s=56.735; }
    else if (ss=="PO") { n="PO"; r="";        a=84;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="AT") { n="AT"; r="";        a=85;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="RN") { n="RN"; r="";        a=86;     w=0.0;         h=0.0;        s=0.0;    }
    
    else if (ss=="FR") { n="FR"; r="";        a=87;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="RA") { n="RA"; r="";        a=88;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="AC") { n="AC"; r="";        a=89;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="TH") { n="TH"; r="FCC_A1";  a=90;     w=232.0381;    h=6350.0;     s=51.8;   }
    else if (ss=="PA") { n="PA"; r="BCT_Aa";  a=91;     w=231.0359;    h=6439.176;   s=51.882; }
    else if (ss=="U")  { n="U";  r="ORT_A20"; a=92;     w=238.0289;    h=6364.0;     s=50.20;  }
    else if (ss=="NP") { n="NP"; r="ORTHO";   a=93;     w=237.0482;    h=6606.536;   s=50.4590;}
    else if (ss=="PU") { n="PU"; r="";        a=94;     w=244;         h=6902.0;     s=54.4610;}
    else if (ss=="AM") { n="AM"; r="DHCP";    a=95;     w=243.0;       h=6407.00;    s=55.3960;}
    else if (ss=="CM") { n="CM"; r="";        a=96;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="BK") { n="BK"; r="";        a=97;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="CF") { n="CF"; r="";        a=98;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="ES") { n="ES"; r="";        a=99;     w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="FM") { n="FM"; r="";        a=100;    w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="MD") { n="MD"; r="";        a=101;    w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="NO") { n="NO"; r="";        a=102;    w=0.0;         h=0.0;        s=0.0;    }
    else if (ss=="LR") { n="LR"; r="";        a=103;    w=0.0;         h=0.0;        s=0.0;    } 
    else throw no_such_element{"I"};
    
    if (command=="ELEMENT") {
        if (ref!=r) { 
            cout<<"Expected ref. state for "<<element<< "="<<r<<". Input value = "<<ref<<". I hope you know what you are doing!"<<endl;
            r=ref;
        }
        if (at_mass!=w) { 
            cout<<"Expected atomic mass for "<<element<< "="<<scientific<<w<<". Input value = "<<at_mass<<". I hope you know what you are doing!"<<endl;
            w=at_mass;
        }
        if (H!=h) { 
            cout<<"Expected enthalpy for "<<element<< "="<<scientific<<h<<". Input value = "<<H<<". I hope you know what you are doing!"<<endl;
            h=H;
        }
        if (S!=s) { 
            cout<<"Expected entropy for "<<element<< "="<<scientific<<s<<". Input value = "<<S<<". I hope you know what you are doing!"<<endl;
            s=S;
        }
    }
}

// Print in TDB format
void Element::print_TDB(ostream& os)
{
   os<<" ELEMENT "<<setw(2)<<n<<"   "<<setw(12)<<r<<"\t "<<scientific<<setw(5)
           <<w<<"\t "<<setw(5)<<h<<"\t "<<setw(5)<<s<<"!\n";        
}

// Print in TDB format (const version)
void Element::print_TDB(ostream& os) const
{
   os<<" ELEMENT "<<setw(2)<<n<<"   "<<setw(12)<<r<<"\t "<<scientific<<setw(5)
           <<w<<"\t "<<setw(5)<<h<<"\t "<<setw(5)<<s<<"!\n";        
}

ostream& operator<<(ostream& os, const Element& e)
{
   return os<<" ELEMENT "<<e.symbol()<<"   "<<e.ref()<<"\t"<<scientific<<setw(5)
           <<e.mass()<<"\t"<<setw(5)<<e.h298()<<"\t"<<setw(5)<<e.s298()<<"\n";
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool List_elements::is(const Element& el) const
{
    if (find(el)==end()) return false;
    return true;
}    

bool List_elements::is(const string el) const
{
    if (find(Element(to_up(el)))==end()) return false;
    return true;
}  

Element& List_elements::operator[](const size_t index)
{
    set<Element, Element_order_alphabetic>::iterator it;
    size_t counter=0;
    for (it=begin(); it!=end(); it++, counter++) 
        if (counter==index) return (*new Element(*it));
    throw out_of_range();
}
    
const Element& List_elements::operator[](const size_t index) const
{
    size_t counter=0;
    for (auto it=begin(); it!=end(); it++, counter++) 
        if (counter==index) return (*new Element(*it));
    throw out_of_range();
}


Element List_elements::get(const string el) const
{ 
    auto it=find(Element(to_up(el),1,0.0));
    if (it==end()) throw no_such_element("");
    return (*it);
}

string List_elements::list() const
{
    if (size()==0) return "";              // maybe better to throw here?
    string temp;
    for (auto it=begin(); it!=end(); it++) 
       temp+=(*it).symbol()+",";
    temp.erase(temp.size()-1,temp.size());      // remove last comma
    return temp;
}

void List_elements::print_TDB(ostream& os)
{
    for (auto it=begin(); it!=end(); it++) 
        (*it).print_TDB(os);
}

