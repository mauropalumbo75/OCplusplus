/* 
 * File:   main.cpp
 * Author: mauro
 *
 * Created on February 11, 2015, 2:01 PM
 */

#include <cstdlib>
#include "magnetic.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    DP T, TC, B0;

    cout<<"T= ";    
    while ((cin>>T)&&(T>0)) {
        cout<<"TC = ";  cin>>TC;
        cout<<"B0 = ";  cin>>B0;
        cout<<"Gmagn(T="<<T<<")= "<<Gmagn(T,TC,B0)<<endl;
        cout<<"Hmagn(T="<<T<<")= "<<Hmagn(T,TC,B0)<<endl;
        cout<<"Smagn(T="<<T<<")= "<<Smagn(T,TC,B0)<<endl;
        cout<<"Cpmagn(T="<<T<<")= "<<Cpmagn(T,TC,B0)<<endl;
        cout<<"T= ";
    }

    return 0;
}

