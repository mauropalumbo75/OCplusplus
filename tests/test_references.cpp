/* 
 * File:   main.cpp
 * Author: mauro
 *
 * Created on February 12, 2015, 2:26 PM
 */

#include <cstdlib>
#include "references.h"


/*
 * 
 */
int main(int argc, char** argv) {
    
    string s1 {"  pippo 'Alan Dinsdale, SGTE Data for Pure Elements, Calphad Vol 15(1991) "
            "p 317-425, also in NPL Report DMA(A)195 Rev. August 1990'"};
    
    string s2 {"REF213  'P. Gustafson, TRITA-MAC 342, (1987); CR-FE-W'"};
    string s3 {" LIST_OF_REFERENCES  NUMBER SOURCE "
            "  REF283  'Alan Dinsdale, SGTE Data for Pure Elements, Calphad Vol 15(1991) p 317-425, "
    " also in NPL Report DMA(A)195 Rev. August 1990' "
    " REF213  'P. Gustafson, TRITA-MAC 342, (1987); CR-FE-W' "
    " REF115  'J-O Andersson, Met Trans A, Vol 19A, (1988) p 1385-1394 "
            " TRITA 0322 (1986); CR-FE-MO' "
    "REF90   'I Ansara, unpublished work (1991); Cr-Si'"};
    
//    Reference r{s1};
//    r.print_TDB();
    
    List_references list {s3};
    list.print_TDB();
            
    return 0;
}

