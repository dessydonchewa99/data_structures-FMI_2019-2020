//
//  main.cpp
//  darch
//
//  Created by Desislava Doncheva on 23.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include <iostream>
#include "CommandLine.hpp"
#include "Helpers.hpp"
#include "speck.hpp"

using namespace std;

int main(int argc, char **argv)
{
    //create command line processor and process commands
    CommandLine command(argc, argv);
    command.processComand();

    return 0;
}
