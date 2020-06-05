//
//  Helpers.cpp
//  darch
//
//  Created by Desislava Doncheva on 23.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include <string.h>
#include "Helpers.hpp"
#include "sha256.hpp"


//calculates size of the file in blocks of FILE_BLOCK_SIZE
size_t RoundedFileSize(size_t fileSize)
{
    size_t result = fileSize;
    size_t tailFileSize;
    tailFileSize = fileSize % FILE_BLOCK_SIZE;
    if(tailFileSize == 0)
    {
        return result;
    }
    result = ((fileSize+FILE_BLOCK_SIZE)/FILE_BLOCK_SIZE)*FILE_BLOCK_SIZE;
    return result;
    
}

//convert string to upper case
void strToUpper(char* str)
{
    size_t i;
    size_t len = strlen(str);
    for(i=0; i<len; i++)
    {
        str[i] = toupper(str[i]);
    }
}

//constructor
Hash::Hash()
{
    clear();
}

//reset hash
void Hash::clear()
{
    unsigned int i;
    for(i = 0; i < HASH_LEN; i++)
    {
        bytes[i] = 0x00;
    }
}

//callculate HASH function (SHA256)
void Hash::calcHash(char* password)
{
    sha256(password, bytes);
}

//compare HASH
bool Hash::compare(Hash otherHash)
{
    unsigned int i;
    for(i = 0; i < HASH_LEN; i++)
    {
        if(bytes[i] != otherHash.bytes[i])
        {
            return false;
        }
    }
    return true;
}

//---------------

//constructor
HeaderStruct::HeaderStruct()
{
    sign = ARCH_PREFIX ;
    passHash.clear();
}
