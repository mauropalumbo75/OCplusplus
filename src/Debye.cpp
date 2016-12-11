
#include "Debye.h"

DP UEinstein(DP T, DP TE)
{
    DP u298Einst {3.0*R*TE/(exp(TE/298.15)-1.0)};          // This is the energy at 298.15 K in Einstein model
    DP XE {TE/T};
    return 3.0*R*T*XE/(exp(XE)-1.0)-u298Einst;
}

DP CpEinstein(DP T, DP TE)
{
    DP XE {TE/T};
    return 3.0*R*sqr(XE)*exp(XE)/sqr((exp(XE)-1));
}

DP SEinstein(DP T, DP TE)
{
    DP XE {TE/T};
    return 3.0*R*(TE/(exp(TE)-1.0)-log(1.0-exp(-TE)));
}

DP FEinstein(DP T, DP TE)
{
    return UEinstein(T,TE)-SEinstein(T,TE)*T;
}

DP GEinstein(DP T, DP TE)
{
    return UEinstein(T,TE)-SEinstein(T,TE)*T;
}


DP CpDebye(DP T, DP TD)
{
    // First calculate the analytical part
    DP xd {TD/T};			// Here xd is the ratio between Debye T and the present T
    DP first {(1/(exp(-xd)-1))*exp(-xd)*xd};
    DP second {4.0*log(1-exp(-xd))};
    DP third {4.0/15.0*pi*sqr3(pi/xd)};    
    
    // Then calculate the series part
    DP serie1 {0}, serie2 {0}, serie3 {0};
    for (int i=1;i<=nDubi;i++)  {
        serie1=serie1+exp(-xd*i)/sqr(i);
        serie2=serie2+exp(-xd*i)/sqr3(i);
        serie3=serie3+exp(-xd*i)/sqr4(i);
    }
    serie1=serie1*12.0/xd;
    serie2=serie2*24.0/sqr(xd);
    serie3=serie3*24.0/sqr3(xd);
    
    // Sum analytical part and series
    return 9.0*R*(first+second+third-serie1-serie2-serie3);  // This is Cv
}

DP UDebye(DP T, DP TD)
{
    DP u298Debye {9.0/8.0*R*TD+3.0*R*298.15*0.25*(CpDebye(T,TD)/(3.0*R)
    +3.0*(TD/298.15)/(exp(TD/298.15)-1.0))};  // This is the energy at 298.15 K in Debye model
    DP xd {TD/T};			// Here xd is the ratio between Debye T and the present T    
    DP Debyeint {0.25*(CpDebye(T,TD)/(3.0*R)+3.0*xd/(exp(xd)-1.0))};
    return 9.0/8.0*R*TD+3.0*R*T*Debyeint-u298Debye;    
}


DP SDebye(DP T, DP TD)
{
    DP xd {TD/T};			// Here xd is the ratio between Debye T and the present T    
    DP Debyeint {0.25*(CpDebye(T,TD)/(3.0*R)+3.0*xd/(exp(xd)-1.0))};
    return +4.0*R*Debyeint-3.0*R*log(1-exp(-xd));
}

DP FDebye(DP T, DP TD)
{
    return UDebye(T,TD)-SDebye(T,TD)*T;
}

DP GDebye(DP T, DP TD)
{
    return UDebye(T,TD)-SDebye(T,TD)*T;    
}

