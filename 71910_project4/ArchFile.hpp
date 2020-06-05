//
//  ArchFile.hpp
//  darch
//
//  Created by Desislava Doncheva on 24.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef ArchFile_hpp
#define ArchFile_hpp

#include <stdio.h>

#include "Helpers.hpp"
using namespace std;

//Archiuve file class
class ArchFile
{
    char archFileName[MAX_FILE_NAME];
    bool fileOpenedForRead;
    bool fileOpenedForWrite;
    char password[MAX_PASSWORD];
    FILE *fp;
    HeaderStruct archHeader;
    
    bool hasPassword();
    
    void CheckFilesToAdd(char *path);
    void enumerateFileMask(char *fileMask);
    bool CheckFileMask(char *Filename, char *Mask);
    void writeFileToArch(char* file);
    size_t removeFileToArch(size_t remainingBytes, char *fileToRemove);
    void ListFilesInArch(size_t remainingBytes);
    void ExtractArchFile(size_t remainingBytes, char *pathToExtr);
    void Encrypt(uint8_t *inBlock, uint8_t *outBlock, int BlockSize, char* password);
    void Decrypt(uint8_t *inBlock, uint8_t *outBlock, int BlockSize, char* password);
    void tryCreatePath(char *pathToFile);
    
public:
    ArchFile();
    
    bool Open(char* archFileName, bool createIfNotExist, bool openForRead, bool openForWrite);
    bool isOpened();
    size_t getFileSize();
    void Close();
    bool CheckArchExist(char* archFileName);
    bool CreateArchFileIfNotExist(char* archFileName);
    void setPassword(char* pass);
    
    void ArchCreate (char *filesToAdd);
    void ArchExtract(char *pathToExtract);
    void ArchList();
    void ArchAdd(char *filesToAdd);
    void ArchRemove(char *fileToRemove);
    
    ~ArchFile();
};
#endif /* ArchFile_hpp */
