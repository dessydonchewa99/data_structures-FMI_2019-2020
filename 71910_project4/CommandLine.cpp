//
//  CommandLine.cpp
//  darch
//
//  Created by Desislava Doncheva on 23.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include <iostream>
#include <string.h>

#include "CommandLine.hpp"
#include "Helpers.hpp"
#include "ArchFile.hpp"

using namespace std;

//Default constructor
CommandLine::CommandLine()
{
    argCount = 0;
    argStrings = nullptr;
}

//constructor
CommandLine::CommandLine(int argc, char **argv)
{
    int i;
    
    argCount = argc;
    if(argCount > 0)
    {
        argStrings = new char* [argCount];
        
        for(i = 0; i<argCount; i++) //copy command line arguments in a new list
        {
            argStrings[i] = new char[strlen(argv[i])+1];
            strcpy(argStrings[i], argv[i]);
        }
    }
    else
    {
        argStrings = nullptr;
    }
}

//process command archive file commands
void CommandLine::processComand()
{
    bool validCommand = false;
    char password[256];
    strcpy(password, "");
    if(argCount <= 1)
    {
        cout << "Missing command, use 'help'" << endl;
        return;
    }
    if(strcmp(argStrings[1], "create")==0) //create archive command
    {
        validCommand = true;
        if(argCount <= 2)
        {
            cout << "Missing archive name param" << endl;
            return;
        }
        if(argCount <= 3)
        {
            cout << "Missing files to add param" << endl;
            return;
        }
        if(argCount >= 5)
        {
            strcpy(password, argStrings[4]);
        }
        createCmd(argStrings[2], argStrings[3], password);
    }
    if(strcmp(argStrings[1], "extract")==0) //extract archive command
    {
        validCommand = true;
        if(argCount <= 2)
        {
            cout << "Missing archive name param" << endl;
            return;
        }
        if(argCount <= 3)
        {
            cout << "Missing path to extract param" << endl;
            return;
        }
        if(argCount >= 5)
        {
            strcpy(password, argStrings[4]);
        }
        extractCmd(argStrings[2], argStrings[3], password);
        
    }
    if(strcmp(argStrings[1], "list")==0) //list archive commnad
    {
        validCommand = true;
        if(argCount <= 2)
        {
            cout << "Missing archive name param" << endl;
            return;
        }
        if(argCount >= 4)
        {
            strcpy(password, argStrings[3]);
        }
        listCmd(argStrings[2], password);
    }
    if(strcmp(argStrings[1], "add")==0) //add files to existing archive command
    {
        validCommand = true;
        if(argCount <= 2)
        {
            cout << "Missing archive name param" << endl;
            return;
        }
        if(argCount <= 3)
        {
            cout << "Missing files to add param" << endl;
            return;
        }
        if(argCount >= 5)
        {
            strcpy(password, argStrings[4]);
        }
         addCmd(argStrings[2], argStrings[3], password);
    }
    if(strcmp(argStrings[1], "remove")==0) //remove file from archive command
    {
        validCommand = true;
        if(argCount <= 2)
        {
            cout << "Missing archive name param" << endl;
            return;
        }
        if(argCount <= 3)
        {
            cout << "Missing file to remove param" << endl;
            return;
        }
        if(argCount >= 5)
        {
            strcpy(password, argStrings[4]);
        }
        removeCmd(argStrings[2], argStrings[3], password);
        
    }
    if(strcmp(argStrings[1], "help")==0 or strcmp(argStrings[1], "?")==0) //print help wit all possible commands
    {
        validCommand = true;
        helpCmd();
    }
    if(validCommand == false)
    {
        cout << "Invalid command! Use 'help'" << endl;
    }
}

//create archive file object and call corresponding command to process it
void CommandLine::createCmd(char *name_dz, char *filesToAdd, char *password)
{
    ArchFile archFileToCreate;
    archFileToCreate.setPassword(password);
    archFileToCreate.Open(name_dz, true, true, true);
    archFileToCreate.ArchCreate(filesToAdd);
    archFileToCreate.Close();
}
void CommandLine::extractCmd(char *name_dz, char *pathToExtract, char *password)
{
    ArchFile archFileToExtr;
    archFileToExtr.setPassword(password);
    archFileToExtr.Open(name_dz, false, true, false);
    archFileToExtr.ArchExtract(pathToExtract);
    archFileToExtr.Close();
}
void CommandLine::listCmd(char *name_dz, char *password)
{
    ArchFile archFileToList;
    archFileToList.setPassword(password);
    archFileToList.Open(name_dz, false, true, false);
    archFileToList.ArchList();
    archFileToList.Close();
}
void CommandLine::addCmd(char *name_dz, char *filesToAdd, char *password)
{
    ArchFile archFileToAdd;
    archFileToAdd.setPassword(password);
    archFileToAdd.Open(name_dz, true, true, true);
    archFileToAdd.ArchAdd(filesToAdd);
    archFileToAdd.Close();
}
void CommandLine::removeCmd(char *name_dz, char *fileToRemove, char *password)
{
    ArchFile archFileToRemove;
    archFileToRemove.setPassword(password);
    archFileToRemove.Open(name_dz, false, true, true);
    archFileToRemove.ArchRemove(fileToRemove);
    archFileToRemove.Close();
}
//print help
void CommandLine::helpCmd()
{
    cout << "-------------------------------" << endl;
    cout << "darch comands:" << endl;
    cout << "-------------------------------" << endl;
    cout << "darch create <archive.dz> <path[" << PATH_SEPARATOR_STR << "files.*]> [password]" << endl;
    cout << "darch extract <archive.dz> <path_to_extract> [password]" << endl;
    cout << "darch list <archive.dz> [password]" << endl;
    cout << "darch add <archive.dz> <path[" << PATH_SEPARATOR_STR << "files.*]> [password]" << endl;
    cout << "darch remove <archive.dz> <file_name> [password]" << endl;
    cout << "darch help" << endl;
    cout << "-------------------------------" << endl;
    cout << "<required param> and [not reqired param]" << endl;
    cout << "-------------------------------" << endl;
}

//destructor
CommandLine::~CommandLine()
{
    int i = 0;
    if(argCount>0 and argStrings!=nullptr)
    {
        for (i = 0; i<argCount; i++) //delete copies of command line arguments
        {
                delete [] argStrings[i];
        }
        delete [] argStrings;
    }
}
