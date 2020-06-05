//
//  CommandLine.hpp
//  darch
//
//  Created by Desislava Doncheva on 23.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef CommandLine_hpp
#define CommandLine_hpp

#include <stdio.h>

//command line processor class
class CommandLine
{
private:
    int argCount;
    char** argStrings;
    
    void createCmd(char *name_dz, char *filesToAdd, char *password);
    void extractCmd(char *name_dz, char *pathToExtract, char *password);
    void listCmd(char *name_dz, char *password);
    void addCmd(char *name_dz, char *filesToAdd, char *password);
    void removeCmd(char *name_dz, char *fileToRemove, char *password);
    void helpCmd();
public:
    CommandLine();
    CommandLine(int argc, char **argv);
    void processComand();
    
    ~CommandLine();
    
    
};
#endif /* CommandLine_hpp */
