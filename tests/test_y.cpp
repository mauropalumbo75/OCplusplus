/* 
 * File:   main.cpp
 * Author: mpalumbo
 *
 * Created on January 29, 2015, 4:22 PM
 */

#include <cstdlib>
#include "../include/y.h"
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    try {
    
    Y prova("H",0.1); 
    cout<<"Y("<<prova.el().symbol()<<")="<<prova.y()<<endl;
        
    Y prova2("He",0.4);
    cout<<"Y("<<prova2.el().symbol()<<")="<<prova2.y()<<endl;
    
    Y prova3(prova);
    cout<<"Y("<<prova3.el().symbol()<<")="<<prova3.y()<<endl;
    
    List_Y lista(prova2);
    cout<<lista.list()<<endl;
        
    cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"<<endl;
    
    Y prova4("He",0.5);
    lista.insert(prova4);    
    cout<<lista.list()<<endl;
    
    lista.insert("C",0.123456789);
    lista.insert("Ti",0.5);
    lista.insert("H",0.1);
    lista.insert("Ti",0.9);
    
    cout<<lista.is("C")<<"   "<<lista.is("H")<<endl;
    
    cout<<lista.list()<<endl;
    
    cout<<lista.get("C")<<lista.get("H")<<endl;
    
    lista.insert("C");
        
    cout<<lista.get("C")<<lista.get("H")<<endl;
    
    cout<<"New test:"<<endl;
    cout<<lista[0]<<lista[1]<<lista[2]<<lista[3]<<endl;
    
    cout<<"New new test:"<<endl;
    cout<<lista<<endl;

    lista.clear();
    
    cout<<lista.get("C")<<lista.get("H")<<endl;
   
    }
    catch (no_such_element) { cout<<"No such element!"<<endl; }
    
    return 0;
}

