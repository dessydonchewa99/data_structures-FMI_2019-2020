//
//  main.cpp
//  homework2_test
//
//  Created by Desislava Doncheva on 17.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "Stack.hpp"
using namespace std;

bool isInputCorrect(char* compString)
{
    unsigned i = 0;
    int level = 0;
    bool inRightOrder = true;
    bool inKavichki = false;
    
    for(i = 0 ; i < strlen(compString); i++)
    {
        if(compString[i] == '\\')
        {
            i++;
            continue;
        }
        if(compString[i] == '"') // if the char is '"'
        {
            if(inKavichki == false)
            {
                inKavichki = true; // in " "
            }
            else
            {
                inKavichki = false; // out " "
            }
            continue; // ignore '"' and continue
        }
        if(inKavichki) // if in " " takes the char and continue
        {
            continue;
        }
        if(compString[i] == '(')
        {
            level++;
        }
        if(compString[i] == ')')
        {
            level--;
        }
        if(level < 0)
        {
            inRightOrder = false;
        }
    }
    
    if(level == 0 and inRightOrder == true and inKavichki == false)
    {
        return true;
    }
    if(level != 0 or inRightOrder == false)
    {
        cout << "Problem with braces" << endl;
    }
    if(inKavichki == true)
    {
        cout << "Problem with kavichki" << endl;
    }
    return false;
}

//------------------------

void decompress(const char* inStr, char* outStr)
{
    Stack stack(100);
    int count = 0;
    char strCount[10];
    unsigned i = 0;
    char ch, nextCh;
    bool inKavichki = false;
 
    outStr[0] = '\0';
    strCount[0] = '\0';
    for (i = 0 ; i<strlen(inStr); i++)
    {
        ch = inStr[i];
        if(ch == '\\') // checking for '\'
        {
            if(i+1 < strlen(inStr)) // if the input string has one more char
            {
                nextCh = inStr[i+1];
                strncat(outStr, &nextCh, 1); // gets next char as a normal symbol and continue
                i++;
                continue;
            }
        }
        
        if(ch == '"') // if the char is '"'
        {
            if(inKavichki == false)
            {
                inKavichki = true; // in " "
            }
            else
            {
                inKavichki = false; // out " "
            }
            continue; // ignore '"' and continue
        }
        if(inKavichki) // if in " " takes the char and continue
        {
            strncat(outStr, &ch, 1);
            continue;
        }
        
        if (ch >= '0' and ch <= '9') // if the char is a digit
        {
            strncat(strCount, &ch, 1); // add digits one by one
        }
        else
        {
            if (ch == '(') // if the char is '('
            {
                RepeatInfo rpi;
                count = atoi(strCount); // converts a strCount to a whole number
                rpi.count = count; // saves to stack repeat number before '('
                rpi.offset = (unsigned)strlen(outStr); // saves to stack the place where repeating will starts
                stack.push(rpi);
                count = 0;
                strCount[0] = '\0';
            }
            else
            {
                if (ch == ')') // if the char is ')'
                {
                    RepeatInfo rpi;
                    rpi = stack.pop(); // restores from stack imformation for coresponding '('
                    unsigned i = 0;
                    for(i = 0; i < (rpi.count-1); i++) // copies the text between ( and ) n-1 times
                    {
                        int currOutLen = (unsigned)strlen(outStr);
                        while(rpi.offset < currOutLen)
                        {
                            char chTmp = outStr[rpi.offset];
                            strncat(outStr, &chTmp, 1); // copies string char by char
                            rpi.offset++;
                        }
                    }
                }
                else // other char
                {
                    strncat(outStr, &ch, 1);
                }
            }
        }
        
    }
}

//------------------------

int main(int argc, char** argv)
{
    char inputString[1000] = ""; //"W3(A2(C)\\(ZZ)DE\"SS\"Y2(X\\53(Y))dsd12(f)";
    char outputString[3000] = "";
    //cout << "Insert string: ";
   // cin >> inputString;
    
    // read from file
    
    if(argc >= 2)
    {
        ifstream inFile;
        inFile.open(argv[1], ios::in);
        if(inFile.is_open())
        {
            inFile >> inputString;
            inFile.close();
        }
        else
        {
            cout << "Cannot open Read file!" << endl;
            return 1;
        }
    }
    else
    {
        cout << "Missing Read File!" << endl;
        return 1;
    }
    
    cout << "Input string: " << inputString << endl;
    if(isInputCorrect(inputString))
    {
        decompress(inputString, outputString);
        cout << "Output: " << outputString << endl;
    }
    
    // write to file
    
    if(argc >= 3)
    {
        ofstream outFile;
        outFile.open(argv[2], ios::out);
        if(outFile.is_open())
        {
            outFile << outputString;
            outFile.close();
        }
        else
        {
            cout << "Cannot open Write File!" << endl;
            return 1;
        }
    }
    else
    {
         cout << "Missing Write File!" << endl;
         return 1;
    }
    return 0;
}
