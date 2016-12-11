/* 
 * File:   magnetic.h
 * Author: Mauro Palumbo
 * 
 * Magnetic functions as in the Inden-Jarl model 
 * 
 *
 * Created on February 11, 2015, 2:02 PM
 */

#ifndef MAGNETIC_H
#define	MAGNETIC_H

#include "../commoninclude/utils.h"
#include "../commoninclude/math_utils.h"
#include "../commoninclude/physconsts.h"

// Magnetic functions as in the Inden-Jarl model
// TC is the Curie temperature, B0 the Bohr magneton, p and D are parameters of the model
// For the BCC phase p=0.40, DP D=1.55828482 default values
// For other common phases p=0.28, DP D=2.342456517 and the values must be provided in input

DP g_tau(DP tau, DP p=0.40, DP D=1.55828482);
DP dgdtau(DP tau, DP p=0.40, DP D=1.55828482);
DP Gmagn(DP T, DP TC, DP B0, DP p=0.40, DP D=1.55828482, string deriv="");
DP Smagn(DP T, DP TC, DP B0, DP p=0.40, DP D=1.55828482);
DP Hmagn(DP T, DP TC, DP B0, DP p=0.40, DP D=1.55828482);
DP Cpmagn(DP T, DP TC, DP B0, DP p=0.40, DP D=1.55828482);

#endif	/* MAGNETIC_H */

