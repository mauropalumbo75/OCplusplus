/* 
 * File:   main.cpp
 * Author: Mauro Palumbo
 *
 * Created on January 16, 2015, 5:32 AM
 */

#include <iostream>
#include "../include/UI.h"

using namespace std;
/*
 * 
 */

int main(int argc, char** argv) {
    

    Systems sys;
    
    Commands commands(sys);

    commands.command_loop();
    
    return 0;
}

