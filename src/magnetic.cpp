
#include "../include/magnetic.h"

DP g_tau(DP tau, DP p, DP D)
{
    if (tau<=1) 
        return 1.0-(79.0/(140.0*p*tau)+474.0/497.0*(1.0/p-1.0)*(sqr3(tau)/6.0+sqr3(tau)*sqr3(tau)*sqr3(tau)/135.0
                +sqr4(tau)*sqr4(tau)*sqr4(tau)*sqr3(tau)/600.0))/D;
    else 
        return -(1.0/(sqr4(tau)*tau*10.0)+1.0/(sqr4(tau)*sqr4(tau)*sqr4(tau)*sqr3(tau)*315.0)
                +1.0/(sqr4(sqr4(tau))*sqr3(sqr3(tau))*1500.0))/D;
}

DP dgdtau(DP tau, DP p, DP D)
{
    if (tau<=1) 
        return (79.0/(140.0*p*sqr(tau))+474.0/497.0*(1.0-1.0/p)*(tau*tau/2.0+sqr4(tau)*sqr4(tau)/15.0
                +sqr4(tau)*sqr4(tau)*sqr4(tau)*tau*tau/40.0))/D;
    else 
        return (1.0/(sqr4(tau)*tau*tau*2.0)+1.0/(sqr4(tau)*sqr4(tau)*sqr4(tau)*sqr4(tau)*21.0)
                +1.0/(sqr4(sqr4(tau))*sqr3(sqr3(tau))*tau*60.0))/D;
}

DP Gmagn(DP T, DP TC, DP B0, DP p, DP D, string deriv)
{
    if (deriv=="")
        return R*T*log(B0+1.0)*g_tau(T/TC,p,D);
    else if (deriv=="T")
        return -Smagn(T,TC,B0,p,D);
}

DP Hmagn(DP T, DP TC, DP B0, DP p, DP D)
{
    DP Hmag {0};
    DP tau {T/TC};
    if (tau<=1) 
        Hmag=(-79.0/(140.0*p*tau)+474.0/497.0*(1.0/p-1.0)*(sqr3(tau)/2.0+sqr3(tau)*sqr3(tau)*sqr3(tau)/15.0
                +sqr4(tau)*sqr4(tau)*sqr4(tau)*sqr3(tau)/40.0))/D;
    else 
        Hmag=-(1.0/(sqr4(tau)*tau*2.0)+1.0/(sqr4(tau)*sqr4(tau)*sqr4(tau)*sqr3(tau)*21.0)
                +1.0/(sqr4(sqr4(tau))*sqr3(sqr3(tau))*60.0))/D;
    Hmag=R*T*log(B0+1.0)*Hmag;
    return Hmag;
}


DP Smagn(DP T, DP TC, DP B0, DP p, DP D)
{
    DP Smag {0};
    DP tau {T/TC};
    if (tau<=1) 
        Smag=1.0-(474.0/497.0*(1.0/p-1.0)*(2.0*sqr3(tau)/3.0+2.0*sqr3(tau)*sqr3(tau)*sqr3(tau)/27.0
                +2.0*sqr4(tau)*sqr4(tau)*sqr4(tau)*sqr3(tau)/75.0))/D;
    else 
        Smag=(2.0/(sqr4(tau)*tau*5.0)+2.0/(sqr4(tau)*sqr4(tau)*sqr4(tau)*sqr3(tau)*45.0)
                +2.0/(sqr4(sqr4(tau))*sqr3(sqr3(tau))*125.0))/D;
    Smag=-R*log(B0+1.0)*Smag;
    return Smag;
}

DP Cpmagn(DP T, DP TC, DP B0, DP p, DP D)
{
    DP Cpmag {0};
    DP tau {T/TC};
    if (tau<=1) 
        Cpmag=(474.0/497.0*(1.0/p-1.0)*(2.0*sqr3(tau)+2*sqr3(tau)*sqr3(tau)*sqr3(tau)/3.0
                +2.0*sqr4(tau)*sqr4(tau)*sqr4(tau)*sqr3(tau)/5.0))/D;
    else 
        Cpmag=(2.0/(sqr4(tau)*tau)+2.0/(sqr4(tau)*sqr4(tau)*sqr4(tau)*sqr3(tau)*3.0)
                +2.0/(sqr4(sqr4(tau))*sqr3(sqr3(tau))*5.0))/D;
    Cpmag=R*log(B0+1.0)*Cpmag;
    return Cpmag;
}


