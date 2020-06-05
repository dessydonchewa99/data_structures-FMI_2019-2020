//
//  Helpers.hpp
//  darch
//
//  Created by Desislava Doncheva on 23.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef Helpers_hpp
#define Helpers_hpp

#include <stdio.h>
#include <iostream>
using namespace std;

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR_STR "\\"
#define PATH_SEPARATOR_CHAR '\\'
#define MKDIR_CMD "mkdir"
#define RWFILEMODE "a+b"
#else
#define PATH_SEPARATOR_STR "/"
#define PATH_SEPARATOR_CHAR '/'
#define MKDIR_CMD "mkdir -p"
#define RWFILEMODE "r+w+b"
#endif

#define MAX_FILE_NAME 512
#define MAX_PASSWORD 100
#define PASS_ALIGN_STR " "
#define PASS_ALIGN_CHAR ' '
#define HASH_LEN 32
#define ARCH_PREFIX 0xDDDD
#define ARCH_PREFIX_LEN 2
#define TEMP_ARCH_FILE "Temp1234.dz"
#define FILE_BLOCK_SIZE 16


size_t RoundedFileSize(size_t fileSize);
void strToUpper(char* str);

//HASH struct
struct Hash
{
    unsigned char bytes[HASH_LEN];
    
    Hash();
    void clear();
    void calcHash(char* password);
    bool compare(Hash otherHash);
};

//Global Archive file header
struct HeaderStruct
{
    uint16_t sign;
    Hash passHash;
    
    HeaderStruct();
};

//Heeader before each file in Archive file
struct FileStruct
{
    uint16_t nameSize;
    char name[MAX_FILE_NAME];
    uint32_t fileSize;
    // file bytes skipped, reading in real time
    
};
#endif /* Helpers_hpp */
