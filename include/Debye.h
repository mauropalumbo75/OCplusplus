/* 
 * File:   Debye.h
 * Author: Mauro Palumbo
 * 
 * This unit contains the functions for thermodynamic functions as in the 
 * Einstein and Debye harmonic models, in the most simple formulation with one
 * constant Einstein (Debye) temperature. 
 * In this harmonic approximation, V=const and P=0 at equilibrium, so that
 * H=U and G=F
 * 
 * The Debye integral is computed using a series,  as in 
 * A. E. Dubinov and A. A. Dubinova, Technical Physics Letters, 2008, 
 * Vol. 34, No. 12, pp. 999–1001
 * The constexpr nDubi determines how many terms in the series are calculated.
 * 
 * Created on February 11, 2015, 3:45 PM
 */

#ifndef DEBYE_H
#define	DEBYE_H

#include "../commoninclude/utils.h"
#include "../commoninclude/math_utils.h"
#include "../commoninclude/physconsts.h"

constexpr DP nDubi {1000};

DP UEinstein(DP T, DP TE);
DP CpEinstein(DP T, DP TE);
DP SEinstein(DP T, DP TE);
DP FEinstein(DP T, DP TE);
DP GEinstein(DP T, DP TE);

DP UDebye(DP T, DP TD);
DP CpDebye(DP T, DP TD);
DP SDebye(DP T, DP TD);
DP FDebye(DP T, DP TD);
DP GDebye(DP T, DP TD);

#endif	/* DEBYE_H */

